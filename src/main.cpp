#include <Arduino.h>
#include <NimBLEDevice.h>
#include "lcd.h"
#include "buttons.h"
#include "ble_commands.h"
// #include "motors.h"  // no motors on this board

#define SPEED_STEP       25
#define SPEED_MIN        50
#define SPEED_MAX        255
#define DEAL_DURATION_MS 600

// ── State ─────────────────────────────────────────────────
int  currentSpeed   = 150;
bool leftMotorOn    = false;
bool rightMotorOn   = false;
bool isDealing      = false;
unsigned long dealStart = 0;

// ── BLE ───────────────────────────────────────────────────
static NimBLERemoteCharacteristic* pChr      = nullptr;
static const NimBLEAdvertisedDevice* advDevice = nullptr;
static bool doConnect  = false;
static bool isConnected = false;
static uint32_t scanTimeMs = 5000;

// ── Send a 2-byte command [cmd, speed] ────────────────────
void sendCommand(uint8_t cmd) {
    if (!isConnected || pChr == nullptr) {
        Serial.println("BLE not connected — command dropped");
        return;
    }
    uint8_t packet[2] = { cmd, (uint8_t)currentSpeed };
    pChr->writeValue(packet, 2, false);
    Serial.printf("BLE TX: cmd=0x%02X speed=%d\n", cmd, currentSpeed);
}

void refreshDisplay() {
    updateDealer(currentSpeed, leftMotorOn, rightMotorOn, isDealing);
}

//============================================//
// BLE Scan Callbacks                         //
//============================================//
class ScanCallbacks : public NimBLEScanCallbacks {
    void onResult(const NimBLEAdvertisedDevice* d) override {
        if (d->isAdvertisingService(NimBLEUUID(SERVICE_UUID))) {
            Serial.printf("Found motor server: %s\n", d->toString().c_str());
            NimBLEDevice::getScan()->stop();
            advDevice = d;
            doConnect = true;
        }
    }
    void onScanEnd(const NimBLEScanResults& results, int reason) override {
        Serial.printf("Scan ended (reason %d) — restarting\n", reason);
        NimBLEDevice::getScan()->start(scanTimeMs, false, true);
    }
} scanCallbacks;

//============================================//
// BLE Client Callbacks                       //
//============================================//
class ClientCallbacks : public NimBLEClientCallbacks {
    void onConnect(NimBLEClient* pClient) override {
        isConnected = true;
        Serial.println("BLE Connected to motor server");
        displayStatus("BLE", "Connected", ST77XX_GREEN);
        delay(800);
        refreshDisplay();
    }
    void onDisconnect(NimBLEClient* pClient, int reason) override {
        isConnected = false;
        pChr = nullptr;
        Serial.printf("BLE Disconnected (reason %d) — rescanning\n", reason);
        displayStatus("BLE", "Lost...", ST77XX_RED);
        delay(800);
        refreshDisplay();
        NimBLEDevice::getScan()->start(scanTimeMs, false, true);
    }
} clientCallbacks;

//============================================//
// Connect to Motor Server                    //
//============================================//
bool connectToServer() {
    NimBLEClient* pClient = NimBLEDevice::getClientByPeerAddress(advDevice->getAddress());
    if (pClient) {
        NimBLEDevice::deleteClient(pClient);
    }

    pClient = NimBLEDevice::createClient();
    pClient->setClientCallbacks(&clientCallbacks, false);
    pClient->setConnectionParams(12, 12, 0, 150);
    pClient->setConnectTimeout(5 * 1000);

    if (!pClient->connect(advDevice)) {
        NimBLEDevice::deleteClient(pClient);
        Serial.println("Connection failed");
        return false;
    }

    NimBLERemoteService* pSvc = pClient->getService(SERVICE_UUID);
    if (pSvc) pChr = pSvc->getCharacteristic(CHAR_UUID);

    if (!pChr) {
        Serial.println("Characteristic not found");
        return false;
    }

    Serial.println("Ready to send commands");
    return true;
}

//============================================//
// Setup                                      //
//============================================//
void setup() {
    Serial.begin(115200);
    lcdSetup();
    buttonsSetup();

    displayStatus("GT DEALER", "Starting BLE...", ST77XX_CYAN);

    NimBLEDevice::init("CardDealer-Controller");
    NimBLEDevice::setPower(3);

    NimBLEScan* pScan = NimBLEDevice::getScan();
    pScan->setScanCallbacks(&scanCallbacks, false);
    pScan->setInterval(100);
    pScan->setWindow(100);
    pScan->setActiveScan(true);
    pScan->start(scanTimeMs);

    Serial.println("Scanning for motor server...");
}

//============================================//
// Loop                                       //
//============================================//
void loop() {

    // ── BLE connect handshake ──────────────────────────────
    if (doConnect) {
        doConnect = false;
        if (!connectToServer()) {
            NimBLEDevice::getScan()->start(scanTimeMs, false, true);
        }
    }

    // ── LEFT toggle ────────────────────────────────────────
    if (leftTriggered) {
        leftTriggered = false;
        leftMotorOn = !leftMotorOn;
        sendCommand(CMD_LEFT);
        Serial.printf("LEFT: %s\n", leftMotorOn ? "ON" : "OFF");
        refreshDisplay();
    }

    // ── RIGHT toggle ───────────────────────────────────────
    if (rightTriggered) {
        rightTriggered = false;
        rightMotorOn = !rightMotorOn;
        sendCommand(CMD_RIGHT);
        Serial.printf("RIGHT: %s\n", rightMotorOn ? "ON" : "OFF");
        refreshDisplay();
    }

    // ── SPEED UP ───────────────────────────────────────────
    if (speedUpTriggered) {
        speedUpTriggered = false;
        currentSpeed = min(currentSpeed + SPEED_STEP, SPEED_MAX);
        sendCommand(CMD_SPEED_UP);
        Serial.printf("SPEED UP: %d\n", currentSpeed);
        refreshDisplay();
    }

    // ── SPEED DOWN ─────────────────────────────────────────
    if (speedDownTriggered) {
        speedDownTriggered = false;
        currentSpeed = max(currentSpeed - SPEED_STEP, SPEED_MIN);
        sendCommand(CMD_SPEED_DOWN);
        Serial.printf("SPEED DOWN: %d\n", currentSpeed);
        refreshDisplay();
    }

    // ── DEAL ───────────────────────────────────────────────
    if (dealTriggered && !isDealing) {
        dealTriggered = false;
        isDealing  = true;
        dealStart  = millis();
        sendCommand(CMD_DEAL);
        Serial.println("DEAL: Dealing...");
        refreshDisplay();
    }

    if (isDealing && (millis() - dealStart >= DEAL_DURATION_MS)) {
        isDealing = false;
        Serial.println("DEAL: Done.");
        refreshDisplay();
    }
}