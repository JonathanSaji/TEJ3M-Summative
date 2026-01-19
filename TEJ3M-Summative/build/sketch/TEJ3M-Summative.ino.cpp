#include <Arduino.h>
#line 1 "C:\\Users\\Jonathan\\Documents\\TEJ3M-Summative\\TEJ3M-Summative\\TEJ3M-Summative.ino"
#include <pitches.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

//intialize variables
int speaker = 7;

//INVERTED LOGIC
int led1 = 6; //blue
int led2 = 5; //yellow 
int led3 = 4; //red  
int led4 = 3; //green
int led5 = 2; //white


String lyrics[] = {"Happy Birthday","To You","Mr Roller"};

#line 19 "C:\\Users\\Jonathan\\Documents\\TEJ3M-Summative\\TEJ3M-Summative\\TEJ3M-Summative.ino"
void setup();
#line 38 "C:\\Users\\Jonathan\\Documents\\TEJ3M-Summative\\TEJ3M-Summative\\TEJ3M-Summative.ino"
void loop();
#line 51 "C:\\Users\\Jonathan\\Documents\\TEJ3M-Summative\\TEJ3M-Summative\\TEJ3M-Summative.ino"
void printSentence(LiquidCrystal_I2C lcd,String sentence);
#line 19 "C:\\Users\\Jonathan\\Documents\\TEJ3M-Summative\\TEJ3M-Summative\\TEJ3M-Summative.ino"
void setup() {
    Serial.begin(9600);

    for(int i = 2; i <= 7; i++){
        if(i == 7){
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
    tone(speaker,NOTE_A4);

    for(int i = 0; i <= 2; i++){
        printSentence(lcd,lyrics[i]);
    }
    
    



}

void printSentence(LiquidCrystal_I2C lcd,String sentence){
  lcd.setCursor(0,1);
  lcd.print(sentence);
  delay(500);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(sentence);
  delay(10);
}


