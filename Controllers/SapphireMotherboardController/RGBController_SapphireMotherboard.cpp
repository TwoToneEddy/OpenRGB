/*---------------------------------------------------------*\
| RGBController_SapphireMotherboard.cpp                     |
|                                                           |
|   Sapphire motherboard ARGB support                       |
|   27 Oct 2026                                             |
|                                                           |
|   This file is part of the OpenRGB project                 |
|   SPDX-License-Identifier: GPL-2.0-or-later                 |
\*---------------------------------------------------------*/

#include "RGBController_SapphireMotherboard.h"

/**------------------------------------------------------------------*\
    @name Sapphire NITRO+ B850M WIFI
    @category Motherboard
    @type SMBus
    @save :x:
    @direct :x:
    @effects :white_check_mark:
    @detectors DetectSapphireMotherboardControllers
    @comment All motherboard ARGB headers share one lighting zone.
\*-------------------------------------------------------------------**/

RGBController_SapphireMotherboard::RGBController_SapphireMotherboard(SapphireMotherboardController* controller_ptr)
{
    controller  = controller_ptr;
    name        = "SAPPHIRE NITRO+ B850M WIFI";
    vendor      = "Sapphire";
    type        = DEVICE_TYPE_MOTHERBOARD;
    description = "Sapphire motherboard ARGB controller";
    location    = controller->GetLocation();

    const char* mode_names[] = { "Static", "Rainbow", "Breathing", "Off" };
    const unsigned int mode_values[] =
    {
        SAPPHIRE_MB_MODE_STATIC, SAPPHIRE_MB_MODE_RAINBOW,
        SAPPHIRE_MB_MODE_BREATHING, SAPPHIRE_MB_MODE_OFF
    };

    int current_mode = controller->ReadRegister(SAPPHIRE_MB_REG_MODE);
    int brightness   = controller->ReadRegister(SAPPHIRE_MB_REG_BRIGHTNESS);

    for(unsigned int i = 0; i < 4; i++)
    {
        mode new_mode;
        new_mode.name           = mode_names[i];
        new_mode.value          = mode_values[i];
        new_mode.flags          = MODE_FLAG_HAS_BRIGHTNESS;
        new_mode.brightness_min = 0;
        new_mode.brightness_max = 10;
        new_mode.brightness     = (brightness >= 0 && brightness <= 10) ? brightness : 10;
        new_mode.color_mode     = MODE_COLORS_NONE;

        if(i == 0 || i == 2)
        {
            new_mode.flags     |= MODE_FLAG_HAS_PER_LED_COLOR;
            new_mode.color_mode = MODE_COLORS_PER_LED;
        }
        if(i == 1 || i == 2)
        {
            int speed = controller->ReadRegister(i == 1 ? SAPPHIRE_MB_REG_RAINBOW_SPEED : SAPPHIRE_MB_REG_EFFECT_SPEED);
            new_mode.flags     |= MODE_FLAG_HAS_SPEED;
            new_mode.speed_min  = 9;
            new_mode.speed_max  = 0;
            new_mode.speed      = (speed >= 0 && speed <= 9) ? speed : 3;
        }
        if(i == 3)
        {
            new_mode.flags = 0;
        }
        modes.push_back(new_mode);
        if(current_mode == (int)new_mode.value)
        {
            active_mode = i;
        }
    }

    SetupZones();

    int red   = controller->ReadRegister(SAPPHIRE_MB_REG_RED);
    int green = controller->ReadRegister(SAPPHIRE_MB_REG_GREEN);
    int blue  = controller->ReadRegister(SAPPHIRE_MB_REG_BLUE);
    if(red >= 0 && green >= 0 && blue >= 0)
    {
        colors[0] = ToRGBColor(red, green, blue);
    }
}

RGBController_SapphireMotherboard::~RGBController_SapphireMotherboard()
{
    Shutdown();
    delete controller;
}

void RGBController_SapphireMotherboard::SetupZones()
{
    zone new_zone;
    new_zone.name       = "ARGB Headers";
    new_zone.type       = ZONE_TYPE_SINGLE;
    new_zone.leds_min   = 1;
    new_zone.leds_max   = 1;
    new_zone.leds_count = 1;
    zones.push_back(new_zone);

    led new_led;
    new_led.name = "ARGB Headers";
    leds.push_back(new_led);
    SetupColors();
}

void RGBController_SapphireMotherboard::DeviceUpdateLEDs()
{
    controller->SetColor(RGBGetRValue(colors[0]), RGBGetGValue(colors[0]), RGBGetBValue(colors[0]), modes[active_mode].value);
}

void RGBController_SapphireMotherboard::DeviceUpdateZoneLEDs(int /*zone*/)
{
    DeviceUpdateLEDs();
}

void RGBController_SapphireMotherboard::DeviceUpdateSingleLED(int /*led*/)
{
    DeviceUpdateLEDs();
}

void RGBController_SapphireMotherboard::DeviceUpdateMode()
{
    mode& current = modes[active_mode];
    if(controller->SetMode(current.value, current.speed, current.brightness))
    {
        DeviceUpdateLEDs();
    }
}
