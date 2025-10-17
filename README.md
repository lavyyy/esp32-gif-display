# ESP32 GIF Display

A ESP32-based GIF display that creates a WiFi access point for uploading and displaying GIF animations on LCD screens. Originally designed for the ESP32-S3-LCD-2 Waveshare board but configurable for other ESP32 boards with SPI LCD displays.

## Hardware Requirements

- **ESP32 board** (Configured for the ESP32-S3-LCD-2 by default)
- **SPI LCD display** (240x320 resolution by default)

## Board Compatibility

This project should work with any ESP32 board, but you'll need to:

1. **Configure pin assignments** in `config.h` to match your board's LCD connections
2. **Enable PSRAM** (Required due to the use of `GIFPlayer.h` from [AnimatedGIF](https://github.com/bitbank2/AnimatedGIF))

### Recommended Boards

- **ESP32-S3-LCD-2 Waveshare** (tested, pin configuration included)
- **ESP32-S3** with SPI LCD breakout (PSRAM required)
- **ESP32** with SPI LCD breakout (PSRAM required)

**Important**: Only ESP32 boards with PSRAM support will work with this project.

## Important Configuration Notes

### PSRAM Configuration

**REQUIRED**: PSRAM is mandatory for this project. The `GIFPlayer.h` library from AnimatedGIF requires PSRAM memory allocation (3 × width × height bytes) and will not work without it.

In Arduino IDE:

1. Select your ESP32 board variant
2. Set **PSRAM** to **"OPI PSRAM"** (this is what works with ESP32-S3-LCD-2)
3. Leave other settings as default unless you encounter issues

**Note**: This project will not function on boards without PSRAM support due to the memory requirements of the GIF player.

## Required Libraries

Install these libraries through Arduino IDE Library Manager:

1. **bb_spi_lcd** - For LCD display control
2. **AnimatedGIF** - For GIF decoding and playback

### Library Installation Quirk

There's a known issue with the AnimatedGIF library: the `GIFPlayer.h` header file is not included in the standard library installation. You'll need to manually add it:

1. Download `GIFPlayer.h` from the [AnimatedGIF GitHub repository](https://github.com/bitbank2/AnimatedGIF/tree/master/src)
2. Place it in your Arduino libraries folder alongside `AnimatedGIF.h` and `AnimatedGIF.cpp`
3. The file should be located at: `Arduino/libraries/AnimatedGIF/src/GIFPlayer.h`

## Features

- **Web-based GIF Upload**: Upload GIFs through a simple web interface
- **Orientation Control**: Switch between portrait (240x320) and landscape (320x240) display modes
- **Real-time Status**: Live upload status and progress indicators
- **Persistent Settings**: Display orientation settings are saved and restored on reboot
- **Hot-swappable GIFs**: Upload new GIFs without interrupting playback

## Setup Instructions

1. **Install Required Libraries** (see Library Installation Quirk above)
2. **Configure Arduino IDE**:
   - Select your ESP32 board variant
   - Set PSRAM to "OPI PSRAM"
3. **Configure Pin Assignments**: Update `config.h` to match your board's LCD connections
4. **Upload the Code** to your ESP32 board
5. **Connect to WiFi**: The ESP32 will create an access point named "ESP32-GIF" (password: "upload123")
6. **Open Web Interface**: Navigate to `http://192.168.4.1` in your browser

## Usage

1. **Set Display Orientation**: Choose between Portrait or Landscape mode
2. **Upload GIF**: Select a GIF file and click "Upload & Switch"
3. **Enjoy**: Your GIF will start playing immediately on the LCD

## Technical Details

- **Display Resolution**: 240x320 pixels
- **Supported Format**: GIF animations
- **Storage**: Uses LittleFS filesystem
- **WiFi**: Creates access point for easy configuration
- **Memory**: Requires PSRAM for GIF decoding (3 × width × height bytes)

## File Structure

```
esp32-gif-display/
├── esp32-gif-display.ino    # Main Arduino sketch
├── config.h                 # Pin definitions and configuration
├── data/
│   └── index.html          # Web interface
└── src/
    ├── display/            # LCD display control
    ├── gif_player/         # GIF playback logic
    └── webui/              # Web server and upload handling
```

## Troubleshooting

- **GIF not displaying**: Ensure PSRAM is enabled in board settings (required)
- **Compilation errors**: Verify your ESP32 board supports PSRAM
- **Upload fails**: Check that GIF file is valid and not too large
- **Display issues**: Verify pin connections match `config.h` definitions
- **Library errors**: Ensure `GIFPlayer.h` is manually added to AnimatedGIF library

## Pin Configuration

The project uses configurable pin assignments defined in `config.h`. Default configuration for ESP32-S3-LCD-2 Waveshare:

- LCD_SCLK: GPIO 39
- LCD_MOSI: GPIO 38
- LCD_MISO: GPIO 40
- LCD_DC: GPIO 42
- LCD_RST: GPIO 0
- LCD_CS: GPIO 45
- LCD_BL: GPIO 1 (backlight control)

**For other boards**: Modify the pin definitions in `config.h` to match your specific LCD connections.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
