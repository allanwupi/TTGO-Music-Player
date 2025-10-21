// TTGO MUSIC PLAYER v3.3
// By Allan Wu
// Updated: 21 October 2025

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <Preferences.h>
#include "songs.h"
#include "pitches.h"

const char *PROGRAM_NAME = "TTGO MUSIC PLAYER v3.3";
const char *AUTHOR_DETAILS = " by Allan Wu (21/10/2025)";
Preferences menuPrefs;

int SCREEN_LENGTH = 320;
int SCREEN_WIDTH = 170;
int HEADER_WIDTH = 20;

int UP_BUTTON;
int DOWN_BUTTON;
int screenOrientation;
int chosenSong;

const int TREBLE = 1;           // LEDC channel 1
const int BASS = 2;             // LEDC channel 2
const int TREBLE_BUZZER = 1;    // Output pin 1
const int BASS_BUZZER = 2;      // Output pin 2

TFT_eSPI TFT = TFT_eSPI();
TFT_eSPI *tft;

// Probably shouldn't change these values
#define LEDC_FREQUENCY 20000
#define LEDC_RESOLUTION 16

#define T_DISPLAY_COLS 320
#define T_DISPLAY_ROWS 170
#define DEFAULT_ORIENTATION 3
#define DEFAULT_SELECTION 0

#define MENU_X_DATUM    10
#define HEADER_DATUM    7
#define MENU_SPACING    14
#define BUFFER_CHARS    50

#define TITLE_COLOUR        TFT_WHITE       // Program name
#define SELECTED_COLOUR     TFT_WHITE      	// Selected option
#define DESELECTED_COLOUR   0x2965          // Greyed-out option
#define HEADER_COLOUR       TFT_WHITE       // Top description
#define HI_COLOUR           TFT_CYAN        // Treble track
#define LO_COLOUR           TFT_DARKCYAN    // Bass track
#define BG_COLOUR           TFT_BLACK       // Background

void userSelectSong(int defaultChoice, TFT_eSPI *tft);
void convertTrack(Track *usong, TFT_eSPI *tft, bool printToDisplay = false);
unsigned long playSingleTrack(Track song, TFT_eSPI *tft, int barsToDisplay = 1, unsigned long prevElapsed = 0);
unsigned long playTracks(Track melody, Track bass, TFT_eSPI *tft, int barsToDisplay = 1, unsigned long prevElapsed = 0);

void setup()
{
    Serial.begin(115200);
    // Setup pins and LEDC channels for tone generation
    pinMode(TREBLE_BUZZER, OUTPUT);
    pinMode(BASS_BUZZER, OUTPUT);
    ledcSetup(TREBLE, LEDC_FREQUENCY, LEDC_RESOLUTION);
    ledcSetup(BASS, LEDC_FREQUENCY, LEDC_RESOLUTION);
    ledcAttachPin(TREBLE_BUZZER, TREBLE);
    ledcAttachPin(BASS_BUZZER, BASS);
    // Retrieve user preferences from memory
    menuPrefs.begin("menuPrefs", false);
    if (!menuPrefs.isKey("orientation")) menuPrefs.putInt("orientation", DEFAULT_ORIENTATION);
    if (!menuPrefs.isKey("selection")) menuPrefs.putInt("selection", DEFAULT_SELECTION);
    if (!menuPrefs.isKey("length")) menuPrefs.putInt("length", T_DISPLAY_COLS);
    if (!menuPrefs.isKey("width")) menuPrefs.putInt("width", T_DISPLAY_ROWS);
    if (!menuPrefs.isKey("header")) menuPrefs.putInt("header", 20);
    screenOrientation = menuPrefs.getInt("orientation");
    chosenSong = menuPrefs.getInt("selection");
    SCREEN_LENGTH = menuPrefs.getInt("length");
    SCREEN_WIDTH = menuPrefs.getInt("width");
    HEADER_WIDTH = menuPrefs.getInt("header");
    // Convert tracks to playable format and go to startup menu
    tft = &TFT;
    tft->init();
    for (int i = 0; i < NUM_TRACKS; i++) convertTrack(SongPtrs[i], tft); 
    userSelectSong(chosenSong, tft);
}

