/*---------------------------------------------------------*\
| SapphireMotherboardController.cpp                         |
|                                                           |
|   Sapphire motherboard ARGB support                       |
|   27 Oct 2026                                             |
|                                                           |
|   This file is part of the OpenRGB project                 |
|   SPDX-License-Identifier: GPL-2.0-or-later                 |
\*---------------------------------------------------------*/

#include <chrono>
#include <thread>
#include "LogManager.h"
#include "SapphireMotherboardController.h"

SapphireMotherboardController::SapphireMotherboardController(i2c_smbus_interface* bus_ptr)
{
    bus = bus_ptr;
}

std::string SapphireMotherboardController::GetLocation()
{
    return("I2C: " + std::string(bus->info.device_name) + ", address 0x29");
}

int SapphireMotherboardController::ReadRegister(unsigned char reg)
{
    return(bus->i2c_smbus_read_byte_data(SAPPHIRE_MB_ADDRESS, reg));
}

bool SapphireMotherboardController::WriteRegister(unsigned char reg, unsigned char value)
{
    if(bus->i2c_smbus_write_byte_data(SAPPHIRE_MB_ADDRESS, reg, value) < 0)
    {
        LOG_WARNING("[Sapphire Motherboard] Failed to write register 0x%02X", reg);
        return(false);
    }

    /*-----------------------------------------------------*\
    | Allow the MCU to process each write, as TriXX-M does. |
    \*-----------------------------------------------------*/
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    return(true);
}

bool SapphireMotherboardController::SetMode(unsigned char mode, unsigned char speed, unsigned char brightness)
{
    if((brightness > 10) || (speed > 9))
    {
        return(false);
    }

    switch(mode)
    {
        case SAPPHIRE_MB_MODE_RAINBOW:
            if(!WriteRegister(SAPPHIRE_MB_REG_RAINBOW_SPEED, speed))
            {
                return(false);
            }
            break;

        case SAPPHIRE_MB_MODE_BREATHING:
            if(!WriteRegister(SAPPHIRE_MB_REG_EFFECT_SPEED, speed))
            {
                return(false);
            }
            break;

        case SAPPHIRE_MB_MODE_STATIC:
        case SAPPHIRE_MB_MODE_OFF:
            break;

        default:
            return(false);
    }

    return(WriteRegister(SAPPHIRE_MB_REG_BRIGHTNESS, brightness)
        && WriteRegister(SAPPHIRE_MB_REG_MODE, mode));
}

bool SapphireMotherboardController::SetColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char mode)
{
    if((mode != SAPPHIRE_MB_MODE_STATIC) && (mode != SAPPHIRE_MB_MODE_BREATHING))
    {
        return(false);
    }

    /*-----------------------------------------------------*\
    | Reapply the mode to latch the new RGB values.         |
    \*-----------------------------------------------------*/
    return(WriteRegister(SAPPHIRE_MB_REG_RED, red)
        && WriteRegister(SAPPHIRE_MB_REG_GREEN, green)
        && WriteRegister(SAPPHIRE_MB_REG_BLUE, blue)
        && WriteRegister(SAPPHIRE_MB_REG_MODE, mode));
}
