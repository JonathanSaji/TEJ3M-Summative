/*
 * Project: Karaoke Boogie Nights!
 * Author:  Jonathan Saji
 * Date:    Jan 28th, 2026
 *
 * Description: 
 * An Arduino-based karaoke system capable of playing "Happy Birthday" and "Silent Night".
 * Features:
 * - Serial input for song selection and lyric personalization.
 * - Dual-buzzer output (Melody + Bass) using manual PWM for polyphony.
 * - synchronized LED visual effects.
 * - I2C LCD lyric display.
 */

#include <pitches.h>
#include <LiquidCrystal_I2C.h>
#include <avr/pgmspace.h>
#include "songs.h" // Contains note arrays, durations, and lyrics

// --- Hardware Configuration ---
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Buzzer Pins
const int melodyBuzzer = 7;
const int bassBuzzer   = 8;

// LED Pins (Active Low Configuration: LOW = ON)
const int ledBlue  = 6;
const int ledYellow= 5;
const int ledRed   = 4;
const int ledGreen = 3;
const int ledWhite = 2;

// --- Global Timing Variables ---
unsigned long currentTime;

// LCD & Lyrics Timing
int lyricsCount = 0;
int lyricsIndex = 0;
unsigned long lastLyricsChangeTime = 0;
int lyricsDisplayDuration[100] = {0}; // Buffer for lyric timings
int lyricsDisplayCounter = 0;
int currentDisplayInterval = 0; // Will be set during song setup
int lyricsRow = 1;

// String buffer for current song lyrics
String lyrics[20] = {"PAUSE"};

// Melody Timing
unsigned long melodyNoteEndTime = 0;
bool melodyNoteActive = false;
int melodyNoteIndex = 0;

// Bass Timing
unsigned long bassNoteEndTime = 0;
bool bassNoteActive = false;
int bassNoteIndex = 0;

// Manual PWM / Waveform Generation Variables
unsigned long lastMelodyToggleTime = 0;
unsigned long lastBassToggleTime = 0;
unsigned long melodyHalfPeriod = 0;
unsigned long bassHalfPeriod = 0;
bool melodyBuzzerState = false;
bool bassBuzzerState = false;
long currentMicrosTime = 0;
long currentBassMicrosTime = 0;

// LED State Tracking
int lastActiveLED = -1;

// Playback State
bool playMusic = false;
int songLength = 0;

// Buffers for Song Data (RAM)
// Note: Actual data is read from PROGMEM (pgm_read_word) into these arrays
int melodyNotes[50]     = {REST};
int melodyDurations[50] = {REST};
int bassDurations[50]   = {REST};
int bassNotes[50]       = {REST};
int noteDurationSignature = 0;

// Serial Menu State
bool choicemade = false;
bool choice1made = false;