void loop()
{
    unsigned long totalTime = 0;
    switch (chosenSong) {
        case (0):
            playTracks(Megalovania, MegalovaniaBass, tft);
            break;
        case (1):
            totalTime = playTracks(TheLegend1, TheLegendBass1, tft, 2, 0);
            totalTime = playTracks(TheLegend2, TheLegendBass2, tft, 2, totalTime);
            totalTime = playTracks(TheLegend3, TheLegendBass3, tft, 2, totalTime);
            break;
        case (2):
            playSingleTrack(FreedomMotif, tft, 4);
            break;
        case (5):
            for (int i = 0; i < NUM_TRACKS; i++) convertTrack(SongPtrs[i], tft, true); // fall-through to refresh menu
        default:
            userSelectSong(chosenSong, tft);
    }
}

void userSelectSong(int defaultChoice, TFT_eSPI *tft) {
    int prevUp = 1, prevDown = 1, currUp = 0, currDown = 0;
    int prevChoice = -1;
    int currChoice = defaultChoice;
    bool startPlayer = false;
    UP_BUTTON = 14 * !(screenOrientation == 0 || screenOrientation == 3); // 0 if orientation 3
    DOWN_BUTTON = 14 * (screenOrientation == 0 || screenOrientation == 3); // 14 if orientation 3
    tft->setRotation(screenOrientation);
    tft->fillScreen(BG_COLOUR);
    tft->setTextColor(TITLE_COLOUR, BG_COLOUR);
    tft->setTextSize(1);
    tft->setCursor(MENU_X_DATUM,HEADER_DATUM);
    tft->printf(PROGRAM_NAME);
    if (HEADER_WIDTH > 20) tft->printf("\n\n ");
    tft->printf(AUTHOR_DETAILS);
    tft->drawFastHLine(0, HEADER_WIDTH, SCREEN_LENGTH, TITLE_COLOUR);
    while (!startPlayer) {
        currUp = !digitalRead(UP_BUTTON);
        currDown = !digitalRead(DOWN_BUTTON);
        if (!prevUp && currUp) {
            if (currChoice == NUM_SONGS) {
                int temp = SCREEN_LENGTH;
                SCREEN_LENGTH = SCREEN_WIDTH;
                SCREEN_WIDTH = temp;
                HEADER_WIDTH = (screenOrientation % 2) ? 40 : 20;
                screenOrientation = (screenOrientation+1) % 4;
                menuPrefs.putInt("orientation", screenOrientation);
                menuPrefs.putInt("length", SCREEN_LENGTH);
                menuPrefs.putInt("width", SCREEN_WIDTH);
                menuPrefs.putInt("header", HEADER_WIDTH);
            }
            startPlayer = true;
        } else if (!prevDown && currDown) {
            currChoice = (currChoice+1) % (NUM_SONGS+1);
        }
        if (prevChoice != currChoice) {
            tft->setTextColor(DESELECTED_COLOUR, BG_COLOUR);
            for (int i = 0; i < NUM_SONGS+1; i++) tft->drawString(SONG_DESCRIPTIONS[i], MENU_X_DATUM, HEADER_WIDTH+10+MENU_SPACING*i);
            tft->setTextColor(SELECTED_COLOUR, BG_COLOUR);
            tft->drawString(SONG_DESCRIPTIONS[currChoice], MENU_X_DATUM, HEADER_WIDTH+10+MENU_SPACING*currChoice);
            tft->setTextColor(TITLE_COLOUR, BG_COLOUR);
        }
        prevChoice = currChoice;
        prevUp = currUp;
        prevDown = currDown;
    }
    tft->fillScreen(BG_COLOUR);
    tft->setTextSize(1);
    chosenSong = currChoice;
    menuPrefs.putInt("selection", currChoice);
}

