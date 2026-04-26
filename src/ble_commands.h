#pragma once

#define SERVICE_UUID   "4180"
#define CHAR_UUID      "0002"
#define DEVICE_NAME    "CardDealer-Motors"

// Command bytes — first byte of every BLE write
#define CMD_SHUFFLE       0x01   // start both motors
#define CMD_SHUFFLE_STOP  0x02   // stop both motors
#define CMD_DEAL          0x03   // start deal motor
#define CMD_DEAL_STOP     0x04   // stop deal motor