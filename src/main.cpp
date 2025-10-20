#include <Arduino.h>
#include <TFT_eSPI.h>
#include "songs.h"
#include "pitches.h"

const char *PROGRAM_NAME = " TTGO MUSIC PLAYER v3.0 ";

#define TREBLE 1
#define BASS 2
#define TREBLE_BUZZER 1
#define BASS_BUZZER 2

#define MENU_X_DATUM    10
#define MENU_Y_DATUM    30
#define BUFFER_CHARS    50

#define PRIMARY_TEXT_COLOUR     TFT_WHITE
#define HIGH_EMPHASIS_COLOUR    TFT_GOLD
#define LOW_EMPHASIS_COLOUR     0x2965
#define BACKGROUND_COLOUR       TFT_BLACK

TFT_eSPI TFT = TFT_eSPI();
TFT_eSPI *tft;

int UP_BUTTON = 0;
int DOWN_BUTTON = 14;

int screenOrientation = 3;
int chosenSong = 0;

void userSelectSong(TFT_eSPI *tft);
void convertToAbsoluteTime(Song_t song);
unsigned long playSingleTrack(Song_t song, TFT_eSPI *tft, int barsToDisplay = 1, unsigned long elapsed = 0);
unsigned long playTracks(Song_t song, Song_t bass, TFT_eSPI *tft, int barsToDisplay = 1, unsigned long elapsed = 0);

void setup()
{
    //Serial.begin(115200);
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
	ledcSetup(TREBLE, 20000, 16);
	ledcSetup(BASS, 20000, 16);
	ledcAttachPin(TREBLE_BUZZER, TREBLE);
	ledcAttachPin(BASS_BUZZER, BASS);
    userSelectSong(tft);
    tft->setTextSize(1);
    for (int i = 0; i < NUM_TRACKS; i++)
        convertToAbsoluteTime(*SongPtrs[i]);
}

void loop()
{
    unsigned long runTime = 0;
    switch (chosenSong) {
        case (0):
            playTracks(Megalovania, MegalovaniaBass, tft);
            break;
        case (1):
            runTime = playSingleTrack(TheLegend1, tft, 2, 0);
            runTime = playSingleTrack(TheLegend2, tft, 2, runTime);
            runTime = playSingleTrack(TheLegend3, tft, 2, runTime);
            break;
        case (2):
            playSingleTrack(FreedomMotif, tft, 4);
           break;
        default:
            // TODO: Put an error message here?
            userSelectSong(tft);
    }
}

