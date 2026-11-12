#include <SoftwareSerial.h>
#include <EBYTE_LoRa_E220.h>

#define LED_PIN LED_BUILTIN
#define LORA_RX 3
#define LORA_TX 1

SoftwareSerial loraSerial(LORA_RX, LORA_TX);
EBYTE_LoRa_E220 e220(&loraSerial);

const unsigned long BLINK_DURATION = 10000;
const unsigned long BLINK_INTERVAL = 1000;
bool blinking = false;
unsigned long blinkStart = 0;
bool ledState = false;
unsigned long lastToggle = 0;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  Serial.begin(115200);
  loraSerial.begin(9600);
  delay(100);

  e220.init();
  Serial.println("E220 ready");
}

void loop() {
  if (e220.available() > 1) {
    String message = e220.receiveMessage().data;
    message.trim();

    Serial.print("Received: ");
    Serial.println(message);

    if (message == "BLINK") {
      blinking = true;
      blinkStart = millis();
      ledState = false;
      lastToggle = millis();
      digitalWrite(LED_PIN, HIGH);
      Serial.println("Blink started");
    }
  }

  if (blinking) {
    unsigned long now = millis();

    if (now - blinkStart >= BLINK_DURATION) {
      blinking = false;
      digitalWrite(LED_PIN, HIGH);
      Serial.println("Blink finished");
    } else {
      if (now - lastToggle >= BLINK_INTERVAL) {
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState ? LOW : HIGH);
        lastToggle = now;
      }
    }
  }
}
