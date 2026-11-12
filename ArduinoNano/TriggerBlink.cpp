#include <SoftwareSerial.h>

const int sensorPin = A0;

// --- ПИНЫ ДЛЯ МОДУЛЯ E220 ---
#define E220_RX_PIN 3  
#define E220_TX_PIN 2  
#define M0_PIN     4   
#define M1_PIN     5   

SoftwareSerial mySerial(E220_RX_PIN, E220_TX_PIN);

// --- НАСТРОЙКИ ---
float alphaSTA = 0.6;     
float alphaLTA = 0.001;   
float triggerRatio = 3.0; 

float sta = 0;
float lta = 0;
float baseline = 512;      

bool alarmSent = false;
unsigned long lastAlarmTime = 0;
const unsigned long cooldown = 150; 

void setup() {
  Serial.begin(57600); 
  mySerial.begin(9600); 
  
  pinMode(M0_PIN, OUTPUT);
  pinMode(M1_PIN, OUTPUT);
  digitalWrite(M0_PIN, LOW);
  digitalWrite(M1_PIN, LOW);
  delay(100); 
}

void loop() {
  int raw = analogRead(sensorPin);
  
  baseline = baseline + 0.0005 * (raw - baseline);
  float val = abs(raw - baseline); 
  
  sta = sta + alphaSTA * (val - sta);
  lta = lta + alphaLTA * (val - lta);
  
  float ratio = 0;
  if (lta > 1.0) { 
    ratio = sta / lta;
  }
  
  bool isHit = false; 
  
  // Логика удара (порог 3.0 и реальный удар > 15)
  if (ratio > triggerRatio && val > 15 && !alarmSent && (millis() - lastAlarmTime > cooldown)) {
    alarmSent = true;
    lastAlarmTime = millis();
    isHit = true;
  }
  
  if (ratio < 1.1) {
    alarmSent = false;
  }
  
  // 1. ОТПРАВЛЯЕМ LORA (если был удар)
  if (isHit) {
    mySerial.print("BLINK\n"); 
    // Даем SoftwareSerial и USB-порту 20 мс, чтобы все восстановиться
    delay(20); 
  }

  // 2. ВЫВОДИМ ТОЛЬКО ЦИФРЫ НА ГРАФИК
  // Никаких "Ratio:" или "Threshold:". Только числа через запятую.
  Serial.print(ratio, 1);           // Линия 1 (Value 1) - это Ratio
  Serial.print(",");
  Serial.print(triggerRatio, 1);    // Линия 2 (Value 2) - это Threshold (порог)
  Serial.print(",");
  
  if (isHit) {
    Serial.println("5");            // Линия 3 (Value 3) - Hit (подскочит до 5)
  } else {
    Serial.println("0");
  }
  
  delay(30); 
}