void userSelectSong(TFT_eSPI *tft) {
    int prevUp = 0, prevDown = 0;
    int currUp = 0, currDown = 0;
    int prevChoice = -1;
    static int currChoice = 0;
    bool startPlayer = false;

    tft->setCursor(0, 0);
    tft->printf(PROGRAM_NAME);
    tft->drawFastHLine(0, 20, 320, TFT_WHITE);

    while (!startPlayer) {
        currUp = !digitalRead(UP_BUTTON);
        currDown = !digitalRead(DOWN_BUTTON);
        if (prevUp && !currUp) {
            if (currChoice == NUM_SONGS) {
                int temp;
                temp = UP_BUTTON;
                UP_BUTTON = DOWN_BUTTON;
                DOWN_BUTTON = temp;
                screenOrientation = (screenOrientation > 1) ? 1 : 3;
                tft->setRotation(screenOrientation);
            }
            startPlayer = true;
        } else if (prevDown && !currDown) {
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

        prevUp = currUp;
        prevDown = currDown;
    }
    tft->fillScreen(BACKGROUND_COLOUR);
    chosenSong = currChoice;
}

void convertToAbsoluteTime(Song_t song) {
    int Time = 0; // Absolute time steps
    for (int i = 0; i < song.numNotes; i++) {
        Time += song.notes[i].noteLength;
        song.notes[i].noteLength = Time;
    }
}

unsigned long playSingleTrack(Song_t song, TFT_eSPI *tft, int barsToDisplay, unsigned long elapsed) {
    int freq1 = 0, freq2 = 0;
    int n, minN, maxN;
    for (n = 1 ; n <= NUM_FREQS ; n++) {
        if (song.minFreq == TONE_INDEX[n]) minN = n;
        if (song.maxFreq == TONE_INDEX[n]) {
            maxN = n;
            break;
        }
    }
    const int T0 = song.period;
    const int divisions = song.bar * barsToDisplay;
    const int dx = 320/divisions;
    const int dy = 150/(maxN - minN);
    const char *noteName = song.notes[0].noteName;
    tft->setCursor(7,7);
    tft->printf("%d:%02d  00/00  ---  %s", elapsed/60000, (elapsed/1000)%60, song.name);
    tft->drawFastHLine(0, 20, 320, TFT_WHITE);
    int now = 0, next = 0, bars = 0, i = 0, j = 0;
    bool finalNote = false;
    bool finished = false;
    bool movedBar = false, printed = false;
    int duration, reqDelay;
    int minutes = elapsed/60000, seconds = (elapsed/1000)%60, prevSeconds = -1;
    unsigned long startTime = millis();
    unsigned long playTime = millis();
    while (!finished) {
        minutes = (millis() - playTime + elapsed) / 60000;
        seconds = ((millis() - playTime + elapsed) / 1000) % 60;
        if (seconds != prevSeconds) {
            tft->setCursor(7,7);
            tft->printf("%d:%02d", minutes, seconds);
            prevSeconds = seconds;
        }
        if (now % song.bar == 0 && !movedBar) {
            if (now % divisions == 0) tft->fillRect(0, 21, 320, 149, TFT_BLACK); 
            bars++;
            tft->setCursor(7,7);
            tft->printf("%d:%02d  %2d/%-2d", minutes, seconds, bars, song.numBars);
            movedBar = true;
        }
        if (now == next && !printed) {
            printed = true;
            if (!finalNote) {
                duration = (i > 0) ? (song.notes[i].noteLength - song.notes[i-1].noteLength) : song.notes[i].noteLength;
                freq1 = song.notes[i].pitch;
                if (freq1) {
                    ledcWriteTone(TREBLE, freq1);
                    for (n = minN ; n <= maxN ; n++) if (freq1 == TONE_INDEX[n]) break;
                    tft->drawFastHLine((now%divisions)*dx, 169-dy*(n-minN), dx*duration-2, HIGH_EMPHASIS_COLOUR);
                    noteName = song.notes[i].noteName;
                    tft->setCursor(7,7);
                    tft->printf("%d:%02d  %2d/%-2d  %-3s", minutes, seconds, bars, song.numBars, noteName);
                }
                else ledcWriteTone(TREBLE, 0);
                next = song.notes[i].noteLength;
                if (i == song.numNotes-1) finalNote = true;
                else i++;
            } else {
                ledcWriteTone(TREBLE, 0);
                finished = true;
            }
        }
        if (millis() - startTime > T0) {
            movedBar = false;
            printed = false;
            now++;
            startTime = millis();
        }
    }
    return minutes*60000+seconds*1000;
}

unsigned long playTracks(Song_t song, Song_t bass, TFT_eSPI *tft, int barsToDisplay, unsigned long elapsed) {
    int freq1 = 0, freq2 = 0;
    int n, minN, maxN;
    for (n = 1 ; n <= NUM_FREQS ; n++) {
        if (bass.minFreq == TONE_INDEX[n]) minN = n;
        if (song.maxFreq == TONE_INDEX[n]) {
            maxN = n;
            break;
        }
    }
    const int T0 = song.period;
    const int divisions = song.bar * barsToDisplay;
    const int dx = 320/divisions;
    const int dy = 150/(maxN - minN);
    const char *trebleNoteName = song.notes[0].noteName;
    const char *bassNoteName = bass.notes[0].noteName;
    tft->setCursor(7,7);
    tft->printf("%d:%02d  00/00  ---.---  %s", elapsed/60000, (elapsed/1000)%60, song.name);
    tft->drawFastHLine(0, 20, 320, TFT_WHITE);
    int now = 0, bars = 0, i = 0, j = 0;
    int nextTreble = 0, nextBass = 0;
    bool lastTrebleNote = false, lastBassNote = false;
    bool finishedTreble = false, finishedBass = false;
    bool movedBar = false, wroteTreble = false, wroteBass = false;
    int duration, reqDelay;
    int minutes = elapsed/60000, seconds = (elapsed/1000)%60, prevSeconds = -1;
    unsigned long startTime = millis();
    unsigned long playTime = millis();
    while (!finishedTreble || !finishedBass) {
        minutes = (millis() - playTime) / 60000;
        seconds = ((millis() - playTime) / 1000) % 60;
        if (seconds != prevSeconds) {
            tft->setCursor(7,7);
            tft->printf("%d:%02d", minutes, seconds);
            prevSeconds = seconds;
        }
        if (now % song.bar == 0 && !movedBar) {
            if (now % divisions == 0) tft->fillRect(0, 21, 320, 149, TFT_BLACK); 
            bars++;
            tft->setCursor(7,7);
            tft->printf("%d:%02d  %2d/%-2d", minutes, seconds, bars, song.numBars);
            movedBar = true;
        }
        if (now == nextTreble && !wroteTreble) {
            wroteTreble = true;
            if (!lastTrebleNote) {
                duration = (i > 0) ? (song.notes[i].noteLength - song.notes[i-1].noteLength) : song.notes[i].noteLength;
                freq1 = song.notes[i].pitch;
                if (freq1) {
                    ledcWriteTone(TREBLE, freq1);
                    for (n = minN ; n <= maxN ; n++) if (freq1 == TONE_INDEX[n]) break;
                    tft->drawFastHLine((now%divisions)*dx, 169-dy*(n-minN), dx*duration-2, HIGH_EMPHASIS_COLOUR);
                    trebleNoteName = song.notes[i].noteName;
                    tft->setCursor(7,7);
                    tft->printf("%d:%02d  %2d/%-2d  %-3s.%-3s", minutes, seconds, bars, song.numBars, trebleNoteName, bassNoteName);
                }
                else ledcWriteTone(TREBLE, 0);
                nextTreble = song.notes[i].noteLength;
                if (i == song.numNotes-1) lastTrebleNote = true;
                else i++;
            } else {
                ledcWriteTone(TREBLE, 0);
                finishedTreble = true;
            }
        }
        if (now == nextBass && !wroteBass) {
            wroteBass = true;
            if (!lastBassNote) {
                duration = (j > 0) ? (bass.notes[j].noteLength - bass.notes[j-1].noteLength) : bass.notes[j].noteLength;
                freq2 = bass.notes[j].pitch;
                if (freq2) {
                    ledcWriteTone(BASS, freq2);
                    for (n = minN ; n <= maxN ; n++) if (freq2 == TONE_INDEX[n]) break;
                    tft->drawFastHLine((now%divisions)*dx, 169-dy*(n-minN), dx*duration-2, TFT_DARKGREY);
                    bassNoteName = bass.notes[j].noteName;
                    tft->setCursor(7,7);
                    tft->printf("%d:%02d  %2d/%-2d  %-3s.%-3s", minutes, seconds, bars, song.numBars, trebleNoteName, bassNoteName);
                }
                else ledcWriteTone(BASS, 0);
                nextBass = bass.notes[j].noteLength;
                if (j == bass.numNotes-1) lastBassNote = true;
                else j++;
            } else {
                ledcWriteTone(BASS, 0);
                finishedBass = true;
            }
        }
        if (millis() - startTime > T0) {
            movedBar = false;
            wroteTreble = false;
            wroteBass = false;
            now++;
            startTime = millis();
        }
    }
    return minutes*60000+seconds*1000;
}