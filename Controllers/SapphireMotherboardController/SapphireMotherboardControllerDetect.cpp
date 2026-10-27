/*---------------------------------------------------------*\
| SapphireMotherboardControllerDetect.cpp                   |
|                                                           |
|   Sapphire motherboard ARGB support                       |
|   27 Oct 2026                                             |
|                                                           |
|   This file is part of the OpenRGB project                 |
|   SPDX-License-Identifier: GPL-2.0-or-later                 |
\*---------------------------------------------------------*/

#include "DetectionManager.h"
#include "dmiinfo.h"
#include "pci_ids.h"
#include "RGBController_SapphireMotherboard.h"

DetectedControllers DetectSapphireMotherboardControllers(std::vector<i2c_smbus_interface*>& buses)
{
    DetectedControllers detected_controllers;
    DMIInfo board;

    /*-----------------------------------------------------*\
    | Only probe the verified board and AMD primary SMBus. |
    | The USB device is not the TriXX-M lighting transport. |
    \*-----------------------------------------------------*/
    if(board.getManufacturer() != "SAPPHIRE" || board.getMainboard() != "SAPPHIRE NITRO+ B850M WIFI")
    {
        return(detected_controllers);
    }

    for(unsigned int i = 0; i < buses.size(); i++)
    {
        i2c_smbus_interface* bus = buses[i];
        if(bus->info.pci_vendor != AMD_VEN || bus->info.pci_device != AMD_FCH_SMBUS_DEV || bus->info.port_id != 0)
        {
            continue;
        }

        int current_mode = bus->i2c_smbus_read_byte_data(SAPPHIRE_MB_ADDRESS, SAPPHIRE_MB_REG_MODE);
        int brightness   = bus->i2c_smbus_read_byte_data(SAPPHIRE_MB_ADDRESS, SAPPHIRE_MB_REG_BRIGHTNESS);
        if(current_mode < 0 || current_mode > 0x0E || brightness < 0 || brightness > 10)
        {
            continue;
        }

        SapphireMotherboardController* controller = new SapphireMotherboardController(bus);
        detected_controllers.push_back(new RGBController_SapphireMotherboard(controller));
        break;
    }

    return(detected_controllers);
}

REGISTER_I2C_DETECTOR("Sapphire NITRO+ B850M WIFI", DetectSapphireMotherboardControllers);