void convertTrack(Track *usong, TFT_eSPI *tft, bool printToDisplay) {
    int currFreq;
    int minFreq = DS8;
    int maxFreq = NOTE_B0;
    int time = 0; // Convert note durations to absolute end time
    for (int i = 0; i < usong->numNotes; i++) {
        if (!usong->converted) {
            time += usong->notes[i].noteLength;
            usong->notes[i].noteLength = time;
        }
        currFreq = usong->notes[i].pitch;
        if (currFreq && currFreq < minFreq) minFreq = currFreq;
        if (currFreq && currFreq > maxFreq) maxFreq = currFreq;
    }
    if (!usong->converted) {
        usong->numBars = time / (usong->bar);
        usong->converted = true;
    }
    int n, minN, maxN;
    for (n = 1; n <= NUM_FREQS; n++) {
        if (TONE_INDEX[n] == minFreq) minN = n;
        if (TONE_INDEX[n] == maxFreq) {
            maxN = n;
            break;
        }
    }
    usong->minFreq = minN;
    usong->maxFreq = maxN;
    int songDuration = usong->period * usong->numBars * usong->bar;
    int minutes = songDuration/60000, seconds = (songDuration/1000)%60;
    if (printToDisplay) {
        tft->setCursor(0,0);
        if (HEADER_WIDTH > 20) {
            tft->printf("%s\n[%d] (%dm%02ds)\n\n", usong->name, usong->numNotes, minutes, seconds);
            tft->printf("T0=%dms bars=%dx%d\nlo=%dHz hi=%dHz [%02d-%02d]\n\n",
                usong->period, usong->numBars, usong->bar, minFreq, maxFreq, usong->minFreq, usong->maxFreq);
        } else {
            tft->printf("%s [%d] (%dm%02ds)\n\n", usong->name, usong->numNotes, minutes, seconds);
            tft->printf("T0=%dms bars=%dx%d lo=%dHz hi=%dHz [%02d-%02d]\n\n",
                usong->period, usong->numBars, usong->bar, minFreq, maxFreq, usong->minFreq, usong->maxFreq);
        }
        for (int k = 0; k < usong->bar; k++) {
            tft->printf("%02d:{%4d,%3s,%3d}  ", k, usong->notes[k].pitch, usong->notes[k].noteName, usong->notes[k].noteLength);
            if (HEADER_WIDTH > 20 || k % 2 == 1) tft->printf("\n");
        }
        int prevUp = 1, prevDown = 1, currUp = 0, currDown = 0;
        while (true) {
            currUp = !digitalRead(UP_BUTTON);
            currDown = !digitalRead(DOWN_BUTTON);
            if (!prevUp && currUp || !prevDown && currDown) break;
            prevUp = currUp;
            prevDown = currDown;
            delay(100);
        }
        tft->fillScreen(BG_COLOUR);
    }
}

