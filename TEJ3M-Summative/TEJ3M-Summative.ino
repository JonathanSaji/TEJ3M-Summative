#include <pitches.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Buzzer pins
int melodyBuzzer = 7;
int bassBuzzer = 8;

// LED pins (active low)
int ledBlue = 6;
int ledYellow = 5;
int ledRed = 4;
int ledGreen = 3;
int ledWhite = 2;

// LCD lyrics timing
const int lyricsCount = 8;
int lyricsIndex = 0;
unsigned long lastLyricsChangeTime = 0;
int lyricsDisplayDuration[] = {1000, 1000, 1000, 1000, 1500, 1000, 1000, 1500}; // milliseconds
int lyricsDisplayCounter = 0;
int currentDisplayInterval = lyricsDisplayDuration[0];
int lyricsRow = 1;

// Melody buzzer timing
unsigned long melodyNoteEndTime = 0;
bool melodyNoteActive = false;
int melodyNoteIndex = 0;

// Bass buzzer timing
unsigned long bassNoteEndTime = 0;
bool bassNoteActive = false;
int bassNoteIndex = 0;


// Buzzer PWM timing
unsigned long lastMelodyToggleTime = 0;
unsigned long lastBassToggleTime = 0;
unsigned long melodyHalfPeriod = 0;
unsigned long bassHalfPeriod = 0;
bool melodyBuzzerState = false;
bool bassBuzzerState = false;
long currentMicrosTime = 0;
long currentBassMicrosTime = 0;

// Track active LED for proper on/off control
int lastActiveLED = -1;


String lyrics[lyricsCount] = {"Happy Birthday","To You","Happy Birthday","To You","Happy Birthday","Dear JAYDEN","Happy Birthday","To You!"};
const int songLength = 25;
int melodyNotes[songLength] = {NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_F4, NOTE_E4, NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4,
                               NOTE_G4, NOTE_F4, NOTE_C4, NOTE_C4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_E4, NOTE_D4, NOTE_AS4,
                               NOTE_AS4, NOTE_A4, NOTE_F4, NOTE_G4, NOTE_F4};
int melodyDurations[songLength] = {3, 4, 2, 2, 2, 1, 3, 4, 2, 2, 2, 1, 3, 4, 2, 2, 2, 2, 2, 4, 4, 2, 2, 2, 1};

int bassDurations[songLength] = {4, 4, 4, 4, 4, 4, 4, 4,4,
                                 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
                                 4, 4, 4, 4, 4};

int bassNotes[songLength] = {REST,REST,NOTE_F4,REST,REST,NOTE_C4,REST,REST,NOTE_C3,REST,
                            REST,NOTE_F4,REST,REST,NOTE_F4,REST,REST,NOTE_AS4,REST,REST,
                            REST,NOTE_F4,REST,NOTE_C4,NOTE_F4};


int noteDurationSignature = 1500;
void setup() {
    Serial.begin(9600);

    for (int i = 2; i <= 8; i++) {
        pinMode(i, OUTPUT);
        if (i != 7 && i != 8) {
            digitalWrite(i, HIGH);
        }
    }

    // Initialize LCD
    lcd.init();
    lcd.backlight();
    lcd.clear();
}   

