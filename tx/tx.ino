#include <SPI.h>
#include "RF24.h"

#define CE_PIN 7
#define CSN_PIN 8
RF24 radio(CE_PIN, CSN_PIN);

uint8_t address[][6] = {"1Node", "2Node"};
bool radioNumber = 1;  // Utilizza "2Node" come indirizzo
const int accelerometerPin = A0;

// Configurazione campionamento
const int sampleInterval = 50;     // Intervallo tra campioni (ms)
const int sampleDuration = 5000;    // Durata totale (ms)
const int sampleCount = sampleDuration / sampleInterval;
int samples[sampleCount];

int payload = 0;
enum State { WAITING, SAMPLING };
State currentState = WAITING;
unsigned long lastSampleTime = 0;
int currentSample = 0;

void setup() {
  Serial.begin(115200);
  if (!radio.begin()) {
    Serial.println("Errore radio!");
    while (1);
  }
  
  radio.setPALevel(RF24_PA_LOW);
  radio.setPayloadSize(sizeof(payload));
  radio.openWritingPipe(address[radioNumber]);
  radio.stopListening();
  
  pinMode(accelerometerPin, INPUT);
  Serial.println("TX pronto. Invio 'S' per iniziare la rilevazione.");
}

void loop() {
  if (currentState == WAITING && Serial.available() && Serial.read() == 'S') {
    currentState = SAMPLING;
    currentSample = 0;
    lastSampleTime = millis();
    Serial.println("Rilevazione iniziata...");
  }

  if (currentState == SAMPLING) {
    if (millis() - lastSampleTime >= sampleInterval) {
      samples[currentSample] = analogRead(accelerometerPin);
      lastSampleTime = millis();
      
      if (++currentSample >= sampleCount) {
        // Calcola battiti
        int battiti = 0;
        for (int i = 1; i < sampleCount; i++) {
          if (samples[i-1] * samples[i] < 0) battiti++;
        }
        payload = battiti;
        
        if (radio.write(&payload, sizeof(payload))) {
          Serial.print("Battiti inviati: ");
          Serial.println(payload);
        } else {
          Serial.println("Invio fallito!");
        }
        currentState = WAITING;
        Serial.println("Pronto per una nuova misura. Invio 'S'");
      }
    }
  }
}