#include <Arduino.h>
#include <NimBLEDevice.h>
#include "motors.h"
#include "ble_commands.h"

#define DEAL_DURATION_MS 600

// ── Motor state ───────────────────────────────────────────
int  currentSpeed   = 150;
bool leftMotorOn    = false;
bool rightMotorOn   = false;
bool isDealing      = false;
unsigned long dealStart = 0;

void applyMotors() {
    motor1Speed(leftMotorOn  ? currentSpeed : 0);
    motor2Speed(rightMotorOn ? currentSpeed : 0);
}

//============================================//
// BLE Server Callbacks                       //
//============================================//
class ServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
        Serial.println("Controller connected");
    }
    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
        Serial.printf("Controller disconnected (reason %d) — re-advertising\n", reason);
        // Stop motors on disconnect for safety
        motor1Speed(0);
        motor2Speed(0);
        leftMotorOn  = false;
        rightMotorOn = false;
        NimBLEDevice::getAdvertising()->start();
    }
};

//============================================//
// Characteristic Write Callback              //
// Fires every time controller sends a cmd   //
//============================================//
class CharCallbacks : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* pChr, NimBLEConnInfo& connInfo) override {
        std::string val = pChr->getValue();
        if (val.size() < 2) {
            Serial.println("Bad packet length");
            return;
        }

        uint8_t cmd   = (uint8_t)val[0];
        uint8_t speed = (uint8_t)val[1];

        // Always sync speed from controller
        currentSpeed = speed;

        switch (cmd) {
            case CMD_LEFT:
                leftMotorOn = !leftMotorOn;
                applyMotors();
                Serial.printf("LEFT: %s  speed=%d\n", leftMotorOn ? "ON" : "OFF", currentSpeed);
                break;

            case CMD_RIGHT:
                rightMotorOn = !rightMotorOn;
                applyMotors();
                Serial.printf("RIGHT: %s  speed=%d\n", rightMotorOn ? "ON" : "OFF", currentSpeed);
                break;

            case CMD_SPEED_UP:
            case CMD_SPEED_DOWN:
                // Speed already synced above — just reapply to running motors
                applyMotors();
                Serial.printf("SPEED: %d\n", currentSpeed);
                break;

            case CMD_DEAL:
                isDealing  = true;
                dealStart  = millis();
                motor1Speed(currentSpeed);
                motor2Speed(currentSpeed);
                Serial.printf("DEAL: firing at speed=%d\n", currentSpeed);
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
    motorSetup();

    Serial.println("Motor server starting...");

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

    pService->start();

    NimBLEAdvertising* pAdv = NimBLEDevice::getAdvertising();
    pAdv->addServiceUUID(SERVICE_UUID);
    pAdv->start();

    Serial.println("Advertising — waiting for controller...");
}

//============================================//
// Loop                                       //
//============================================//
void loop() {
    // Auto-stop deal pulse
    if (isDealing && (millis() - dealStart >= DEAL_DURATION_MS)) {
        isDealing = false;
        applyMotors();  // restore to toggle state
        Serial.println("DEAL: done");
    }
    delay(10);
}