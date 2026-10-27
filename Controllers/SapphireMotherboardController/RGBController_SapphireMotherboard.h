/*---------------------------------------------------------*\
| RGBController_SapphireMotherboard.h                       |
|                                                           |
|   Sapphire motherboard ARGB support                       |
|   27 Oct 2026                                             |
|                                                           |
|   This file is part of the OpenRGB project                 |
|   SPDX-License-Identifier: GPL-2.0-or-later                 |
\*---------------------------------------------------------*/

#pragma once

#include "RGBController.h"
#include "SapphireMotherboardController.h"

class RGBController_SapphireMotherboard : public RGBController
{
public:
    RGBController_SapphireMotherboard(SapphireMotherboardController* controller_ptr);
    ~RGBController_SapphireMotherboard();

    void SetupZones();
    void DeviceUpdateLEDs() override;
    void DeviceUpdateZoneLEDs(int zone) override;
    void DeviceUpdateSingleLED(int led) override;
    void DeviceUpdateMode() override;

private:
    SapphireMotherboardController* controller;
};
