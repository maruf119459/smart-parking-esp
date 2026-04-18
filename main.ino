#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h> 
#include <VL53L0X.h>
#include <Keypad.h>

// ================= WIFI =================
const char* ssid = "hello123";
const char* password = "hello123";
String serverURL = "https://smart-parking-backend-u47b.onrender.com/api/parking/verify";

// ================= LCD =================
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ================= SERVO =================
Servo gateServo;
const int servoPin = 18;

// ================= ULTRASONIC =================
#define TRIG 5
#define ECHO 19

// ================= TOF SENSOR =================
VL53L0X sensor;

// ================= KEYPAD SETUP =================
const byte ROWS = 4; 
const byte COLS = 4; 
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {32, 33, 25, 26}; 
byte colPins[COLS] = {27, 14, 12, 13}; 

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ================= VARIABLES =================
String otp = "";

// ================= FUNCTIONS =================
long readUltrasonic() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long duration = pulseIn(ECHO, HIGH, 30000); 
  return duration * 0.034 / 2;
}

String getVehicleType() {
  Serial.println("--- VL53L0X Reading ---");
  uint16_t distance = sensor.readRangeSingleMillimeters();
  
  if (sensor.timeoutOccurred()) {
    Serial.println("STATUS: ToF Sensor Timeout!");
    return "unknown";
  }
  
  float cm = distance / 10.0;
  
  Serial.print("RAW DISTANCE: "); Serial.print(distance); Serial.println(" mm");
  Serial.print("CALCULATED HEIGHT: "); Serial.print(cm); Serial.println(" cm");

  if (cm <= 3.0) return "bike";
  if (cm <= 13.0) return "car";
  if (cm <= 20.0) return "bus";
  return "unknown";
}

void openGate() {
  Serial.println("Action: Opening Gate...");
  gateServo.write(90);
  delay(3000); 
  Serial.println("Action: Closing Gate...");
  gateServo.write(0);
}

void sendToServer(String otpCode, String vehicleType) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = serverURL + "?oneTimeKey=" + otpCode + "&vehicleType=" + vehicleType;
    Serial.println("HTTP Request: " + url);
    
    http.begin(url);
    http.setConnectTimeout(5000); 
    
    int code = http.GET();
    Serial.print("HTTP Status: "); Serial.println(code);

    if (code > 0) {
      String response = http.getString();
      Serial.println("Response: " + response);
      lcd.clear();
      if (response.indexOf("ENTRY_OK") != -1 || response.indexOf("EXIT_OK") != -1) {
        lcd.print("Access Granted");
        lcd.setCursor(0,1);
        lcd.print(vehicleType);
        openGate();
      } else {
        lcd.print("Access Denied");
      }
    } else {
      Serial.println("Error: Backend Unreachable");
      lcd.clear();
      lcd.print("Server Error");
    }
    http.end();
  } else {
    Serial.println("Error: WiFi lost");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("System starting...");

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  ESP32PWM::allocateTimer(0);
  gateServo.setPeriodHertz(50); 
  gateServo.attach(servoPin, 500, 2400); 
  gateServo.write(0);

  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();

  if (!sensor.init()) {
    Serial.println("CRITICAL: VL53L0X not found!");
  }
  sensor.setTimeout(500);

  WiFi.begin(ssid, password);
  lcd.print("WiFi Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");
  lcd.clear();
  lcd.print("System Ready");
}

void loop() {
  long distance = readUltrasonic();

  if (distance > 0 && distance < 10) { 
    Serial.print("Ultrasonic Trigger: "); Serial.print(distance); Serial.println(" cm");
    lcd.clear();
    lcd.print("Vehicle Present");
    delay(1000);
    lcd.clear();
    lcd.print("Enter OTP:");
    lcd.setCursor(0, 1);
    
    otp = "";
    String maskedOtp = ""; // To show '*' on screen
    bool submitted = false;
    unsigned long startTime = millis();

    // Loop for 30 seconds to capture keypad input
    while (millis() - startTime < 30000) {
      char key = keypad.getKey();
      
      if (key) {
        // 'C' is not on your 4x4 layout, so '*' or 'A/B/C/D' can be used. 
        // Based on your prompt, I'm using '*' as Clear (C) and '#' as Submit.
        if (key == '*' || key == 'C') { 
          otp = "";
          maskedOtp = "";
          lcd.setCursor(0, 1);
          lcd.print("                "); // Clear line
          lcd.setCursor(0, 1);
          Serial.println("Input Cleared");
        } 
        else if (key == '#') {
          if (otp.length() > 0) {
            Serial.println("OTP Submitted: " + otp);
            submitted = true;
            break;
          }
        } 
        else if (isdigit(key)) { // Only accept numbers
          otp += key;
          maskedOtp += "*";
          lcd.setCursor(0, 1);
          lcd.print(maskedOtp);
          Serial.print("Key Pressed: "); Serial.println(key);
        }
      }
    }

    if (submitted) {
      lcd.clear();
      lcd.print("Verifying...");
      String type = getVehicleType(); 
      sendToServer(otp, type);
    } else {
      lcd.clear();
      lcd.print("Timed Out");
      Serial.println("User timed out (No input received).");
    }
    
    delay(3000);
    lcd.clear();
    lcd.print("System Ready");
  }
  delay(200);
}
