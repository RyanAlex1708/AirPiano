#include <SoftwareSerial.h>

// Pin assignments
const int buzzer = 8;
const int trig_pin = 9;
const int echo_pin = 10;
const int ledPins[] = {2, 3, 4, 5, 6}; 


SoftwareSerial BT(0, 1);  

// Notes frequencies
const int NOTE_C = 261;
const int NOTE_D = 294;
const int NOTE_E = 329;
const int NOTE_F = 349;
const int NOTE_G = 392;
const int ALARM = 520;

// Melodies
const int original[] = {NOTE_C, NOTE_D, NOTE_E, NOTE_F, NOTE_G}; 
const int melody1[] = {NOTE_C, NOTE_D, NOTE_E}; 
const int melody2[] = {NOTE_E, NOTE_D, NOTE_C, NOTE_D, NOTE_E}; 
const int melody3[] = {NOTE_C, NOTE_E, NOTE_D, NOTE_F, NOTE_E, NOTE_G}; 
const int melody4[] = {NOTE_E, NOTE_D, NOTE_C, NOTE_D, NOTE_E, NOTE_E, NOTE_E};

// Durations for melodies
const int duration1[] = {300, 300, 300}; 
const int duration2[] = {300, 300, 300, 300, 300}; 
const int duration3[] = {300, 300, 300, 300, 300, 300};
const int duration4[] = {300, 300, 300, 300, 300, 300, 300};

const int* melodies[] = {melody1, melody2, melody3, melody4};
const int* noteDurations[] = {duration1, duration2, duration3, duration4};
const int melodyLengths[] = {3, 5, 6, 7};

int currentMelody = 0; 
int userInput[7]; 
int currentIndex = 0;
float distance = 0.0;
bool startGame = false;  

void setup() {
  pinMode(echo_pin, INPUT);
  pinMode(trig_pin, OUTPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);
  BT.begin(9600); // Start Bluetooth communication
  
  for (int i = 0; i < 5; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
  Serial.println("Waiting for Bluetooth signal...");
}

void loop() {
  // Check for Bluetooth signal from MIT App
  if (BT.available()) {
    Serial.println("Bluetooth Connected");
    char command = BT.read();
    if (command == '1') {
      startGame = true;  // Set flag to start the game
      Serial.println("Start Signal Received from App!");
      playMelody();
    } 
  }

  if (!startGame) {
    return; // Wait until the signal "1" is received
  }

  distance = getDistance();
  int distance = getDistance();
  int detectedNote = detectNote(distance);
  Serial.println(detectedNote);
  if (detectedNote != -1) {
    int noteToPlay = original[detectedNote]; 

    // Get the correct LED index for the note being played
    int ledIndex = getLEDIndex(noteToPlay);

    // Play the note and light up the corresponding LED
    Serial.println("Note Played");
    tone(buzzer, noteToPlay);
    lightUpLED(detectedNote); 
    delay(300);
    noTone(buzzer);
    turnOffLEDs();

    // Store user input and check if it matches the melody
    userInput[currentIndex] = detectedNote;

    if (detectedNote == getNoteIndexFromMelody(currentIndex)) {
      currentIndex++;

      if (currentIndex == melodyLengths[currentMelody]) {
        Serial.println("Correct Sequence!");

        if (currentMelody < 3) {
          currentMelody++;
          Serial.println("Next melody!");
          delay(1000);
          playMelody();
        } else {
          Serial.println("Congratulations! You completed all melodies!");
          for (int i = 0; i < 5; i++) {
            digitalWrite(ledPins[i], HIGH);
            delay(50);
            digitalWrite(ledPins[i], LOW);
            delay(50);
          } for (int i = 4; i >= 0; i--) {
            digitalWrite(ledPins[i], HIGH);
            delay(50);
            digitalWrite(ledPins[i], LOW);
            delay(50);
          }
          currentMelody = 0;
        }

        resetGame();
      }
    } else {
      Serial.println("Wrong note! Try again.");
      flashAllLEDs();
      resetGame();
      playMelody();
    }
  }

  delay(200);
}

// Function to play the current melody
void playMelody() {
  Serial.print("Playing Melody ");
  Serial.println(currentMelody + 1);

  for (int i = 0; i < melodyLengths[currentMelody]; i++) {
    int note = melodies[currentMelody][i]; 
    int ledIndex = getLEDIndex(note); 

    tone(buzzer, note); 
    lightUpLED(ledIndex); 
    delay(noteDurations[currentMelody][i]);
    noTone(buzzer);
    turnOffLEDs();
    delay(200);
  }
}

int getLEDIndex(int note) {
  if (note == NOTE_C) return 0; 
  if (note == NOTE_D) return 1; 
  if (note == NOTE_E) return 2; 
  if (note == NOTE_F) return 3; 
  if (note == NOTE_G) return 4; 
  return -1; 
}

int getNoteIndexFromMelody(int index) {
  int expectedNote = melodies[currentMelody][index]; 

  if (expectedNote == NOTE_C) return 0;
  if (expectedNote == NOTE_D) return 1;
  if (expectedNote == NOTE_E) return 2;
  if (expectedNote == NOTE_F) return 3;
  if (expectedNote == NOTE_G) return 4;

  return -1; 
}

// Function to detect note based on distance
int detectNote(int distance) {
  if (distance < 15) return 0; 
  else if (distance < 25) return 1; 
  else if (distance < 35) return 2; 
  else if (distance < 45) return 3; 
  else if (distance < 55) return 4; 
  else return -1; 
}

// Function to reset game progress
void resetGame() {
  currentIndex = 0;
  delay(500);
}


// Function to turn on the correct LED
void lightUpLED(int note) {
  digitalWrite(ledPins[note], HIGH);
}

// Function to turn off all LEDs
void turnOffLEDs() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(ledPins[i], LOW);
  }
}

// Function to flash all LEDs for error feedback
void flashAllLEDs() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(ledPins[i], HIGH);
  }
  delay(100);
  for (int i = 0; i < 5; i++) {
    digitalWrite(ledPins[i], LOW);
  }
  delay(100);
   for (int i = 0; i < 5; i++) {
    digitalWrite(ledPins[i], HIGH);
  }
  delay(100);
  for (int i = 0; i < 5; i++) {
    digitalWrite(ledPins[i], LOW);
  }
  delay(50);
}

// Function to get the distance from the ultrasonic sensor
int getDistance() {
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);
  
  int timing = pulseIn(echo_pin, HIGH);
  return (timing * 0.034) / 2;
}
