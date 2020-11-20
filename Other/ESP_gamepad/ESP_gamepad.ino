#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "BLE2902.h"
#include "BLEHIDDevice.h"
#include "HIDTypes.h"
#include "HIDKeyboardTypes.h"
#include <driver/adc.h>

BLEHIDDevice* hid;
BLECharacteristic* input;
BLECharacteristic* output;

uint8_t buttons = 0;
uint8_t button1 = 0;
uint8_t button2 = 0;
uint8_t button3 = 0;
bool connected = false;

class MyCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer){
    connected = true;
    BLE2902* desc = (BLE2902*)input->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
    desc->setNotifications(true);
  }

  void onDisconnect(BLEServer* pServer){
    connected = false;
    BLE2902* desc = (BLE2902*)input->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
    desc->setNotifications(false);
  }
};

/*
 * This callback is connect with output report. In keyboard output report report special keys changes, like CAPSLOCK, NUMLOCK
 * We can add digital pins with LED to show status
 * bit 0 - NUM LOCK
 * bit 1 - CAPS LOCK
 * bit 2 - SCROLL LOCK
 */
 class MyOutputCallbacks : public BLECharacteristicCallbacks {
 void onWrite(BLECharacteristic* me){
    uint8_t* value = (uint8_t*)(me->getValue().c_str());
    ESP_LOGI(LOG_TAG, "special keys: %d", *value);
  }
};

void taskServer(void*){


    BLEDevice::init("Esp32 Joy");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyCallbacks());

    hid = new BLEHIDDevice(pServer);
    input = hid->inputReport(1); // <-- input REPORTID from report map
    output = hid->outputReport(1); // <-- output REPORTID from report map

    output->setCallbacks(new MyOutputCallbacks());

    std::string name = "Test Co.";
    hid->manufacturer()->setValue(name);

    hid->pnp(0x02, 0xe502, 0xa111, 0x0210);
    hid->hidInfo(0x00,0x02);

  BLESecurity *pSecurity = new BLESecurity();
//  pSecurity->setKeySize();
  pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);

    const uint8_t report[] = {
      0x05, 0x01, /* Usage Page (Generic Desktop) */
 0x09, 0x04, /* Usage (Gamepad) */
 
 0xa1, 0x01, /* Collection (Application) */
 0x09, 0x01, /* Usage (Pointer) */
 
 /* 8 axes, signed 16 bit resolution, range -32768 to 32767 (16 bytes) */
 0xa1, 0x00, /* Collection (Physical) */
 0x05, 0x01, /* Usage Page (Generic Desktop) */
 0x09, 0x30, /* Usage (X) */
 0x09, 0x31, /* Usage (Y) */
 0x09, 0x32, /* Usage (Analog1) */
 0x09, 0x33, /* Usage (Analog2) */
 0x09, 0x34, /* Usage (Analog3) */
 0x09, 0x35, /* Usage (Analog4) */
 0x09, 0x36, /* Usage (Analog5) */
 0x09, 0x37, /* Usage (Analog6) */
 0x16, 0x00, 0x80, /* Logical Minimum (-32768) */
 0x26, 0xff, 0x7f, /* Logical Maximum (32767) */
 0x75, 16, /* Report Size (16) */
 0x95, 8, /* Report Count (8) */
 0x81, 0x82, /* Input (Data, Variable, Absolute, Volatile) */
 0xc0, /* End Collection */
 
 /* 40 buttons, value 0=off, 1=on (5 bytes) */
 0x05, 0x09, /* Usage Page (Button) */
 0x19, 1, /* Usage Minimum (Button 1) */
 0x29, 40, /* Usage Maximum (Button 40) */
 0x15, 0x00, /* Logical Minimum (0) */
 0x25, 0x01, /* Logical Maximum (1) */
 0x75, 1, /* Report Size (1) */
 0x95, 40, /* Report Count (40) */
 0x81, 0x02, /* Input (Data, Variable, Absolute) */
 0xc0 /* End Collection */
    };

    hid->reportMap((uint8_t*)report, sizeof(report));
    hid->startServices();

    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->setAppearance(HID_GAMEPAD);
    pAdvertising->addServiceUUID(hid->hidService()->getUUID());
    pAdvertising->start();
    hid->setBatteryLevel(34);

    ESP_LOGD(LOG_TAG, "Advertising started!");
    delay(portMAX_DELAY);
  
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

 

  xTaskCreate(taskServer, "server", 20000, NULL, 5, NULL);
}

void loop() {
 
 
  delay(1000);
}