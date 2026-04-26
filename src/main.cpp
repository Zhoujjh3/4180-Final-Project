#include <Arduino.h>
#include <NimBLEDevice.h>
#include "lcd.h"
#include "buttons.h"
#include "ble_commands.h"

#define SHUFFLE_DURATION_MS 24000  // 20 seconds

// ── State ─────────────────────────────────────────────────
bool isShuffling = false;
bool isDealing   = false;
unsigned long shuffleStart = 0;

// ── BLE ───────────────────────────────────────────────────
static NimBLERemoteCharacteristic* pChr       = nullptr;
static const NimBLEAdvertisedDevice* advDevice = nullptr;
static bool     doConnect   = false;
static bool     isConnected = false;
static uint32_t scanTimeMs  = 5000;

// ── Send a 1-byte command ─────────────────────────────────
void sendCommand(uint8_t cmd) {
    if (!isConnected || pChr == nullptr) {
        Serial.println("BLE not connected — command dropped");
        return;
    }
    uint8_t packet[1] = { cmd };
    pChr->writeValue(packet, 1, false);
    Serial.printf("BLE TX: cmd=0x%02X\n", cmd);
}

void refreshDisplay() {
    updateDealer(isDealing, isShuffling);
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
        resetDealerDisplay();
        delay(800);
        refreshDisplay();
    }
    void onDisconnect(NimBLEClient* pClient, int reason) override {
        isConnected = false;
        pChr = nullptr;
        Serial.printf("BLE Disconnected (reason %d) — rescanning\n", reason);
        displayStatus("BLE", "Lost...", ST77XX_RED);
        resetDealerDisplay();
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
    resetDealerDisplay();

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

    // ── SHUFFLE: both motors on for 20s ───────────────────
    if (shuffleTriggered && !isShuffling && !isDealing) {
        shuffleTriggered = false;
        isShuffling  = true;
        shuffleStart = millis();
        sendCommand(CMD_SHUFFLE);
        Serial.println("Shuffle: start");
        displayStatus("SHUFFLE", "Shuffling...", ST77XX_YELLOW);
        resetDealerDisplay();
    }

    // Auto-stop shuffle after 20 seconds
    if (isShuffling && (millis() - shuffleStart >= SHUFFLE_DURATION_MS)) {
        isShuffling = false;
        sendCommand(CMD_SHUFFLE_STOP);
        Serial.println("Shuffle: done");
        displayStatus("SHUFFLE", "Done!", ST77XX_GREEN);
        resetDealerDisplay();
        delay(1500);
        refreshDisplay();
    }

    // ── DEAL: toggle deal motor on/off ────────────────────
    if (dealTriggered && !isShuffling) {
        dealTriggered = false;

        if (!isDealing) {
            isDealing = true;
            sendCommand(CMD_DEAL);
            Serial.println("Deal: On");
            displayStatus("DEAL", "Dealing...", ST77XX_GREEN);
            resetDealerDisplay();
        } else {
            isDealing = false;
            sendCommand(CMD_DEAL_STOP);
            Serial.println("Deal: Off");
            displayStatus("DEAL", "Stopped", ST77XX_RED);
            resetDealerDisplay();
            delay(1000);
            refreshDisplay();
        }
    }
}