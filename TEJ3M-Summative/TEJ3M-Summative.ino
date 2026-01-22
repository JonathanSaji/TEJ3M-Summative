#include <pitches.h>
#include <LiquidCrystal_I2C.h>
#include <avr/pgmspace.h>
#include "songs.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

//Time
unsigned long currentTime;


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

String lyrics[260] = {"PAUSE"};

int songLength = 0;



// Default empty arrays
int melodyNotes[260] = {REST};
int melodyDurations[260] = {REST};
int bassDurations[260] = {REST};
int bassNotes[260] = {REST};

int noteDurationSignature = 0;

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
    currentTime = millis();

    // Check for serial input - read entire line until newline
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim(); // Remove any whitespace
        
        if (input == "1") {
            setVariablesToPlay(happyBirthdayLength, happyBirthdayMelody, happyBirthdayDurations, happyBirthdaybassNotes, 
                                happyBirthdayBassDurations, happybirthdayNoteDurationSignature, HBDlyricsCount, HBDlyrics, 
                                happybirthdayLyricsDuration);
                                    
        } 
        else if(input == "2") {
            setVariablesToPlay(GiveYouUpLength, GiveYouUpNotes, GiveYouUpDurations, GiveYouUpBassNotes, 
                                GiveYouUpBassDurations, GiveYouUpNoteDurationSignature, GiveYouUpLyricsCount, GiveYouUpLyrics, 
                                GiveYouUpLyricsDuration);
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

void setVariablesToPlay(const int length, const int melodyArray[], const int melodyDurArray[], const int bassArray[], 
                        const int bassDurArray[], const int durationSignature, const int lyricCount, const String lyricArray[], 
                        const int lyricDurArray[]) {
    for(int i = 0; i < lyricCount; i++) {
                lyrics[i] = lyricArray[i];
                lyricsDisplayDuration[i] = pgm_read_int(&lyricDurArray[i]);
            }
    
    for (int i = 0; i < length; i++) {
                melodyNotes[i] = pgm_read_int(&melodyArray[i]);
                melodyDurations[i] = pgm_read_int(&melodyDurArray[i]);
                bassNotes[i] = pgm_read_int(&bassArray[i]);
                bassDurations[i] = pgm_read_int(&bassDurArray[i]);
            }
            songLength = length;
            playMusic = true;
            lastLyricsChangeTime = currentTime;
            noteDurationSignature = durationSignature;
            lyricsCount = lyricCount;
            lyricsDisplayCounter = 0;
            melodyNoteIndex = 0;
            bassNoteIndex = 0;
            lyricsIndex = 0;
}

/*
 * NOTES
 * From https://docs.arduino.cc/language-reference/en/functions/advanced-io/tone/
 * "If you want to play different pitches on multiple pins, you need to call noTone()
 *  on one pin before calling tone() on the next pin."
 */



