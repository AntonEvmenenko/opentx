/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "opentx.h"

extern "C" {
#include "usb/device/hid-joystick/HIDDJoystickDriver.h"
unsigned char HIDDJoystickDriver_ChangeJoystickState(const HIDDJoystickInputReport *report);
}

void usbJoystickInit()
{
  HIDDJoystickDriver_Initialize();
}

void usbJoystickDeinit()
{

}

void usbJoystickUpdate()
{
  static uint8_t HID_Buffer[11];

  HID_Buffer[0] = 0;
  HID_Buffer[1] = 0;
  HID_Buffer[2] = 0;
  for (int i = 0; i < 8; ++i) {
    if ( channelOutputs[i+8] > 0 ) {
      HID_Buffer[0] |= (1 << i);
    }
    if ( channelOutputs[i+16] > 0 ) {
      HID_Buffer[1] |= (1 << i);
    }
    if ( channelOutputs[i+24] > 0 ) {
      HID_Buffer[2] |= (1 << i);
    }
  }

  //analog values
  //uint8_t * p = HID_Buffer + 1;
  for (int i = 0; i < 8; ++i) {
    int16_t value = channelOutputs[i] / 8;
    if ( value > 127 ) value = 127;
    else if ( value < -127 ) value = -127;
    HID_Buffer[i+3] = static_cast<int8_t>(value);
  }

  HIDDJoystickDriver_ChangeJoystickState((HIDDJoystickInputReport*)&HID_Buffer);
}
