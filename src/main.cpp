#include <Arduino.h>
#include <NimBLEDevice.h>
#include "lcd.h"
#include "buttons.h"
#include "ble_commands.h"
#include "menu.h"

#define SHUFFLE_DURATION_MS  24000
#define DEAL_DISPENSE_MS     800    // how long to run motor per card
#define DEAL_PAUSE_MS        5000   // pause between cards

// ── Shuffle state ─────────────────────────────────────────────
unsigned long shuffleStart = 0;

// dealer state machine
enum DealState { 
  DEAL_IDLE, 
  DEAL_DISPENSING, 
  DEAL_PAUSING 
};
static DealState     dealState  = DEAL_IDLE;
static int           cardsDealt = 0;
static int           totalCards = 0;
static unsigned long dealTimer  = 0;

// BLE stuff
static NimBLERemoteCharacteristic* pChr       = nullptr;
static const NimBLEAdvertisedDevice* advDevice = nullptr;
static bool     doConnect   = false;
static bool     isConnected = false;
static uint32_t scanTimeMs  = 5000;

void sendCommand(uint8_t cmd);

//deal sequence helper functions
void startDealSequence() {
    totalCards = (int)numPlayers * (int)numCards;
    cardsDealt = 0;
    dealState  = DEAL_DISPENSING;
    dealTimer  = millis();
    sendCommand(CMD_DEAL);
    Serial.printf("Deal sequence: %d total cards\n", totalCards);
}

void stopDealSequence() {
    sendCommand(CMD_DEAL_STOP);
    dealState  = DEAL_IDLE;
    cardsDealt = 0;
    totalCards = 0;
}

// Called every loop
void runDealSequence() {
    if (dealState == DEAL_IDLE) return;

    if (dealState == DEAL_DISPENSING) {
        if (millis() - dealTimer >= DEAL_DISPENSE_MS) {
            sendCommand(CMD_DEAL_STOP);
            cardsDealt++;
            Serial.printf("Card %d / %d dispensed\n", cardsDealt, totalCards);

            if (cardsDealt >= totalCards) {
                // All cards dealt — return to menu
                dealState  = DEAL_IDLE;
                currScreen = SCREEN_MENU;
                drawScreen(SCREEN_MENU);
            } else {
                dealState = DEAL_PAUSING;
                dealTimer = millis();
            }
        }
    } else if (dealState == DEAL_PAUSING) {
        if (millis() - dealTimer >= DEAL_PAUSE_MS) {
            sendCommand(CMD_DEAL);
            dealState = DEAL_DISPENSING;
            dealTimer = millis();
        }
    }
}

//============================================//
// BLE: Send Command                          //
//============================================//
void sendCommand(uint8_t cmd) {
    if (!isConnected || pChr == nullptr) {
        Serial.println("BLE not connected — command dropped");
        return;
    }
    uint8_t packet[1] = { cmd };
    pChr->writeValue(packet, 1, false);
    Serial.printf("BLE TX: cmd=0x%02X\n", cmd);
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
        currScreen = SCREEN_MENU;
        drawScreen(SCREEN_MENU);
    }
    void onDisconnect(NimBLEClient* pClient, int reason) override {
        isConnected = false;
        pChr = nullptr;
        stopDealSequence();
        shuffleStart = 0;
        Serial.printf("BLE Disconnected (reason %d) — rescanning\n", reason);
        displayStatus("BLE", "Lost...", ST77XX_RED);
        delay(800);
        currScreen = SCREEN_MENU;
        drawScreen(SCREEN_MENU);
        NimBLEDevice::getScan()->start(scanTimeMs, false, true);
    }
} clientCallbacks;

//============================================//
// Connect to Motor Server                    //
//============================================//
bool connectToServer() {
    NimBLEClient* pClient = NimBLEDevice::getClientByPeerAddress(advDevice->getAddress());
    if (pClient) NimBLEDevice::deleteClient(pClient);

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
    loadFromNV();

    displayStatus("GT DEALER", "Connecting...", ST77XX_CYAN);

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


void loop() {

    // BLE handshake
    if (doConnect) {
        doConnect = false;
        if (!connectToServer())
            NimBLEDevice::getScan()->start(scanTimeMs, false, true);
    }

    // menu input
    if (upTriggered || downTriggered || selectTriggered) {
        screenStates prevScreen = currScreen;

        transitionScreen(upTriggered, downTriggered, selectTriggered);

        upTriggered     = false;
        downTriggered   = false;
        selectTriggered = false;

        if (currScreen != prevScreen) {
            // Entered shuffling
            if (currScreen == SCREEN_SHUFFLING) {
                shuffleStart = millis();
                sendCommand(CMD_SHUFFLE);

            // Left shuffling manually (select press) — no auto-deal
            } else if (prevScreen == SCREEN_SHUFFLING) {
                sendCommand(CMD_SHUFFLE_STOP);
            }
            
            // Entered manual shuffling — motors on, no timer
            if (currScreen == SCREEN_MANUAL_SHUFFLING) {        // ← new
                sendCommand(CMD_SHUFFLE);

            // Left manual shuffling (select press) — just stop motors
            } else if (prevScreen == SCREEN_MANUAL_SHUFFLING) { // ← new
                sendCommand(CMD_SHUFFLE_STOP);
            }

            // Entered dealing — start sequence
            if (currScreen == SCREEN_DEALING) {
                startDealSequence();

            // Left dealing manually — stop sequence
            } else if (prevScreen == SCREEN_DEALING) {
                stopDealSequence();
            }
        }
    }

    // Auto-stop shuffle and auto-start deal 
    if (currScreen == SCREEN_SHUFFLING && (millis() - shuffleStart >= SHUFFLE_DURATION_MS)) {
        sendCommand(CMD_SHUFFLE_STOP);
        Serial.println("Shuffle done — starting deal sequence");
        currScreen = SCREEN_DEALING;
        drawScreen(SCREEN_DEALING);
        startDealSequence();
    }

    // run the deal state machine
    runDealSequence();
}