unsigned long playSingleTrack(Track song, TFT_eSPI *tft, int barsToDisplay, unsigned long prevElapsed) {
    int freq1 = 0, freq2 = 0, n = 1;
    const int minN = song.minFreq;
    const int maxN = song.maxFreq;
    const int T0 = song.period;
    const int divisions = song.bar * barsToDisplay;
    const int dx = ((SCREEN_LENGTH < T_DISPLAY_COLS) ? 160 : 320)/divisions;
    const int x0 = (HEADER_WIDTH > 20) ? 5 : 0;
    const int dy = (SCREEN_WIDTH-HEADER_WIDTH)/(maxN-minN);
    const char *noteName = song.notes[0].noteName;
    tft->setTextColor(HEADER_COLOUR, BG_COLOUR);
    tft->setCursor(HEADER_DATUM,HEADER_DATUM);
    tft->printf("%d:%02d  --/--  ---  ", prevElapsed/60000, (prevElapsed/1000)%60);
    if (HEADER_WIDTH > 20) tft->printf("\n\n");
    tft->printf(" %s", song.name);
    tft->drawFastHLine(0, HEADER_WIDTH, SCREEN_LENGTH, HEADER_COLOUR);
    int now = 0, next = 0, bars = 0, i = 0, j = 0;
    int stop = song.notes[song.numNotes-1].noteLength;
    bool movedBar = false, busy = false;
    int minutes = prevElapsed/60000, seconds = (prevElapsed/1000)%60, prevSeconds = -1;
    unsigned long prevTick = millis();
    unsigned long startTime = millis();
    while (now < stop) {
        minutes = (millis() - startTime + prevElapsed) / 60000;
        seconds = ((millis() - startTime + prevElapsed) / 1000) % 60;
        if (seconds != prevSeconds) {
            tft->setCursor(HEADER_DATUM,HEADER_DATUM);
            tft->printf("%d:%02d", minutes, seconds);
            prevSeconds = seconds;
        }
        if (now % song.bar == 0 && !movedBar) {
            if (now % divisions == 0) tft->fillRect(0, HEADER_WIDTH+1, SCREEN_LENGTH, SCREEN_WIDTH-HEADER_WIDTH-1, BG_COLOUR); 
            bars++;
            tft->setCursor(HEADER_DATUM,HEADER_DATUM);
            tft->printf("%d:%02d  %2d/%-2d", minutes, seconds, bars, song.numBars);
            movedBar = true;
        }
        if (now == next && !busy) {
            busy = true;
            int duration = (i > 0) ? (song.notes[i].noteLength - song.notes[i-1].noteLength) : song.notes[i].noteLength;
            freq1 = song.notes[i].pitch;
            if (freq1) {
                ledcWriteTone(TREBLE, freq1);
                for (n = minN ; n <= maxN ; n++) if (freq1 == TONE_INDEX[n]) break;
                tft->drawFastHLine(x0+(now%divisions)*dx, max(HEADER_WIDTH+2, SCREEN_WIDTH-1-dy*(n-minN)), dx*duration-1, HI_COLOUR);
                noteName = song.notes[i].noteName;
                tft->setCursor(HEADER_DATUM,HEADER_DATUM);
                tft->printf("%d:%02d  %2d/%-2d  %-3s", minutes, seconds, bars, song.numBars, noteName);
            } else {
                ledcWriteTone(TREBLE, 0);
            }
            next = song.notes[i++].noteLength;
        }
        if (millis() - prevTick >= T0) {
            movedBar = false, busy = false;
            now++;
            prevTick = millis();
        }
    }
    return millis() - startTime + prevElapsed;
}

