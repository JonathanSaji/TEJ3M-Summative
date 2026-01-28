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
 * - Synchronized LED visual effects.
 * - I2C LCD lyric display with non-blocking staggered scrolling.
 */

#include <pitches.h>
#include <LiquidCrystal_I2C.h>
#include <avr/pgmspace.h>
#include "songs.h" // Contains note arrays, durations, and lyrics

// --- Hardware Configuration ---
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Buzzer Pins
#define melodyBuzzer 7
#define bassBuzzer   8

// LED Pins (Active Low Configuration: LOW = ON)
#define ledBlue 6
#define ledYellow 5
#define ledRed   4
#define ledGreen 3
#define ledWhite 2

// --- Global Timing Variables ---
unsigned long currentTime;

// LCD & Lyrics Timing
int lyricsCount = 0;
int lyricsIndex = 0;
unsigned long lastLyricsChangeTime = 0;
int lyricsDisplayDuration[100] = {0}; // Buffer for lyric timings
int lyricsDisplayCounter = 0;
int currentDisplayInterval = 0; 
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
int melodyNotes[50];
int melodyDurations[50];
int bassDurations[50];
int bassNotes[50];
int noteDurationSignature = 0;

// Serial Menu State
bool choicemade = false;
bool choice1made = false;

void setup() {
  Serial.begin(9600);

  // Initialize Pins
  for (int i = 2; i <= 8; i++) {
    pinMode(i, OUTPUT);
    if (i < 7) {
      digitalWrite(i, HIGH); // Initialize LEDs to OFF
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

    if (choice1made && input != "") {
      if (input.length() > 10) {
        lcd.clear();
        lcd.print("Name too long");
        delay(1000);
        enterName();
      } else {
        HBDlyrics[5] = "Dear " + input + "!";
        setVariablesToPlay(
          happyBirthdayLength, happyBirthdayMelody, happyBirthdayDurations, 
          happyBirthdaybassNotes, happyBirthdayBassDurations, 
          happybirthdayNoteDurationSignature, HBDlyricsCount, HBDlyrics, 
          happybirthdayLyricsDuration
        );
      }
    }
    else if (input == "1" && !choicemade) {
      choice1made = true;
      choicemade = true;
      enterName();
    }
    else if (input == "2" && !choicemade) {
      setVariablesToPlay(
        Snlength, Snnotes, SnDurations, SnBassNotes, 
        SnBassDurations, SnNoteDurationSignature, SnLyricsCount, SnLyrics, 
        SnLyricsDuration
      );
      choicemade = true;
    }
    else {
      lcd.clear();
      lcd.print("Invalid Option");
      delay(1000);
      lcd.clear();
      playMenu();
    }
    input = ""; 
  }

  // --- Music Playback & Scroller Logic ---
  if (playMusic) {
    
    // --- SCROLLER BLOCK 1: THE TIMING GATE ---
    // This allows the screen to update only when the specific duration for 
    // the current lyric has passed, without stopping the music (non-blocking).
    if (currentTime - lastLyricsChangeTime >= currentDisplayInterval && lyricsIndex < lyricsCount) {
      
      if (lyricsIndex >= lyricsCount) {
        lyricsIndex = 0;
      }

      lcd.clear(); 
      
      // --- SCROLLER BLOCK 2: THE ROLLING ROW LOGIC ---
      // This creates a staggered transition between Row 0 and Row 1.
      
      // State A: Print current lyric to the bottom row (Setup for next line)
      if (lyricsRow == 1) {
        lcd.setCursor(0, 1);
        lcd.print(lyrics[lyricsIndex]);
        lyricsRow--; // Next time, move this lyric to the top
      } 
      // State B: Move current lyric to top row and preview the NEXT lyric on bottom
      else if (lyricsRow == 0) {
        lcd.setCursor(0, 0);
        lcd.print(lyrics[lyricsIndex]);
        
        lcd.setCursor(0, 1);
        // Preview next line logic
        if (lyricsIndex == lyricsCount - 1) {
          lcd.print(lyrics[0]); // Loop back to start for preview
        } else {
          lcd.print(lyrics[lyricsIndex + 1]); // Show what's coming next
        }
        lyricsRow++;
        lyricsIndex++; // Move the song position forward
      }

      // --- SCROLLER BLOCK 3: DYNAMIC INTERVAL UPDATING ---
      // Instead of a fixed scroll speed, we pull the specific duration 
      // for how long THIS lyric should stay on screen from our timing array.
      if (lyricsDisplayCounter >= lyricsCount) {
        lyricsDisplayCounter = 0;
      }
      
      currentDisplayInterval = lyricsDisplayDuration[lyricsDisplayCounter++];
      lastLyricsChangeTime = currentTime;
    }

    // --- Note Management Logic ---
    if (melodyNoteIndex < songLength) {
      if (!melodyNoteActive) {
        if (lastActiveLED != -1) {
          digitalWrite(lastActiveLED, HIGH);
          delay(10);//slight delay to notice note change
        }

        // LED Visualizer Logic
        switch (melodyNotes[melodyNoteIndex]) {
          case NOTE_C4: case NOTE_C5: digitalWrite(ledBlue, LOW); lastActiveLED = ledBlue; break;
          case NOTE_D4: case NOTE_D5: digitalWrite(ledYellow, LOW); lastActiveLED = ledYellow; break;
          case NOTE_F4: case NOTE_B4: digitalWrite(ledRed, LOW); lastActiveLED = ledRed; break;
          case NOTE_G4: case NOTE_E4: case NOTE_E5: digitalWrite(ledGreen, LOW); lastActiveLED = ledGreen; break;
          case NOTE_A4: case NOTE_AS4: case NOTE_F5: digitalWrite(ledWhite, LOW); lastActiveLED = ledWhite; break;
          default: lastActiveLED = -1; break;
        }

        melodyHalfPeriod = 1000000 / (melodyNotes[melodyNoteIndex] * 2);
        bassHalfPeriod   = 1000000 / (bassNotes[melodyNoteIndex] * 2);
        
        lastMelodyToggleTime = micros();
        lastBassToggleTime   = micros();

        melodyBuzzerState = false;
        bassBuzzerState = false;
        
        digitalWrite(melodyBuzzer, LOW);
        digitalWrite(bassBuzzer, LOW);

        melodyNoteEndTime = currentTime + (noteDurationSignature / pgm_read_word(&melodyDurations[melodyNoteIndex]));
        melodyNoteActive = true;
        bassNoteEndTime = currentTime + (noteDurationSignature / pgm_read_word(&bassDurations[melodyNoteIndex]));
        bassNoteActive = true;
      }

      // Manual PWM Waveform Generation
      currentMicrosTime = micros();
      playMelody(melodyHalfPeriod, melodyBuzzer);
      playBass(bassHalfPeriod, bassBuzzer);

      if (currentTime >= melodyNoteEndTime) {
        if (lastActiveLED != -1) digitalWrite(lastActiveLED, HIGH);
        digitalWrite(melodyBuzzer, LOW);
        digitalWrite(bassBuzzer, LOW);
        
        melodyNoteIndex++;
        bassNoteIndex++;
        melodyNoteActive = false;
        bassNoteActive = false;
      }
    } else {
      lcd.clear();
      playMusic = false;
      playMenu();
      choicemade = false;
      choice1made = false;
    }
  }
}

// --- Helper Functions ---

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
  lcd.print("1 = HBD 2 = SN");
  lcd.setCursor(0, 1);
  lcd.print("Select Song:");
}

void enterName() {
  lcd.clear();
  lcd.print("Enter Name: ");
  lcd.setCursor(0, 1);
  lcd.print("MAX 10 CHAR");
}

void setVariablesToPlay(const int length, const int melodyArray[], const int melodyDurArray[], 
                        const int bassArray[], const int bassDurArray[], const int durationSignature, 
                        const int lyricCount, const String lyricArray[], const int lyricDurArray[]) {
  
  for (int i = 0; i < lyricCount; i++) {
    lyrics[i] = lyricArray[i];
    lyricsDisplayDuration[i] = pgm_read_word(&lyricDurArray[i]);
  }

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
  currentDisplayInterval = 0;
}