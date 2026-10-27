/* Sapphire motherboard transport regression tests. SPDX-License-Identifier: GPL-2.0-or-later */
#include <cassert>
#include <cstring>
#include <vector>
#include "LogManager.h"
#include "SapphireMotherboardController.h"

/* Isolate logging from the application and network server. */
static unsigned int warnings = 0;
LogManager::LogManager() {}
LogManager::~LogManager() {}
LogManager* LogManager::get()
{
    static LogManager logger;
    return &logger;
}
void LogManager::LogEntry(const char*, int, unsigned int, const char*, ...)
{
    warnings++;
}

struct Write
{
    unsigned char reg;
    unsigned char value;
};

class MockBus : public i2c_smbus_interface
{
public:
    std::vector<Write> writes;
    int fail_at = -1;
    unsigned char registers[256] = {};

    s32 i2c_smbus_xfer(u8 address, char rw, u8 reg, int size, i2c_smbus_data* data) override
    {
        assert(address == 0x29);
        assert(size == I2C_SMBUS_BYTE_DATA);
        if(rw == I2C_SMBUS_READ)
        {
            data->byte = registers[reg];
            return 0;
        }
        writes.push_back({ reg, data->byte });
        return (int)writes.size() == fail_at ? -1 : 0;
    }
    s32 i2c_xfer(u8, char, int*, u8*) override
    {
        assert(false);
        return -1;
    }
};

int main()
{
    MockBus bus;
    SapphireMotherboardController controller(&bus);
    assert(bus.writes.empty());
    bus.registers[0x10] = 6;
    assert(controller.ReadRegister(0x10) == 6);
    assert(bus.writes.empty());

    assert(controller.SetColor(0x12, 0x34, 0x56, 6));
    assert(bus.writes.size() == 4);
    assert(bus.writes[0].reg == 0x1A && bus.writes[0].value == 0x12);
    assert(bus.writes[1].reg == 0x1B && bus.writes[1].value == 0x34);
    assert(bus.writes[2].reg == 0x1C && bus.writes[2].value == 0x56);
    assert(bus.writes[3].reg == 0x10 && bus.writes[3].value == 6);

    /* A failed channel write must not latch a partial RGB triplet. */
    for(int failure = 1; failure <= 4; failure++)
    {
        bus.writes.clear();
        bus.fail_at = failure;
        assert(!controller.SetColor(1, 2, 3, 8));
        assert((int)bus.writes.size() == failure);
    }
    assert(warnings == 4);
    bus.fail_at = -1;
    bus.writes.clear();
    assert(!controller.SetMode(0xFF, 3, 10));
    assert(!controller.SetMode(0, 10, 10));
    assert(!controller.SetMode(0, 3, 11));
    assert(!controller.SetColor(1, 2, 3, 7));
    assert(!controller.SetColor(1, 2, 3, 0));
    assert(bus.writes.empty());

    assert(controller.SetMode(0, 9, 10));
    assert(bus.writes.size() == 3);
    assert(bus.writes[0].reg == 0x15 && bus.writes[0].value == 9);
    assert(bus.writes[1].reg == 0x3E && bus.writes[1].value == 10);
    assert(bus.writes[2].reg == 0x10 && bus.writes[2].value == 0);
    bus.writes.clear();
    assert(controller.SetMode(8, 0, 0));
    assert(bus.writes[0].reg == 0x1D && bus.writes[0].value == 0);
    assert(bus.writes[1].reg == 0x3E && bus.writes[1].value == 0);
    assert(bus.writes[2].reg == 0x10 && bus.writes[2].value == 8);
    bus.writes.clear();
    bus.fail_at = 1;
    assert(!controller.SetMode(0, 3, 10));
    assert(bus.writes.size() == 1);
    bus.fail_at = -1;
    bus.writes.clear();
    assert(controller.SetMode(7, 0, 10));
    assert(bus.writes.size() == 2);
    assert(bus.writes.back().reg == 0x10 && bus.writes.back().value == 7);
}
