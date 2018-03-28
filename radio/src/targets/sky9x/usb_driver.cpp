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
#include "usb/device/massstorage/MSDDriver.h"
extern void MSDDriver_Reset(void);
extern void USBD_Connect(void);
extern void USBD_Disconnect(void);
}

static usbMode selectedUsbMode = USB_UNSELECTED_MODE;
static bool usbDriverStarted = false;

static void configureUsbClock(void)
{
    /* Enable PLLB for USB */
    PMC->CKGR_PLLBR = CKGR_PLLBR_DIVB(1)
                    | CKGR_PLLBR_MULB(7)
                    | CKGR_PLLBR_PLLBCOUNT_Msk;
    while((PMC->PMC_SR & PMC_SR_LOCKB) == 0); // TODO  && (timeout++ < CLOCK_TIMEOUT));
    /* USB Clock uses PLLB */
    PMC->PMC_USB = PMC_USB_USBDIV(1)    /* /2   */
                 | PMC_USB_USBS;        /* PLLB */
}

void usbStart()
{
  configureUsbClock();

  if (selectedUsbMode == USB_JOYSTICK_MODE) {
    usbJoystickInit();
  } else if (selectedUsbMode == USB_MASS_STORAGE_MODE) {
    usbMassStorageInit();
  }

  USBD_Connect();
  usbDriverStarted = true;
}

void usbStop()
{
  if (selectedUsbMode == USB_JOYSTICK_MODE) {
    usbJoystickDeinit();
  } else if (selectedUsbMode == USB_MASS_STORAGE_MODE) {
    usbMassStorageDeinit();
  }

  USBD_Disconnect();
  usbDriverStarted = false;
}

bool usbStarted()
{
  return usbDriverStarted;
}

int getSelectedUsbMode()
{
  return selectedUsbMode;
}

void setSelectedUsbMode(int mode)
{
  selectedUsbMode = usbMode(mode);
}

void usbPluggedIn()
{
  usbMassStorage();
}

extern "C" void USBDDriverCallbacks_ConfigurationChanged(unsigned char cfgnum)
{
  if (selectedUsbMode == USB_JOYSTICK_MODE) {
    (void)cfgnum;
  } else if (selectedUsbMode == USB_MASS_STORAGE_MODE){
    MSDDriver_Reset();
  }
}

extern "C" void USBDCallbacks_RequestReceived(const USBGenericRequest *request)
{
  if (selectedUsbMode == USB_JOYSTICK_MODE) {
    HIDDJoystickDriver_RequestHandler(request);
  } else if (selectedUsbMode == USB_MASS_STORAGE_MODE){
    MSDDriver_RequestHandler(request);
  }
}
