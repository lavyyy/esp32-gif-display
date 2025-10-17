#pragma once

// Pins
#define LCD_SCLK   39
#define LCD_MOSI   38
#define LCD_MISO   40
#define LCD_DC     42
#define LCD_RST    0
#define LCD_CS     45
#define LCD_BL     1

// LCD
#define LCD_ROTATION_PORTRAIT 0
#define LCD_ROTATION_LANDSCAPE 90
#define LCD_W                 240
#define LCD_H                 320

// Wi-Fi AP
#define AP_SSID   "ESP32-GIF"
#define AP_PASS   "upload123"

// Files
#define GIF_ACTIVE_PATH  "/gifs/current.gif"
#define GIF_TMP_PATH     "/gifs/.upload.tmp"
#define ORIENTATION_FILE "/gifs/orientation.txt"