void loop() {
    unsigned long currentTime = millis();
    // Check if it's time to show the next message
    if (currentTime - lastLyricsChangeTime >= currentDisplayInterval && lyricsIndex < lyricsCount) {
    
        // Loop back to start when reaching the end
        if (lyricsIndex >= lyricsCount) {
            lyricsIndex = 0;
        }

        // Clear and display the new message
        lcd.clear();
        if (lyricsRow == 1) {
            lcd.setCursor(0, 1);
            lcd.print(lyrics[lyricsIndex]);
            lyricsRow--;
        }
        else if (lyricsRow == 0) {
            lcd.setCursor(0, 0);
            lcd.print(lyrics[lyricsIndex]);
            lcd.setCursor(0, 1);
            // If it gets to the last lyric, loop back to the first
            if (lyricsIndex == lyricsCount - 1) {
                lcd.print(lyrics[0]);
            }
            else {
                lcd.print(lyrics[lyricsIndex + 1]);
            }
            lyricsRow++;
            lyricsIndex++;
        }
        if (lyricsDisplayCounter >= lyricsCount) {
            lyricsDisplayCounter = 0;
        }
        currentDisplayInterval = lyricsDisplayDuration[lyricsDisplayCounter++];
        lastLyricsChangeTime = currentTime;
    }

    // Buzzer note handling
    if (melodyNoteIndex < songLength) {
        if (!melodyNoteActive) {
            // Turn off previously active LED
            if (lastActiveLED != -1) {
                digitalWrite(lastActiveLED, HIGH);
            }
            
            // Start new note and light up corresponding LED
            switch (melodyNotes[melodyNoteIndex]) {
                case NOTE_C4:
                case NOTE_C5:
                    digitalWrite(ledBlue, LOW);
                    lastActiveLED = ledBlue;
                    break;
                case NOTE_D4:
                    digitalWrite(ledYellow, LOW);
                    lastActiveLED = ledYellow;
                    break;
                case NOTE_F4:
                    digitalWrite(ledRed, LOW);
                    lastActiveLED = ledRed;
                    break;
                case NOTE_G4:
                case NOTE_E4:
                    digitalWrite(ledGreen, LOW);
                    lastActiveLED = ledGreen;
                    break;
                case NOTE_A4:
                case NOTE_AS4:
                    digitalWrite(ledWhite, LOW);
                    lastActiveLED = ledWhite;
                    break;
                default:
                    lastActiveLED = -1;
                    break;
            }
            // Calculate half period for buzzer toggle (in microseconds)
            melodyHalfPeriod = 1000000 / (melodyNotes[melodyNoteIndex] * 2);
            bassHalfPeriod = 1000000 / (bassNotes[melodyNoteIndex] * 2);
            lastMelodyToggleTime = micros();
            lastBassToggleTime = micros();

            melodyBuzzerState = false;
            bassBuzzerState = false;

            digitalWrite(melodyBuzzer, LOW);
            digitalWrite(bassBuzzer, LOW);

            melodyNoteEndTime = currentTime + (noteDurationSignature / melodyDurations[melodyNoteIndex]);
            melodyNoteActive = true;

            bassNoteEndTime = currentTime + (noteDurationSignature / bassDurations[melodyNoteIndex]);
            bassNoteActive = true;
        }
        
        // Toggle buzzer pins for continuous tone
        currentMicrosTime = micros();
        playMelody(melodyHalfPeriod, melodyBuzzer);
        playBass(bassHalfPeriod, bassBuzzer);
        
        if (currentTime >= melodyNoteEndTime) {
            // Note time is done - turn off the LED that was on
            if (lastActiveLED != -1) {
                digitalWrite(lastActiveLED, HIGH);
            }
            digitalWrite(melodyBuzzer, LOW);
            digitalWrite(bassBuzzer, LOW);
            melodyNoteIndex++;
            bassNoteIndex++;
            melodyNoteActive = false;
            bassNoteActive = false;
        }
    }
    else {
        delay(1000); // Wait a second before restarting
        lcd.clear();
    }
}

void playMelody(long halfPeriod, int buzzerPin) {
    if (currentMicrosTime - lastMelodyToggleTime >= halfPeriod) {
        melodyBuzzerState = !melodyBuzzerState;
        digitalWrite(buzzerPin, melodyBuzzerState ? HIGH : LOW);
        lastMelodyToggleTime = currentMicrosTime;
    }
}


void playBass(long halfPeriod, int buzzerPin) {
    currentBassMicrosTime = micros();
    if (currentBassMicrosTime - lastBassToggleTime >= halfPeriod) {
        bassBuzzerState = !bassBuzzerState;
        digitalWrite(buzzerPin, bassBuzzerState ? HIGH : LOW);
        lastBassToggleTime = currentBassMicrosTime;
    }
}





/*

NOTES

From https://docs.arduino.cc/language-reference/en/functions/advanced-io/tone/ 
"If you want to play different pitches on multiple pins, you need to call noTone() 
  on one pin before calling tone() on the next pin."
  
  */



