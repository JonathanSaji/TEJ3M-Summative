#include <Arduino.h>
#line 1 "C:\\Users\\Jonathan\\Documents\\TEJ3M-Summative\\TEJ3M-Summative\\TEJ3M-Summative.ino"
#include <pitches.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

//Buzzer and Speaker Pins (NORMAL LOGIC)
int buzzer = 7;
int speaker = 8;

//INVERTED LOGIC
int led1 = 6; //blue
int led2 = 5; //yellow 
int led3 = 4; //red  
int led4 = 3; //green
int led5 = 2; //white

//millis() workaround for lcd lyrics
const int lyricsCount = 8;
int currentIndex = 0;
unsigned long lastChangeTime = 0;
int displayLength[] = {1000,1000,1000,1000,1500,1000,1000,1500}; //in milliseconds
int displayCounter = 0;
int displayInterval = displayLength[0]; //default start 1 second
int LCDlevel = 1;

//millis() workaround for buzzer notes
unsigned long noteEndTime = 0;
bool noteActive = false;
int currentNote = 0;


//workouround tone command
unsigned long lastBuzzerToggle = 0;
unsigned long buzzerHalfPeriod = 0;
bool buzzerState = false;


String lyrics[lyricsCount] = {"Happy Birthday","To You","Happy Birthday","To You","Happy Birthday","Dear JAYDEN","Happy Birthday","To You!"};
const int songLength = 25;
int melody[songLength] = {NOTE_C4,NOTE_C4,NOTE_D4,NOTE_C4,NOTE_F4,NOTE_E4,NOTE_C4,NOTE_C4,NOTE_D4,NOTE_C4,
                        NOTE_G4, NOTE_F4,NOTE_C4,NOTE_C4,NOTE_C5,NOTE_A4,NOTE_F4,NOTE_E4,NOTE_D4,NOTE_AS4,
                        NOTE_AS4,NOTE_A4,NOTE_F4,NOTE_G4,NOTE_F4};
int noteDurations[songLength] = {3,4,2,2,2,1,3,4,2,2,2,1,3,4,2,2,2,2,2,4,4,2,2,2,1};
int signature = 1500;
#line 45 "C:\\Users\\Jonathan\\Documents\\TEJ3M-Summative\\TEJ3M-Summative\\TEJ3M-Summative.ino"
void setup();
#line 64 "C:\\Users\\Jonathan\\Documents\\TEJ3M-Summative\\TEJ3M-Summative\\TEJ3M-Summative.ino"
void loop();
#line 45 "C:\\Users\\Jonathan\\Documents\\TEJ3M-Summative\\TEJ3M-Summative\\TEJ3M-Summative.ino"
void setup() {
    Serial.begin(9600);

    for(int i = 2; i <= 8; i++){
        if(i == 7 || i == 8){
            pinMode(i,OUTPUT);
        }
        else{
             pinMode(i,OUTPUT);
             digitalWrite(i,HIGH);
        }
    }

    //Intiallizing LCD
    lcd.init();
    lcd.backlight();
    lcd.clear();
}   

void loop() {
    unsigned long currentTime = millis();
  // Check if it's time to show the next message
  if (currentTime - lastChangeTime >= displayInterval && currentIndex < lyricsCount - 1) {
    
    // Loop back to start when reaching the end
    if (currentIndex >= lyricsCount) {
      currentIndex = 0;
    }
    
    // Clear and display the new message
    lcd.clear();
    if(LCDlevel == 1){
        lcd.setCursor(0, 1);
        lcd.print(lyrics[currentIndex]);
        LCDlevel--;
    }
    else if(LCDlevel == 0){
        lcd.setCursor(0, 0);
        lcd.print(lyrics[currentIndex]);
        lcd.setCursor(0, 1);
        //if it gets to the last lyric, loop back to the first
        if(currentIndex == lyricsCount - 1){
            lcd.print(lyrics[0]);
        }
        else{
            lcd.print(lyrics[currentIndex + 1]);
        }
        LCDlevel++;
        currentIndex++;
    }
    displayInterval = displayLength[displayCounter++];
    if(displayCounter >= lyricsCount-1){
        displayCounter = 0;
    }
    lastChangeTime = currentTime;
  }

    // Buzzer note handling
    if (currentNote < songLength) {
        if (!noteActive) {
            // Start new note
            switch(melody[currentNote]){
                case NOTE_C4:
                case NOTE_C5:
                    digitalWrite(led1,LOW);
                    break;
                case NOTE_D4:
                    digitalWrite(led2,LOW);
                    break;
                case NOTE_F4:
                    digitalWrite(led3,LOW);
                    break;
                case NOTE_G4:
                case NOTE_E4:
                    digitalWrite(led4,LOW);
                    break;
                case NOTE_A4:
                case NOTE_AS4:
                    digitalWrite(led5,LOW);
                    break;
                default:
                    break;
            }
            // Calculate half period for buzzer toggle (in microseconds)
            buzzerHalfPeriod = 1000000 / (melody[currentNote] * 2);
            lastBuzzerToggle = micros();
            buzzerState = false;
            digitalWrite(buzzer, LOW);
            digitalWrite(speaker, LOW);
            noteEndTime = currentTime + (signature / noteDurations[currentNote]);
            noteActive = true;
        }
        
        // Toggle buzzer pin for continuous tone
        unsigned long currentMicros = micros();
        if (currentMicros - lastBuzzerToggle >= buzzerHalfPeriod) {
            buzzerState = !buzzerState;
            digitalWrite(buzzer, buzzerState ? HIGH : LOW);
            digitalWrite(speaker, buzzerState ? HIGH : LOW);
          
            lastBuzzerToggle = currentMicros;
        }
        
        if (currentTime >= noteEndTime) {
            // Note time is done
            digitalWrite(led1,HIGH);
            digitalWrite(led2,HIGH);
            digitalWrite(led3,HIGH);
            digitalWrite(led4,HIGH);
            digitalWrite(led5,HIGH);
            digitalWrite(buzzer, LOW);
            digitalWrite(speaker, LOW);
            currentNote++;
            noteActive = false;
            
        }
    }
    else{
        delay(1000); //wait a second before restarting
        lcd.clear();
    }
}



/*

NOTES

From https://docs.arduino.cc/language-reference/en/functions/advanced-io/tone/ 
"If you want to play different pitches on multiple pins, you need to call noTone() 
  on one pin before calling tone() on the next pin."
  
  */




