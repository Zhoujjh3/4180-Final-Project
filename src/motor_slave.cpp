#include <Arduino.h>
#include <NimBLEDevice.h>
#include "motors.h"
#include "ble_commands.h"

//============================================//
// BLE Server Callbacks                       //
//============================================//
class ServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
        Serial.println("Controller connected");
    }
    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
        Serial.printf("Controller disconnected (reason %d) — re-advertising\n", reason);
        // Safety stop all motors on disconnect
        motor1Speed(0);
        motor2Speed(0);
        outputMotorSpeed(0);
        NimBLEDevice::getAdvertising()->start();
    }
};

//============================================//
// Characteristic Write Callback              //
//============================================//
class CharCallbacks : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* pChr, NimBLEConnInfo& connInfo) override {
        std::string val = pChr->getValue();
        if (val.size() < 1) {
            Serial.println("Bad packet");
            return;
        }

        uint8_t cmd = (uint8_t)val[0];

        switch (cmd) {
            case CMD_SHUFFLE:
                motor1Speed(255);       // shuffle motor left
                motor2Speed(255);       // shuffle motor right
                Serial.println("Shuffle: start");
                break;

            case CMD_SHUFFLE_STOP:
                motor1Speed(0);
                motor2Speed(0);
                Serial.println("Shuffle: done");
                break;

            case CMD_DEAL:
                outputMotorSpeed(255);  // deal motor
                Serial.println("Deal: On");
                break;

            case CMD_DEAL_STOP:
                outputMotorSpeed(0);
                Serial.println("Deal: Off");
                break;

            default:
                Serial.printf("Unknown cmd: 0x%02X\n", cmd);
                break;
        }
    }
};

//============================================//
// Setup                                      //
//============================================//
void setup() {
    Serial.begin(115200);
    delay(3000);

    motorSetup();

    NimBLEDevice::init(DEVICE_NAME);
    NimBLEDevice::setPower(3);

    NimBLEServer* pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    NimBLEService* pService = pServer->createService(SERVICE_UUID);

    NimBLECharacteristic* pChr = pService->createCharacteristic(
        CHAR_UUID,
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR
    );
    pChr->setCallbacks(new CharCallbacks());

    NimBLEAdvertising* pAdv = NimBLEDevice::getAdvertising();
    pAdv->addServiceUUID(SERVICE_UUID);
    pAdv->start();

    Serial.println("Advertising — waiting for controller...");
}

//============================================//
// Loop                                       //
//============================================//
void loop() {
    delay(10);
}