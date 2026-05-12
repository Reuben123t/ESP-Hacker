# ESP-Hacker ⚡

A comprehensive, low-level firmware suite designed for the ESP32 platform. This project enables security auditing, Wi-Fi network analysis, and hardware-level penetration testing. It leverages the native promiscuous mode of the ESP32 to monitor, capture, and interact with 802.11 wireless frames in real-time.

---

## 🚀 Core Features

*   **Promiscuous Mode Sniffing:** Captures raw IEEE 802.11 frames (Management, Control, and Data).
*   **Packet Injection:** Forges and transmits custom Wi-Fi frames for targeted network auditing.
*   **Deauthentication Testing:** Evaluates network resilience against unauthorized disconnection frames.
*   **Beacon Spamming:** Generates multiple fake Access Points (SSIDs) simultaneously to test client behavior.
*   **Probe Request Monitoring:** Tracks active device discovery signals from nearby smartphones and laptops.
*   **Channel Hopping:** Automatically cycles through the 2.4GHz spectrum (Channels 1–14) to maximize capture density.

---

## 🛠️ Hardware Requirements

*   **Development Board:** NodeMCU-32S, ESP32-WROOM-32, or any standard ESP32 development board.
*   **USB Cable:** Micro-USB or USB-C cable supporting data transfer (not power-only cables).
*   **Host System:** PC running Windows, macOS, or Linux.

---

## 💻 Installation & Setup

### Prerequisites
1. Download and install the [Arduino IDE](https://arduino.cc) (Version 2.0 or higher recommended).
2. Install the ESP32 Arduino Core:
   * Go to **File** > **Preferences**.
   * Add this URL to *Additional Boards Manager URLs*: 
     `githubusercontent.com`
   * Go to **Tools** > **Board** > **Boards Manager**, search for `esp32` by Espressif, and click **Install**.

### Flashing the Firmware
1. Clone this repository to your local machine:
   ```bash
   git clone github.com
   ```
2. Open the main `.ino` sketch file in the Arduino IDE.
3. Select your specific board under **Tools** > **Board** > **ESP32 Arduino**.
4. Connect your ESP32 board to your computer via USB.
5. Select the correct serial port under **Tools** > **Port**.
6. Click the **Upload** arrow button in the top left corner.

---

## 📖 How to Use

1. After a successful upload, keep the ESP32 connected to your PC.
2. Open the **Serial Monitor** in the Arduino IDE (**Tools** > **Serial Monitor**).
3. Set the baud rate to **115200**.
4. Press the **EN/RST** button on the ESP32 board to restart the system.
5. Follow the on-screen command line interface (CLI) menus to select tools, change Wi-Fi channels, or start monitoring packets.

---

## 🛡️ Disclaimer

This software is developed strictly for educational purposes, authorized security auditing, and defensive research. Do not use this tool on networks or devices without explicit, written permission from the owner. The author accepts no liability for misuse, network disruption, or legal violations caused by this software.
