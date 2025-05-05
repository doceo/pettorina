#include <SPI.h>
#include "RF24.h"

#define CE_PIN 7
#define CSN_PIN 8
RF24 radio(CE_PIN, CSN_PIN);

uint8_t address[][6] = {"1Node", "2Node"};
bool radioNumber = 0;  // Utilizza "1Node" come indirizzo

void setup() {
  Serial.begin(115200);
  if (!radio.begin()) {
    Serial.println("Errore radio!");
    while (1);
  }
  
  radio.setPALevel(RF24_PA_LOW);
  radio.openReadingPipe(1, address[!radioNumber]);
  radio.startListening();
  Serial.println("RX pronto. In attesa di dati...");
}

void loop() {
  if (radio.available()) {
    int battiti;
    radio.read(&battiti, sizeof(battiti));
    Serial.print("Battiti rilevati (5s): ");
    Serial.print(battiti);
    Serial.print(" | BPM: ");
    Serial.println(battiti * 12);  // Conversione in battiti/minuto
  }
}