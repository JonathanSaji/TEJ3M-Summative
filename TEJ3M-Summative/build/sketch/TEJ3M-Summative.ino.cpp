#include <Arduino.h>
#line 1 "C:\\Users\\Jonathan\\Documents\\TEJ3M-Summative\\TEJ3M-Summative\\TEJ3M-Summative.ino"
#include <pitches.h>

//intialize variables
int speaker = 7;

//INVERTED LOGIC
int led1 = 6; //blue
int led2 = 5; //yellow 
int led3 = 4; //red  
int led4 = 3; //green
int led5 = 2; //white


#line 14 "C:\\Users\\Jonathan\\Documents\\TEJ3M-Summative\\TEJ3M-Summative\\TEJ3M-Summative.ino"
void setup();
#line 28 "C:\\Users\\Jonathan\\Documents\\TEJ3M-Summative\\TEJ3M-Summative\\TEJ3M-Summative.ino"
void loop();
#line 14 "C:\\Users\\Jonathan\\Documents\\TEJ3M-Summative\\TEJ3M-Summative\\TEJ3M-Summative.ino"
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
}   

void loop() {
    tone(speaker,NOTE_A4);
    
}

