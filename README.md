

# 🚗 Smart Parking Gate System (ESP32 + IoT)

This project is an **IoT-based Smart Parking Entry/Exit System** built using **ESP32**, integrating multiple sensors and a backend API for secure vehicle access using OTP verification.

It detects a vehicle, takes OTP input via keypad, identifies vehicle type using a ToF sensor, and verifies access through a cloud backend before opening the gate.

---

## 📌 Features

* 🔐 OTP-based secure vehicle entry/exit
* 📡 WiFi-enabled backend verification (REST API)
* 🚗 Vehicle detection using Ultrasonic Sensor
* 📏 Vehicle type detection using VL53L0X ToF sensor
* ⌨️ Keypad-based OTP input
* 📺 LCD display for user interaction
* 🚪 Servo-controlled automatic gate
* ⏱️ Timeout handling for user input

---

## 🧰 Hardware Requirements

* ESP32 Board
* Ultrasonic Sensor (HC-SR04)
* VL53L0X ToF Distance Sensor
* 16x2 LCD with I2C module
* 4x4 Matrix Keypad
* Servo Motor (SG90 or similar)
* Jumper Wires
* Breadboard / PCB

---

## 🔌 Pin Configuration

| Component           | ESP32 Pin      |
| ------------------- | -------------- |
| Servo Motor         | 18             |
| Ultrasonic TRIG     | 5              |
| Ultrasonic ECHO     | 19             |
| I2C SDA (LCD & ToF) | 21             |
| I2C SCL (LCD & ToF) | 22             |
| Keypad Rows         | 32, 33, 25, 26 |
| Keypad Columns      | 27, 14, 12, 13 |

---

## 📦 Required Libraries (Dependencies)

Install the following libraries in Arduino IDE:

* `WiFi.h` (built-in for ESP32)
* `HTTPClient.h` (built-in)
* `Wire.h` (built-in)
* `LiquidCrystal_I2C`
* `ESP32Servo`
* `VL53L0X`
* `Keypad`

### 📥 Install via Arduino Library Manager

1. Open Arduino IDE
2. Go to **Sketch → Include Library → Manage Libraries**
3. Search and install:

```
LiquidCrystal I2C
ESP32Servo
VL53L0X by Pololu
Keypad by Mark Stanley & Alexander Brevig
```

---

## ⚙️ Installation & Setup Guide

### 1️⃣ Install Arduino IDE

Download and install Arduino IDE:
[https://www.arduino.cc/en/software](https://www.arduino.cc/en/software)

---

### 2️⃣ Install ESP32 Board Support

Open terminal or follow steps:

#### Command-line (if using arduino-cli):

```bash
arduino-cli core update-index
arduino-cli core install esp32:esp32
```

Or manually in Arduino IDE:

* Go to **File → Preferences**
* Add this URL in *Additional Board Manager URLs*:

```
https://dl.espressif.com/dl/package_esp32_index.json
```

* Then go to **Tools → Board → Board Manager**
* Search **ESP32** and install

---

### 3️⃣ Install Required Libraries (CLI)

```bash
arduino-cli lib install "LiquidCrystal I2C"
arduino-cli lib install "ESP32Servo"
arduino-cli lib install "VL53L0X"
arduino-cli lib install "Keypad"
```

---

### 4️⃣ Clone / Setup Project

```bash
git clone https://github.com/your-repo/smart-parking-esp32.git
cd smart-parking-esp32
```

Or manually copy your `.ino` file into Arduino IDE.

---

### 5️⃣ Configure WiFi & Backend API

Edit these lines in the code:

```cpp
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

String serverURL = "https://smart-parking-backend-u47b.onrender.com/api/parking/verify";
```

---

### 6️⃣ Connect ESP32

```bash
arduino-cli board list
```

Find your port (e.g., COM4 or /dev/ttyUSB0)

---

### 7️⃣ Upload Code

```bash
arduino-cli compile --fqbn esp32:esp32:esp32 your_file.ino
arduino-cli upload -p COM4 --fqbn esp32:esp32:esp32 your_file.ino
```

Replace `COM4` with your actual port.

---

### 8️⃣ Open Serial Monitor

```bash
arduino-cli monitor -p COM4 -c baudrate=115200
```

---

## 🔄 System Workflow

1. 🚗 Vehicle detected via Ultrasonic Sensor
2. 📺 LCD prompts user to enter OTP
3. ⌨️ User inputs OTP via keypad
4. 📏 ToF sensor detects vehicle type (bike/car/bus)
5. 🌐 ESP32 sends OTP + vehicle type to backend
6. ✅ If valid → Gate opens
7. ❌ If invalid → Access denied

---

## 📡 API Format

```
GET /api/parking/verify?oneTimeKey=12345&vehicleType=car
```

### Expected Response:

* `ENTRY_OK`
* `EXIT_OK`
* Otherwise → Access Denied

---

## ⚠️ Troubleshooting

* ❌ **ESP32 not connecting** → Check COM port & drivers
* ❌ **WiFi not connecting** → Verify SSID/password
* ❌ **Sensor timeout** → Check wiring (VL53L0X)
* ❌ **Server error** → Ensure backend is live
* ❌ **Servo not moving** → Check power supply

---

## 🚀 Future Improvements

* Mobile App Integration
* QR Code Entry System
* Payment Gateway Integration
* Camera-based License Plate Detection

---

## 👨‍💻 Author

**Md. Mohiuddin Maruf**

* Create **system architecture diagram**
* Add **API documentation (Swagger style)**
