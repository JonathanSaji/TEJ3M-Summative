# Arduino Polyphonic Jukebox 🎵

A multi-track Arduino music player capable of rendering **melody and bass lines simultaneously** using dual buzzers. The system features a synchronized LED light show and an LCD display that renders lyrics in real-time.

Designed with memory optimization in mind, this project utilizes `PROGMEM` to store musical data, allowing for complex arrangements on hardware with limited SRAM (like the Arduino Uno).

## 🌟 Key Features

* **Polyphonic Audio:** Plays distinct melody and bass lines using two separate passive buzzers.
* **Synchronized Visuals:**
* **LCD Display:** Scrolls lyrics in time with the music.
* **LED Light Show:** 5-channel LED array lights up corresponding to the pitch of the melody notes.


* **Non-Blocking Architecture:** Uses `millis()` and `micros()` for timing, ensuring smooth playback without the use of `delay()`.
* **Memory Optimized:** Stores large song arrays (notes, durations, lyrics) in flash memory (`PROGMEM`) to prevent SRAM overflow.
* **Interactive Menu:** Select songs dynamically via the Serial Monitor.

## 🛠️ Hardware Requirements

* 1x Arduino (Uno, Nano, or Mega)
* 2x Passive Buzzers (one for Melody, one for Bass)
* 1x LCD Display (16x2) with I2C Backpack
* 5x LEDs (Blue, Yellow, Red, Green, White)
* 5x Resistors (220Ω or 330Ω for LEDs)
* Jumper Wires & Breadboard

## 🔌 Pin Configuration

| Component | Pin / Connection | Description |
| --- | --- | --- |
| **Melody Buzzer** | Digital 7 | Main vocal track |
| **Bass Buzzer** | Digital 8 | Bass/Rhythm track |
| **LCD Display** | SDA / SCL | I2C Data lines |
| **LED White** | Digital 2 | High Notes (A/A#) |
| **LED Green** | Digital 3 | Mid-High Notes (G/E) |
| **LED Red** | Digital 4 | Mid Notes (F) |
| **LED Yellow** | Digital 5 | Mid-Low Notes (D) |
| **LED Blue** | Digital 6 | Low Notes (C) |

## 📦 Installation & Setup

1. **Dependencies:** Ensure you have the following libraries installed in your Arduino IDE:
* `LiquidCrystal_I2C`


2. **File Structure:**
* `main.ino`: The primary logic file (setup, loop, timing).
* `songs.h`: Contains the song data arrays (notes, durations, lyrics).
* `pitches.h`: Standard definitions for musical note frequencies.


3. **Upload:**
* Connect your Arduino via USB.
* Select the correct **Board** and **Port** in the Arduino IDE.
* Click **Upload**.


4. **Usage:**
* Open the **Serial Monitor** (set baud rate to `9600`).
* The menu will appear on the LCD.
* Type `1` and press Enter to play **Happy Birthday**.
* Type `2` and press Enter to play **Silent Night**.



## 🎼 Included Songs

The project currently includes data for the following arrangements:

1. **Happy Birthday**
* *Features:* Classic melody with a simple rhythmic bass line.
* *Lyrics:* Full verse synchronized.


2. **Silent Night**
* *Features:* Full multi-track arrangement. The bass line follows a I-IV-V chord progression (C-F-G) to complement the melody.
* *Lyrics:* Two verses included.



## 📝 Technical Details

### Memory Management (`PROGMEM`)

To accommodate long arrays of notes and lyrics without crashing the Arduino's dynamic memory (SRAM), this project uses the `avr/pgmspace.h` library.

```cpp
// Example of storing data in Flash memory
const int happyBirthdayMelody[] PROGMEM = { NOTE_C4, NOTE_C4, ... };

```

Data is retrieved during playback using `pgm_read_word()`, ensuring the application remains stable even with extensive song libraries.

### Dual-Buzzer Timing

Standard Arduino `tone()` functions cannot play two notes simultaneously on different pins. This project bypasses that limitation by implementing a custom square-wave generator using `micros()`:

* **Melody Loop:** Toggles Pin 7 High/Low based on the note frequency.
* **Bass Loop:** Toggles Pin 9 High/Low based on the bass frequency.
* **Sequencer:** Updates the current note index based on `millis()` duration.

## 🤝 Contributing

Feel free to fork this repository and add your own songs! To add a song, you simply need to:

1. Define note and duration arrays in `songs.h`.
2. Add the lyrics array.
3. Update the switch case in the main loop to handle the new selection.

## 📄 License

This project is open-source and available under the [MIT License](https://www.google.com/search?q=LICENSE).
