#include <Arduino.h>
#line 1 "C:\\Users\\Jonathan\\Documents\\TEJ3M-Summative\\TEJ3M-Summative\\TEJ3M-Summative.ino"
#include <pitches.h>
#include <LiquidCrystal_I2C.h>
#include "songs.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Buzzer pins
const int melodyBuzzer = 7;
const int bassBuzzer = 9;

// LED pins (active low)
const int ledBlue = 6;
const int ledYellow = 5;
const int ledRed = 4;
const int ledGreen = 3;
const int ledWhite = 2;

// LCD lyrics timing
const int HBDlyricsCount = 8;
int lyricsCount = 0; // To be set when loading a song
int lyricsIndex = 0;
unsigned long lastLyricsChangeTime = 0;
int lyricsDisplayDuration[100] = {0}; // milliseconds
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
bool playMusic = false;

String lyrics[250] = {"PAUSE"};
String HBDlyrics [HBDlyricsCount] = {"Happy Birthday", "To You", "Happy Birthday", "To You", "Happy Birthday", "Dear JAYDEN", "Happy Birthday", "To You!"};
int happybirthdayLyricsDuration[HBDlyricsCount] = {2000, 1500, 2000, 1500, 2000, 2500, 2000, 3000};
int songLength = 0;



// Default empty arrays
int melodyNotes[250] = {REST};
int melodyDurations[250] = {REST};
int bassDurations[250] = {REST};
int bassNotes[250] = {REST};

int noteDurationSignature = 0;

#line 67 "C:\\Users\\Jonathan\\Documents\\TEJ3M-Summative\\TEJ3M-Summative\\TEJ3M-Summative.ino"
void setup();
#line 84 "C:\\Users\\Jonathan\\Documents\\TEJ3M-Summative\\TEJ3M-Summative\\TEJ3M-Summative.ino"
void loop();
#line 256 "C:\\Users\\Jonathan\\Documents\\TEJ3M-Summative\\TEJ3M-Summative\\TEJ3M-Summative.ino"
void playMelody(long halfPeriod, int buzzerPin);
#line 264 "C:\\Users\\Jonathan\\Documents\\TEJ3M-Summative\\TEJ3M-Summative\\TEJ3M-Summative.ino"
void playBass(long halfPeriod, int buzzerPin);
#line 273 "C:\\Users\\Jonathan\\Documents\\TEJ3M-Summative\\TEJ3M-Summative\\TEJ3M-Summative.ino"
void playMenu();
#line 278 "C:\\Users\\Jonathan\\Documents\\TEJ3M-Summative\\TEJ3M-Summative\\TEJ3M-Summative.ino"
void setVariablesToPlay();
#line 67 "C:\\Users\\Jonathan\\Documents\\TEJ3M-Summative\\TEJ3M-Summative\\TEJ3M-Summative.ino"
void setup() {
    Serial.begin(9600);

    for (int i = 2; i <= 9; i++) {
        pinMode(i, OUTPUT);
        if (i != 7 && i != 9) {
            digitalWrite(i, HIGH);
        }
    }

    // Initialize LCD
    lcd.init();
    lcd.backlight();
    lcd.clear();
    playMenu();
}

void loop() {
    unsigned long currentTime = millis();

    // Check for serial input - read entire line until newline
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim(); // Remove any whitespace
        
        if (input == "1") {
            for (int i = 0; i < happyBirthdayLength; i++) {
                melodyNotes[i] = happyBirthdayMelody[i];
                melodyDurations[i] = happyBirthdayDurations[i];
                bassNotes[i] = happyBirthdaybassNotes[i];
                bassDurations[i] = happyBirthdayBassDurations[i];
            }
            for(int i = 0; i < HBDlyricsCount; i++) {
                lyrics[i] = HBDlyrics[i];
                lyricsDisplayDuration[i] = happybirthdayLyricsDuration[i];
            }
            songLength = happyBirthdayLength;
            playMusic = true;
            lyricsIndex = 0;
            lastLyricsChangeTime = currentTime;
            lyricsDisplayCounter = 0;
            melodyNoteIndex = 0;
            bassNoteIndex = 0;
            noteDurationSignature = happybirthdayNoteDurationSignature;
            lyricsCount = HBDlyricsCount;
        } 
        else if(input == "2") {
            for (int i = 0; i < GiveYouUpLength; i++) {
                melodyNotes[i] = GiveYouUpNotes[i];
                melodyDurations[i] = GiveYouUpDurations[i];
                //bassNotes[i] = GiveYouUpBassNotes[i]; // No bass for this song
                //bassDurations[i] = GiveYouUpBassDurations[i];
                bassNotes[i] = REST;
                bassDurations[i] = REST;    
            }
            songLength = GiveYouUpLength;
            playMusic = true;
            lyricsIndex = 0;
            lastLyricsChangeTime = currentTime;
            lyricsDisplayCounter = 0;
            melodyNoteIndex = 0;
            bassNoteIndex = 0;
            noteDurationSignature = GiveYouUpNoteDurationSignature;
        }
        else {
            lcd.clear();
            lcd.print("Invalid Option");
            delay(2000);
            lcd.clear();
            playMenu();
        }
    }

    if (playMusic) {
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
            } else if (lyricsRow == 0) {
                lcd.setCursor(0, 0);
                lcd.print(lyrics[lyricsIndex]);
                lcd.setCursor(0, 1);
                // If it gets to the last lyric, loop back to the first
                if (lyricsIndex == lyricsCount - 1) {
                    lcd.print(lyrics[0]);
                } else {
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
        } else {
            delay(1000); // Wait a second before restarting
            lcd.clear();
            playMusic = false;
            playMenu();
        }
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

void playMenu() {
    lcd.setCursor(0, 0);
    lcd.print("1 = HBD 2 = NS");
}

void setVariablesToPlay(){

}

/*
 * NOTES
 * From https://docs.arduino.cc/language-reference/en/functions/advanced-io/tone/
 * "If you want to play different pitches on multiple pins, you need to call noTone()
 *  on one pin before calling tone() on the next pin."
 */




