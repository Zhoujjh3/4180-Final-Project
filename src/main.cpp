#include <Arduino.h>
#include <NimBLEDevice.h>
#include "lcd.h"
#include "buttons.h"
#include "ble_commands.h"
#include "menu.h"

#define SHUFFLE_DURATION_MS 24000  // 20 seconds

unsigned long shuffleStart = 0;

static NimBLERemoteCharacteristic* pChr = nullptr;
static const NimBLEAdvertisedDevice* advDevice = nullptr;
static bool doConnect   = false;
static bool isConnected = false;
static uint32_t scanTimeMs  = 5000;

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
        Serial.printf("BLE Disconnected (reason %d) — rescanning\n", reason);
        displayStatus("BLE", "Lost...", ST77XX_RED);
        delay(800);
        NimBLEDevice::getScan()->start(scanTimeMs, false, true);
        currScreen = SCREEN_MENU;                                    
        drawScreen(SCREEN_MENU);   
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

    // load player count + deal amount from nonvolatile memory
    loadFromNV();
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

    if (upTriggered || downTriggered || selectTriggered) {
      screenStates prevScreen = currScreen;
      transitionScreen(upTriggered, downTriggered, selectTriggered);

      upTriggered = false;
      downTriggered = false;
      selectTriggered = false;

      //BLE commands
      if (currScreen != prevScreen) {
        if (currScreen == SCREEN_SHUFFLING) {
            shuffleStart = millis();                     
            sendCommand(CMD_SHUFFLE);
        } else if (prevScreen == SCREEN_SHUFFLING) {  
            sendCommand(CMD_SHUFFLE_STOP);             
        }
                                                          
        if (currScreen == SCREEN_DEALING) {              
            sendCommand(CMD_DEAL);
        } else if (prevScreen == SCREEN_DEALING) {  
            sendCommand(CMD_DEAL_STOP);
        }                                                
      }   
    }

    // ── Auto-stop shuffle after 20s ───────────────────────
        if (currScreen == SCREEN_SHUFFLING && (millis() - shuffleStart >= SHUFFLE_DURATION_MS)) {         
          sendCommand(CMD_SHUFFLE_STOP);                       
          currScreen = SCREEN_MENU;
          drawScreen(SCREEN_MENU);                           
      }
}