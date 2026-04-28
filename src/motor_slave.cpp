#include <Arduino.h>
#include <NimBLEDevice.h>
#include "motors.h"
#include "ble_commands.h"
#include "esp_pm.h"

#define RAMP_DURATION_MS  300   // time to ramp from 0 to full speed
#define DEAL_FULL_SPEED   200

// ── Deal motor ramp state ─────────────────────────────────────
static bool          isRamping = false;
static unsigned long rampStart = 0;

//============================================//
// BLE Server Callbacks                       //
//============================================//
class ServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
        Serial.println("Controller connected");
    }
    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
        Serial.printf("Controller disconnected (reason %d) — re-advertising\n", reason);
        motor1Speed(0);
        motor2Speed(0);
        outputMotorSpeed(0);
        isRamping = false;
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
                motor1Speed(255);
                motor2Speed(255);
                Serial.println("Shuffle: start");
                break;

            case CMD_SHUFFLE_STOP:
                motor1Speed(0);
                motor2Speed(0);
                Serial.println("Shuffle: done");
                break;

            case CMD_DEAL:
                // Start ramp — loop() drives the actual speed
                isRamping = true;
                rampStart = millis();
                Serial.println("Deal: ramping up");
                break;

            case CMD_DEAL_STOP:
                outputMotorSpeed(0);
                isRamping = false;
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

    esp_pm_config_t pm_config = {
        .max_freq_mhz = 160,
        .min_freq_mhz = 10,
        .light_sleep_enable = true
    };
    esp_pm_configure(&pm_config);

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
    if (isRamping) {
        unsigned long elapsed = millis() - rampStart;
        if (elapsed >= RAMP_DURATION_MS) {
            outputMotorSpeed(DEAL_FULL_SPEED);
            isRamping = false;
        } else {
            int speed = (int)(DEAL_FULL_SPEED * elapsed / RAMP_DURATION_MS);
            outputMotorSpeed(speed);
        }
    }
    delay(10);
}
