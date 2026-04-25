#pragma once

#define SERVICE_UUID   "4180"
#define CHAR_UUID      "0002"
#define DEVICE_NAME    "CardDealer-Motors"

// Command bytes — first byte of every BLE write
#define CMD_LEFT       0x01
#define CMD_RIGHT      0x02
#define CMD_SPEED_UP   0x03
#define CMD_SPEED_DOWN 0x04
#define CMD_DEAL       0x05
// Packet: [CMD, SPEED_VALUE]  — always 2 bytes