unsigned long playTracks(Track melody, Track bass, TFT_eSPI *tft, int barsToDisplay, unsigned long prevElapsed) {
    int freq1 = 0, freq2 = 0, n = 1;
    const int minN = bass.minFreq;
    const int maxN = melody.maxFreq;
    const int T0 = melody.period;
    const int divisions = melody.bar * barsToDisplay;
    const int dx = ((SCREEN_LENGTH < T_DISPLAY_COLS) ? 160 : 320)/divisions;
    const int x0 = (HEADER_WIDTH > 20) ? 5 : 0;
    const int dy = (SCREEN_WIDTH-HEADER_WIDTH)/(maxN - minN);
    const char *trebleNoteName = melody.notes[0].noteName;
    const char *bassNoteName = bass.notes[0].noteName;
    tft->setTextColor(HEADER_COLOUR, BG_COLOUR);
    tft->setCursor(HEADER_DATUM,HEADER_DATUM);
    tft->printf("%d:%02d  --/--  ---.---  ", prevElapsed/60000, (prevElapsed/1000)%60);
    if (HEADER_WIDTH > 20) tft->printf("\n\n");
    tft->printf(" %s", melody.name);
    tft->drawFastHLine(0, HEADER_WIDTH, SCREEN_LENGTH, HEADER_COLOUR);
    int now = 0, bars = 0, i = 0, j = 0;
    int stop = melody.notes[melody.numNotes-1].noteLength;
    if (stop != bass.notes[bass.numNotes-1].noteLength) {
        Serial.printf("Error: melody and bass tracks have differing lengths\n");
        Serial.printf("Melody: %d, bass: %d\n", stop, bass.notes[bass.numNotes-1].noteLength);
        return 0;
    }
    int nextTreble = 0, nextBass = 0;
    bool movedBar = false, trebleBusy = false, bassBusy = false;
    int minutes = prevElapsed/60000, seconds = (prevElapsed/1000)%60, prevSeconds = -1;
    unsigned long prevTick = millis();
    unsigned long startTime = millis();
    while (now < stop) {
        minutes = (millis() - startTime + prevElapsed) / 60000;
        seconds = ((millis() - startTime + prevElapsed) / 1000) % 60;
        if (seconds != prevSeconds) {
            tft->setCursor(HEADER_DATUM,HEADER_DATUM);
            tft->printf("%d:%02d", minutes, seconds);
            prevSeconds = seconds;
        }
        if (now % melody.bar == 0 && !movedBar) {
            if (now % divisions == 0) tft->fillRect(0, HEADER_WIDTH+1, SCREEN_LENGTH, SCREEN_WIDTH-HEADER_WIDTH-1, BG_COLOUR); 
            bars++;
            tft->setCursor(HEADER_DATUM,HEADER_DATUM);
            tft->printf("%d:%02d  %2d/%-2d", minutes, seconds, bars, melody.numBars);
            movedBar = true;
        }
        if (nextTreble == now && !trebleBusy) {
            trebleBusy = true;
            int duration = (i > 0) ? (melody.notes[i].noteLength - melody.notes[i-1].noteLength) : melody.notes[i].noteLength;
            freq1 = melody.notes[i].pitch;
            if (freq1 > 0) {
                ledcWriteTone(TREBLE, freq1);
                for (n = minN ; n <= maxN ; n++) if (freq1 == TONE_INDEX[n]) break;
                tft->drawFastHLine(x0+(now%divisions)*dx, max(HEADER_WIDTH+2, SCREEN_WIDTH-1-dy*(n-minN)), dx*duration-1, HI_COLOUR);
                trebleNoteName = melody.notes[i].noteName;
                tft->setCursor(HEADER_DATUM,HEADER_DATUM);
                tft->printf("%d:%02d  %2d/%-2d  %-3s.%-3s", minutes, seconds, bars, melody.numBars, trebleNoteName, bassNoteName);
            } else {
                ledcWriteTone(TREBLE, 0);
            }
            nextTreble = melody.notes[i++].noteLength;
        }
        if (nextBass == now && !bassBusy) {
            bassBusy = true;
            int duration = (j > 0) ? (bass.notes[j].noteLength - bass.notes[j-1].noteLength) : bass.notes[j].noteLength;
            freq2 = bass.notes[j].pitch;
            if (freq2 > 0) {
                ledcWriteTone(BASS, freq2);
                for (n = minN ; n <= maxN ; n++) if (freq2 == TONE_INDEX[n]) break;
                tft->drawFastHLine(x0+(now%divisions)*dx, max(HEADER_WIDTH+2, SCREEN_WIDTH-1-dy*(n-minN)), dx*duration-1, LO_COLOUR);
                bassNoteName = bass.notes[j].noteName;
                tft->setCursor(HEADER_DATUM,HEADER_DATUM);
                tft->printf("%d:%02d  %2d/%-2d  %-3s.%-3s", minutes, seconds, bars, melody.numBars, trebleNoteName, bassNoteName);
            } else {
                ledcWriteTone(BASS, 0);
            }
            nextBass = bass.notes[j++].noteLength;
        }
        if (millis() - prevTick >= T0) {
            movedBar = false, trebleBusy = false, bassBusy = false;
            now++;
            prevTick = millis();
        }
    }
    ledcWriteTone(TREBLE, 0);
    ledcWriteTone(BASS, 0);
    return millis() - startTime + prevElapsed;
}