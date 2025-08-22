// -----------------------------
// CONFIGURAZIONE - PIN E MAPPATURE
// -----------------------------

// Pin analogici dita (uno per dito)
const int pinDita[4] = {5, 6, 7, 8};  // Indice, Medio, Anulare, Mignolo

// Nomi dita e parti per stampa
const char* nomiDita[4]  = {"Indice", "Medio", "Anulare", "Mignolo"};
const char* nomiParti[4] = {"Unghia", "Polpastrello", "Meta", "Base"};

// Lettere associate a ciascuna parte di ciascun dito
char lettere[4][4] = {
  {'A', 'B', 'C', 'D'},  // Indice
  {'E', 'F', 'G', 'H'},  // Medio
  {'I', 'J', 'K', 'L'},  // Anulare
  {'M', 'N', 'O', 'P'}   // Mignolo
};

// Range soglie analogiche per segmento dito {min, max}
// Ordine corrisponde a: Base, Meta, Polpastrello, Unghia
int soglie[4][2] = {
  {50, 250},     // Base
  {300, 500},    // Meta
  {550, 750},    // Polpastrello
  {800, 1023}    // Unghia
};

// --- Trackball input pin
const int trakballInputPins[5] = {0, 1, 2, 3, 4}; // Su, Giu, Destra, Sinistra, Button
const char* direzioniTrackball[5] = {"SU", "GIU", "DESTRA", "SINISTRA", "BUTTON"};

// --- Pin motore vibrazione e output trackball
const int motoreVibrazionePin = 11;  // Motore vibrazione
const int trakballLedBlu      = 12;
const int trakballLedVerde    = 13;
const int trakballLedRosso    = 14;
const int trakballLedBianco   = 15;


// -----------------------------
// GESTIONE TEMPI DEBOUNCE E FEEDBACK APTICO
// -----------------------------
const unsigned long debounceMs = 200;  // Ritardo debounce tra letture per lo stesso input
const int vibraMs = 60;                 // Durata vibrazione (ms)

unsigned long lastFeedbackDita[4][4]      = {0};  // Ultimo feedback dato per dito e parte
unsigned long lastFeedbackTrackball[5]   = {0};  // Ultimo feedback dato per pulsanti trackball


// -----------------------------
// FUNZIONI AUSILIARIE
// -----------------------------

// Ritorna indice segmento dito corrispondente al valore analogico letto oppure -1 se fuori soglia
int parteDito(int valore) {
  for (int i = 0; i < 4; i++) {
    if (valore >= soglie[i][0] && valore <= soglie[i][1]) {
      return i;
    }
  }
  return -1;
}

// Attiva vibrazione motore per vibraMs millisecondi
void vibrazione() {
  digitalWrite(motoreVibrazionePin, HIGH);
  delay(vibraMs);
  digitalWrite(motoreVibrazionePin, LOW);
}


// -----------------------------
// SETUP
// -----------------------------
void setup() {
  Serial.begin(115200);

  // Configura pin analogici dita come input
  for (int i = 0; i < 4; i++) {
    pinMode(pinDita[i], INPUT);
  }

  // Configura pin digitali trackball input come input con pull-up
  for (int i = 0; i < 5; i++) {
    pinMode(trakballInputPins[i], INPUT_PULLUP);
  }

  // Configura pin output
  pinMode(motoreVibrazionePin, OUTPUT);
  pinMode(trakballLedBlu, OUTPUT);
  pinMode(trakballLedVerde, OUTPUT);
  pinMode(trakballLedRosso, OUTPUT);
  pinMode(trakballLedBianco, OUTPUT);
}


// -----------------------------
// LOOP PRINCIPALE
// -----------------------------
void loop() {
  // ----------------------
  // Rilevazione dita
  // ----------------------
  for (int dito = 0; dito < 4; dito++) {
    int valoreAnalogico = analogRead(pinDita[dito]);
    int parte = parteDito(valoreAnalogico);
    if (parte != -1) {
      if (millis() - lastFeedbackDita[dito][parte] > debounceMs) {
        Serial.print(lettere[dito][parte]);
        Serial.print(" premuto (");
        Serial.print(nomiDita[dito]);
        Serial.print(" ");
        Serial.print(nomiParti[parte]);
        Serial.print(") Valore: ");
        Serial.println(valoreAnalogico);

        vibrazione();
        lastFeedbackDita[dito][parte] = millis();
      }
    }
  }


  // ----------------------
  // Rilevazione trackball
  // ----------------------
  for (int t = 0; t < 5; t++) {
    if (digitalRead(trakballInputPins[t]) == LOW) {  // Pulsante premuto (INPUT_PULLUP)
      if (millis() - lastFeedbackTrackball[t] > debounceMs) {
        Serial.print("Trackball: ");
        Serial.print(direzioniTrackball[t]);
        Serial.println(" premuta");

        vibrazione();
        lastFeedbackTrackball[t] = millis();
      }
    }
  }

  delay(20); // Delay di ciclo per stabilità
}
