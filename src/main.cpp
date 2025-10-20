#include <Arduino.h>
#include <TFT_eSPI.h>
#include "songs.h"
#include "pitches.h"

const char *PROGRAM_NAME = "TTGO MUSIC PLAYER v1.0 ";

#define TREBLE 1
#define BASS 2
#define TREBLE_BUZZER 1
#define BASS_BUZZER 3

#define LEFT_BUTTON 0
#define RIGHT_BUTTON 14

#define MENU_X_DATUM    10
#define MENU_Y_DATUM    30
#define BUFFER_CHARS    50

#define LEFT_BUTTON     0
#define RIGHT_BUTTON    14

#define PRIMARY_TEXT_COLOUR     TFT_WHITE
#define HIGH_EMPHASIS_COLOUR    TFT_GOLD
#define LOW_EMPHASIS_COLOUR     0x2965
#define BACKGROUND_COLOUR       TFT_BLACK

TFT_eSPI TFT = TFT_eSPI();
TFT_eSPI *tft;

int screenOrientation = 3;
int chosenSong = 0;

void userSelectSong(TFT_eSPI *tft);
void playSong(Song_t song, int barsToDisplay, TFT_eSPI *tft);

void setup()
{
    pinMode(TREBLE_BUZZER, OUTPUT);
    pinMode(BASS_BUZZER, OUTPUT);
    pinMode(15, OUTPUT);
    digitalWrite(15, HIGH);
    tft = &TFT;
    tft->init();
    tft->setTextColor(PRIMARY_TEXT_COLOUR, BACKGROUND_COLOUR);
    tft->setTextFont(0);
    tft->setTextSize(2);
    tft->setRotation(screenOrientation);
    tft->fillScreen(BACKGROUND_COLOUR);
	ledcSetup(TREBLE, 10000, 16);
	ledcSetup(BASS, 10000, 16);
	ledcAttachPin(TREBLE_BUZZER, TREBLE);
	ledcAttachPin(BASS_BUZZER, BASS);
    userSelectSong(tft);
    //Serial.begin(115200);
}

void loop()
{
    switch (chosenSong) {
        case (0):
            playSong(Megalovania, 1, tft);
            break;
        case (1):
            playSong(TheLegend0, 4, tft);
            playSong(TheLegend1, 4, tft);
            playSong(TheLegend2, 2, tft);
            playSong(TheLegend3, 4, tft);
            break;
        case (2):
            playSong(FreedomMotif, 2, tft);
           break;
        default:
            // TODO: Put an error message here?
            userSelectSong(tft);
    }
}

void userSelectSong(TFT_eSPI *tft) {
    int prevLeft = 0, prevRight = 0;
    int currLeft = 0, currRight = 0;
    int prevChoice = -1;
    static int currChoice = 0;
    bool startPlayer = false;

    tft->setCursor(0, 0);
    tft->printf(PROGRAM_NAME);
    tft->drawFastHLine(0, 20, 320, TFT_WHITE);

    while (!startPlayer) {
        currLeft = !digitalRead(LEFT_BUTTON);
        currRight = !digitalRead(RIGHT_BUTTON);
        if (prevLeft && !currLeft) {
            if (currChoice == NUM_SONGS) {
                screenOrientation = (screenOrientation > 1) ? 1 : 3;
                tft->setRotation(screenOrientation);
            }
            startPlayer = true;
        } else if (prevRight && !currRight) {
            currChoice = (currChoice + 1) % (NUM_SONGS+1);
        }

        if (prevChoice != currChoice) {
            tft->setTextColor(LOW_EMPHASIS_COLOUR, BACKGROUND_COLOUR);

            for (int i = 0; i < NUM_SONGS+1; i++) {
            tft->drawString(SONG_DESCRIPTIONS[i], MENU_X_DATUM, MENU_Y_DATUM+23*i);
            }

            tft->setTextColor(HIGH_EMPHASIS_COLOUR, BACKGROUND_COLOUR);
            tft->drawString(SONG_DESCRIPTIONS[currChoice], MENU_X_DATUM, MENU_Y_DATUM+23*currChoice);
        }

        tft->setTextColor(PRIMARY_TEXT_COLOUR, BACKGROUND_COLOUR);
        prevChoice = currChoice;

        prevLeft = currLeft;
        prevRight = currRight;
    }
    tft->fillScreen(BACKGROUND_COLOUR);
    chosenSong = currChoice;
}

void playSong(Song_t song, int barsToDisplay, TFT_eSPI *tft)
{
    int freq, n, minN, maxN, T;
    for (n = 1 ; n <= NUM_FREQS ; n++) {
        if (song.minFreq == TONE_INDEX[n]) minN = n;
        if (song.maxFreq == TONE_INDEX[n]) {
            maxN = n;
            break;
        }
    }
    const int songLength = song.numNotes;
    const int dx = 320/barsToDisplay/song.bar;
    const int dy = 150/(maxN - minN);
    const int T0 = song.period;
    const char *noteName;
    int periods = 0;
    unsigned long startTime;
    int act_delay;
    tft->setCursor(0, 0);
    tft->printf("000/%-3d: --- %.13s", songLength, song.name);
    tft->drawFastHLine(0, 20, 320, TFT_WHITE);
    for (int i = 0, k = 0; i < songLength; i++) {
        startTime = millis();
        freq = song.notes[i].pitch;
        noteName = song.notes[i].noteName;
        T = song.notes[i].noteLength * T0;
        tft->setCursor(0, 0);
        if (periods % (barsToDisplay*song.bar) == 0) {
            tft->fillRect(0, 21, 320, 149, TFT_BLACK); 
            periods = 0;
            k = !k;
        }
        if (freq) {
            ledcWriteTone(TREBLE, freq);
            //ledcWriteTone(BASS, 294);
            for (n = minN ; n <= maxN ; n++) if (freq == TONE_INDEX[n]) break;
            tft->drawFastHLine(periods*dx, 169-dy*(n-minN), dx*(T/T0)-2, HIGH_EMPHASIS_COLOUR);
            if (song.overflow)
                tft->printf("%3d/%-3d: %-3s %.13s", i+1, songLength, noteName, (k) ? song.name : song.overflow);
            else
                tft->printf("%3d/%-3d: %-3s %.13s", i+1, songLength, noteName, song.name);
        } else {
            ledcWriteTone(TREBLE, 0);
            //ledcWriteTone(BASS, 294);
            tft->drawFastHLine(periods*dx, 169, dx*(T/T0)-2, LOW_EMPHASIS_COLOUR);
            tft->printf("%3d/%-3d: ", i+1, songLength);
        }
        periods += song.notes[i].noteLength;
        act_delay = T - (millis() - startTime);
        if (act_delay > 0) delay(act_delay);
    }
    ledcWriteTone(TREBLE, 0);
    //ledcWriteTone(BASS, 0);
}