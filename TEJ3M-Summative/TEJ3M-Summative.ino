#include <pitches.h>
int speaker = 7;

void setup() {
    pinMode(speaker,OUTPUT);
}

void loop() {
    tone(speaker,NOTE_A4);


}
