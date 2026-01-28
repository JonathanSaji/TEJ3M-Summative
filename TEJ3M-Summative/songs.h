
#include "pitches.h"    

//HAPPY BIRTHDAY SONG DATA
const int happyBirthdayLength = 25;

const int happyBirthdayMelody[25] PROGMEM = {NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_F4, NOTE_E4,
                               NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_G4, NOTE_F4,
                               NOTE_C4, NOTE_C4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_E4, NOTE_D4,
                               NOTE_AS4, NOTE_AS4, NOTE_A4, NOTE_F4, NOTE_G4, NOTE_F4};

const int happyBirthdayDurations[25] PROGMEM = {3, 4, 2, 2, 2, 1, 3, 4, 2, 2, 2, 1, 3, 4, 2, 2, 2, 2, 2, 4, 4, 2, 2, 2, 1};

const int happyBirthdayBassDurations[25] PROGMEM = {4, 4, 4, 4, 4, 4, 4, 4, 4,
                                      4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
                                      4, 4, 4, 4, 4};

const int happyBirthdaybassNotes[25] PROGMEM = {REST, REST, NOTE_F4, REST, REST, NOTE_C4, REST, REST, NOTE_C3, REST,
                                  REST, NOTE_F4, REST, REST, NOTE_F4, REST, REST, NOTE_AS4, REST, REST,
                                  REST, NOTE_F4, REST, NOTE_C4, NOTE_F4};

const int happybirthdayNoteDurationSignature = 1500;

const int HBDlyricsCount = 8;
String HBDlyrics [HBDlyricsCount] = {"Happy Birthday", "To You", "Happy Birthday", "To You", "Happy Birthday", "Dear JAYDEN", "Happy Birthday", "To You!"};
const int happybirthdayLyricsDuration[HBDlyricsCount] PROGMEM = {1000,1000,1000,1000,1500,1000,1000,1500};
// END HAPPY BIRTHDAY SONG DATA


// NEVER GONNA GIVE YOU UP SONG DATA
// Melody for "Never Gonna Give You Up" --CREATED BY AI--
const int Snlength = 46;
const int Snnotes[] PROGMEM = {
  NOTE_G4, NOTE_A4, NOTE_G4, NOTE_E4,
  NOTE_G4, NOTE_A4, NOTE_G4, NOTE_E4,
  NOTE_D5, NOTE_D5, NOTE_B4,
  NOTE_C5, NOTE_C5, NOTE_G4,

  NOTE_A4, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_A4,
  NOTE_G4, NOTE_A4, NOTE_G4, NOTE_E4,
  NOTE_A4, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_A4,
  NOTE_G4, NOTE_A4, NOTE_G4, NOTE_E4,

  NOTE_D5, NOTE_D5, NOTE_F5, NOTE_D5, NOTE_B4,
  NOTE_C5, NOTE_E5,
  NOTE_C5, NOTE_G4, NOTE_E4,
  NOTE_G4, NOTE_F4, NOTE_D4,
  NOTE_C4
};

const int SnNoteDurationSignature = 2500;
const int SnLyricsCount = 14;
String SnLyrics[SnLyricsCount] = { "Silent night", "holy night",
"All is calm", "all is bright","Round yon Virgin", "Mother and Child",
"Holy Infant so", "tender and mild","Sleep'n heavenly", "peace","Sleep'n heavenly", "peace"};



const int SnLyricsDuration[SnLyricsCount] PROGMEM = {1000,1000,1000,2000,2500,2000,2000,2000,2000,1000,2000,1000,1000,1000};
// Bass line for new song - complements "Never Gonna Give You Up"

//Given by AI
const int SnBassNotes[] PROGMEM = {
  // Silent night, holy night (C Major)
  NOTE_C3, NOTE_C3, NOTE_C3, NOTE_C3,
  NOTE_C3, NOTE_C3, NOTE_C3, NOTE_C3,
  
  // All is calm (G Major)
  NOTE_G3, NOTE_G3, NOTE_G3,
  // All is bright (C Major)
  NOTE_C3, NOTE_C3, NOTE_C3,

  // Round yon Virgin... (F Major)
  NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3,
  // Mother and Child (C Major)
  NOTE_C3, NOTE_C3, NOTE_C3, NOTE_C3,

  // Holy Infant so... (F Major)
  NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3,
  // Tender and mild (C Major)
  NOTE_C3, NOTE_C3, NOTE_C3, NOTE_C3,

  // Sleep in heavenly... (G Major)
  NOTE_G3, NOTE_G3, NOTE_G3, NOTE_G3, NOTE_G3,
  // Peace (C Major)
  NOTE_C3, NOTE_C3,

  // Sleep in heavenly (C Major walking down)
  NOTE_C3, NOTE_E3, NOTE_G3,
  // Peace (G Major Dominant)
  NOTE_G3, NOTE_G3, NOTE_G3,
  NOTE_C3
};

const int SnBassDurations[] PROGMEM = {
  2, 8, 2,
  2, 8, 2,

  2, 4, 2, 
  2, 4, 2, 

  2, 4,
  2, 8,
  2, 8, 2,

  2, 4, 
  2, 8,
  2, 8, 2,

  2, 4,
  2, 8,

  2,
  2,

  2, 4,
  2, 8,
  2,2, 2,
  4, 4, 4,
  4, 8, 4,
  2, 
};

const int SnDurations[] PROGMEM = {
  4, 8, 4, 2,
  4, 8, 4, 2,
  2, 4, 2,
  2, 4, 2,

  2, 4, 4, 8, 4,
  4, 8, 4, 2,
  2, 4, 4, 8, 4,
  4, 8, 4, 2,

  2, 4, 4, 8, 4,
  2, 2,
  4, 4, 4,
  4, 8, 4,
  2, 
};