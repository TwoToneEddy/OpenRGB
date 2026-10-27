/*---------------------------------------------------------*\
| SapphireMotherboardController.h                           |
|                                                           |
|   Sapphire motherboard ARGB support                       |
|   27 Oct 2026                                             |
|                                                           |
|   This file is part of the OpenRGB project                 |
|   SPDX-License-Identifier: GPL-2.0-or-later                 |
\*---------------------------------------------------------*/

#pragma once

#include <string>
#include "i2c_smbus.h"

#define SAPPHIRE_MB_ADDRESS             0x29
#define SAPPHIRE_MB_REG_MODE            0x10
#define SAPPHIRE_MB_REG_RAINBOW_SPEED   0x15
#define SAPPHIRE_MB_REG_RED             0x1A
#define SAPPHIRE_MB_REG_GREEN           0x1B
#define SAPPHIRE_MB_REG_BLUE            0x1C
#define SAPPHIRE_MB_REG_EFFECT_SPEED    0x1D
#define SAPPHIRE_MB_REG_BRIGHTNESS      0x3E

#define SAPPHIRE_MB_MODE_RAINBOW        0x00
#define SAPPHIRE_MB_MODE_STATIC         0x06
#define SAPPHIRE_MB_MODE_OFF            0x07
#define SAPPHIRE_MB_MODE_BREATHING      0x08

class SapphireMotherboardController
{
public:
    SapphireMotherboardController(i2c_smbus_interface* bus);

    std::string GetLocation();
    int         ReadRegister(unsigned char reg);
    bool        SetMode(unsigned char mode, unsigned char speed, unsigned char brightness);
    bool        SetColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char mode);

private:
    i2c_smbus_interface* bus;
    bool WriteRegister(unsigned char reg, unsigned char value);
};
