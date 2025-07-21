# PongWars - E-Paper Display Game

A Pong-style game running on the LilyGo EPD47 e-paper display with ESP32.

## Features

- Pong-style gameplay on e-paper display
- Web interface for remote control
- Automatic WiFi connection to multiple networks
- Configurable grid sizes

## WiFi Configuration

This project requires WiFi credentials to be configured. **For security reasons, the WiFi configuration file is not included in this repository.**

### Setup WiFi Configuration

1. Create a file named `wifi_config.h` in the project root directory
2. Use the following template:

```cpp
#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

// WiFi networks to try (in order of preference)
const char* wifi_networks[][2] = {
    {"YOUR_WIFI_SSID_1", "YOUR_WIFI_PASSWORD_1"},
    {"YOUR_WIFI_SSID_2", "YOUR_WIFI_PASSWORD_2"},
    // Add more networks as needed
};
const int num_networks = sizeof(wifi_networks) / sizeof(wifi_networks[0]);

#endif
```

### Example Configuration

```cpp
const char* wifi_networks[][2] = {
    {"HomeWiFi", "mypassword123"},
    {"OfficeWiFi", "officepass456"},
    {"MobileHotspot", "mobile789"},
};
```

The device will automatically try to connect to each network in order until one succeeds.

## Hardware Requirements

- LilyGo EPD47 e-paper display
- ESP32 with PSRAM enabled

## Installation

1. Clone this repository
2. Create your `wifi_config.h` file (see WiFi Configuration above)
3. Open in PlatformIO or Arduino IDE
4. Upload to your device

## Usage

Once connected to WiFi, the device will:
- Display the IP address on the serial monitor
- Start a web server accessible at `http://[device-ip]`
- Run the PongWars game on the e-paper display

### Web Interface

- `GET /reset` - Reset the game
- `POST /change_size?block_dim=X` - Change grid size (X must be a factor of 60: 1, 2, 3, 4, 5, 6, 10, 12, 15, 20, 30, 60)

## License

[Add your license here]