void setup() {
  Serial.begin(9600);

  // Initialize Pins
  for (int i = 2; i <= 8; i++) {
    pinMode(i, OUTPUT);
    // Initialize LEDs to OFF (Active Low -> HIGH is OFF)
    if (i < 7) {
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

  // --- Serial Input Handling ---
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    // 1. Handling Name Input for Happy Birthday
    if (choice1made && input != "") {
      if (input.length() > 10) {
        lcd.clear();
        lcd.print("Name too long");
        delay(1000);
        enterName();
      } else {
        // Insert name into lyrics array (Assumes HBDlyrics is defined in songs.h)
        HBDlyrics[5] = "Dear " + input + "!";
        
        setVariablesToPlay(
          happyBirthdayLength, happyBirthdayMelody, happyBirthdayDurations, 
          happyBirthdaybassNotes, happyBirthdayBassDurations, 
          happybirthdayNoteDurationSignature, HBDlyricsCount, HBDlyrics, 
          happybirthdayLyricsDuration
        );
      }
    }
    // 2. Select Happy Birthday
    else if (input == "1" && !choicemade) {
      choice1made = true;
      choicemade = true;
      enterName();
    }
    // 3. Select Silent Night
    else if (input == "2" && !choicemade) {
      setVariablesToPlay(
        Snlength, Snnotes, SnDurations, SnBassNotes, 
        SnBassDurations, SnNoteDurationSignature, SnLyricsCount, SnLyrics, 
        SnLyricsDuration
      );
      choicemade = true;
    }
    // 4. Invalid Input
    else {
      lcd.clear();
      lcd.print("Invalid Option");
      delay(1000);
      lcd.clear();
      playMenu();
    }
    input = ""; // Clear buffer
  }

  // --- Music Playback Logic ---
  if (playMusic) {
    
    // 1. LCD Update Logic
    if (currentTime - lastLyricsChangeTime >= currentDisplayInterval && lyricsIndex < lyricsCount) {
      
      // Loop lyrics if song continues
      if (lyricsIndex >= lyricsCount) {
        lyricsIndex = 0;
      }

      lcd.clear();
      
      // Handle rolling display (Row 0 vs Row 1)
      if (lyricsRow == 1) {
        lcd.setCursor(0, 1);
        lcd.print(lyrics[lyricsIndex]);
        lyricsRow--;
      } else if (lyricsRow == 0) {
        lcd.setCursor(0, 0);
        lcd.print(lyrics[lyricsIndex]);
        lcd.setCursor(0, 1);
        
        // Preview next line
        if (lyricsIndex == lyricsCount - 1) {
          lcd.print(lyrics[0]);
        } else {
          lcd.print(lyrics[lyricsIndex + 1]);
        }
        lyricsRow++;
        lyricsIndex++;
      }

      // Reset counter if bounds exceeded
      if (lyricsDisplayCounter >= lyricsCount) {
        lyricsDisplayCounter = 0;
      }
      
      currentDisplayInterval = lyricsDisplayDuration[lyricsDisplayCounter++];
      lastLyricsChangeTime = currentTime;
    }

    // 2. Note Management Logic
    if (melodyNoteIndex < songLength) {
      if (!melodyNoteActive) {
        // Turn off previous LED
        if (lastActiveLED != -1) {
          digitalWrite(lastActiveLED, HIGH);
        }

        // Determine which LED to light based on pitch
        switch (melodyNotes[melodyNoteIndex]) {
          case NOTE_C4: case NOTE_C5:
            digitalWrite(ledBlue, LOW);
            lastActiveLED = ledBlue;
            break;
          case NOTE_D4: case NOTE_D5:
            digitalWrite(ledYellow, LOW);
            lastActiveLED = ledYellow;
            break;
          case NOTE_F4: case NOTE_B4:
            digitalWrite(ledRed, LOW);
            lastActiveLED = ledRed;
            break;
          case NOTE_G4: case NOTE_E4: case NOTE_E5:
            digitalWrite(ledGreen, LOW);
            lastActiveLED = ledGreen;
            break;
          case NOTE_A4: case NOTE_AS4: case NOTE_F5:
            digitalWrite(ledWhite, LOW);
            lastActiveLED = ledWhite;
            break;
          default:
            lastActiveLED = -1;
            break;
        }

        // Calculate Waveform Periods (in microseconds)
        // Formula: 1,000,000 / (Frequency * 2) = Half Period
        melodyHalfPeriod = 1000000 / (melodyNotes[melodyNoteIndex] * 2);
        bassHalfPeriod   = 1000000 / (bassNotes[melodyNoteIndex] * 2);
        
        lastMelodyToggleTime = micros();
        lastBassToggleTime   = micros();

        melodyBuzzerState = false;
        bassBuzzerState = false;
        
        digitalWrite(melodyBuzzer, LOW);
        digitalWrite(bassBuzzer, LOW);

        // Set note duration
        melodyNoteEndTime = currentTime + (noteDurationSignature / melodyDurations[melodyNoteIndex]);
        melodyNoteActive = true;

        bassNoteEndTime = currentTime + (noteDurationSignature / bassDurations[melodyNoteIndex]);
        bassNoteActive = true;
      }

      // Generate Sound (Bit-banging PWM)
      currentMicrosTime = micros();
      playMelody(melodyHalfPeriod, melodyBuzzer);
      playBass(bassHalfPeriod, bassBuzzer);

      // Check if note is finished
      if (currentTime >= melodyNoteEndTime) {
        if (lastActiveLED != -1) {
          digitalWrite(lastActiveLED, HIGH); // LED Off
        }
        digitalWrite(melodyBuzzer, LOW);
        digitalWrite(bassBuzzer, LOW);
        
        melodyNoteIndex++;
        bassNoteIndex++;
        
        melodyNoteActive = false;
        bassNoteActive = false;
      }
    } else {
      // Song finished
      lcd.clear();
      playMusic = false;
      playMenu();
      choicemade = false;
      choice1made = false;
    }
  }
}

// --- Helper Functions ---

/*
 * Manually toggles the Melody buzzer pin.
 * Required because tone() is blocking or uses the same timer,
 * preventing two simultaneous notes on different pins.
 */
void playMelody(long halfPeriod, int buzzerPin) {
  if (currentMicrosTime - lastMelodyToggleTime >= halfPeriod) {
    melodyBuzzerState = !melodyBuzzerState;
    digitalWrite(buzzerPin, melodyBuzzerState ? HIGH : LOW);
    lastMelodyToggleTime = currentMicrosTime;
  }
}

/*
 * Manually toggles the Bass buzzer pin.
 */
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
  lcd.print("1 = HBD 2 = SN");
}

void enterName() {
  lcd.clear();
  lcd.print("Enter Name: ");
  lcd.setCursor(0, 1);
  lcd.print("MAX 10 CHAR");
}

/*
 * Loads song data from PROGMEM (Flash memory) into RAM buffers.
 * Also resets state variables for playback.
 */
void setVariablesToPlay(const int length, const int melodyArray[], const int melodyDurArray[], 
                        const int bassArray[], const int bassDurArray[], const int durationSignature, 
                        const int lyricCount, const String lyricArray[], const int lyricDurArray[]) {
  
  // Load Lyrics
  for (int i = 0; i < lyricCount; i++) {
    lyrics[i] = lyricArray[i];
    lyricsDisplayDuration[i] = pgm_read_word(&lyricDurArray[i]);
  }

  // Load Note Data
  for (int i = 0; i < length; i++) {
    melodyNotes[i]     = pgm_read_word(&melodyArray[i]);
    melodyDurations[i] = pgm_read_word(&melodyDurArray[i]);
    bassNotes[i]       = pgm_read_word(&bassArray[i]);
    bassDurations[i]   = pgm_read_word(&bassDurArray[i]);
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
  
  // Initialize first interval
      currentDisplayInterval = 0;
}