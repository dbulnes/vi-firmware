#include "commands/device_id_command.h"
#include "config.h"
#include "diagnostics.h"
#include "interface/usb.h"
#include "util/log.h"
#include "config.h"
#include "pb_decode.h"
#include <payload/payload.h>
#include "signals.h"
#include <can/canutil.h>
#include <bitfield/bitfield.h>
#include <limits.h>
#include "interface/ble.h"
#include "platform_profile.h"

using openxc::util::log::debug;
using openxc::config::getConfiguration;
using openxc::payload::PayloadFormat;
using openxc::signals::getCanBuses;
using openxc::signals::getCanBusCount;
using openxc::signals::getSignals;
using openxc::signals::getSignalCount;
using openxc::signals::getCommands;
using openxc::signals::getCommandCount;
using openxc::can::lookupBus;
using openxc::can::lookupSignal;

namespace can = openxc::can;
namespace payload = openxc::payload;
namespace config = openxc::config;
namespace diagnostics = openxc::diagnostics;
namespace usb = openxc::interface::usb;
namespace ble = openxc::interface::ble;
namespace uart = openxc::interface::uart;
namespace pipeline = openxc::pipeline;

bool openxc::commands::handleDeviceIdCommmand() {
    // TODO move getDeviceId to openxc::platform, allow each platform to
    // define where the device ID comes from.
    
#ifdef BLE_SUPPORT //deserialize mac address to do read from device flash
    //return the mac address of the device
    char ids[32];
    ble::BleDevice* ble = getConfiguration()->ble;
    
    sprintf(ids,"%02X:%02X:%02X:%02X:%02X:%02X", 
        ble->blesettings.bdaddr[0],ble->blesettings.bdaddr[1],ble->blesettings.bdaddr[2], 
        ble->blesettings.bdaddr[3],ble->blesettings.bdaddr[4],ble->blesettings.bdaddr[5]
    );
    sendCommandResponse(openxc_ControlCommand_Type_DEVICE_ID, true,
                (char *)ids, strlen(ids));
    return true;
    
#else
    uart::UartDevice* uart = &getConfiguration()->uart;
    size_t deviceIdLength = strnlen(uart->deviceId, sizeof(uart->deviceId));
    bool status = false;
    if(deviceIdLength > 0) {
        status = true;
        sendCommandResponse(openxc_ControlCommand_Type_DEVICE_ID, status,
                uart->deviceId, deviceIdLength);
    }
    return status;
#endif
}
