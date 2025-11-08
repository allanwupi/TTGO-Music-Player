// TTGO MUSIC PLAYER v3.3
// By Allan Wu
// Updated: 21 October 2025

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <Preferences.h>
#include "songs.h"
#include "pitches.h"

const char *PROGRAM_NAME = "TTGO MUSIC PLAYER v3.3";
const char *AUTHOR_DETAILS = " by Allan Wu (08/11/2025)";
Preferences menuPrefs;
TFT_eSPI TFT = TFT_eSPI();
TFT_eSPI *tft;

int screenOrientation;
int chosenSong;
int UP_BUTTON;
int DOWN_BUTTON;
int SCREEN_LENGTH = 320;
int SCREEN_WIDTH = 170;
int HEADER_WIDTH = 20;

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
void convertTrack(Track *usong, TFT_eSPI *tft);
void displayTrackInfo(Track *song, TFT_eSPI *tft);
unsigned long playMonoTrack(Track song, TFT_eSPI *tft, int barsToDisplay = 1, unsigned long prevElapsed = 0);
unsigned long playMultiTrack(MultiTrack *m, TFT_eSPI *tft, int barsToDisplay = 1, unsigned long prevElapsed = 0);

MultiTrack MEGALOVANIA = {
    .name = "MEGALOVANIA - Toby Fox",
    .tracks = {&Megalovania_T, &Megalovania_B},
    .size = 2,
    .channels = {TREBLE, BASS},
    .colours = {TFT_CYAN, TFT_DARKCYAN}
};
MultiTrack LEGEND1 = {
    .name = "THE LEGEND - Toby Fox (1/3)",
    .tracks = {&TheLegend1_T, &TheLegend1_B},
    .size = 2,
    .channels = {TREBLE, BASS},
    .colours = {TFT_CYAN, TFT_DARKCYAN}
};
MultiTrack LEGEND2 = {
    .name = "THE LEGEND - Toby Fox (2/3)",
    .tracks = {&TheLegend2_T, &TheLegend2_B},
    .size = 2,
    .channels = {TREBLE, BASS},
    .colours = {TFT_CYAN, TFT_DARKCYAN}
};
MultiTrack LEGEND3 = {
    .name = "THE LEGEND - Toby Fox (3/3)",
    .tracks = {&TheLegend3_T, &TheLegend3_B},
    .size = 2,
    .channels = {TREBLE, BASS},
    .colours = {TFT_CYAN, TFT_DARKCYAN}
};

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
    for (int i = 0; i < NUM_TRACKS; i++) convertTrack(Tracks[i], tft); 
    userSelectSong(chosenSong, tft);
}

void loop()
{
    unsigned long running = 0;
    switch (chosenSong) {
        case (0):
            playMultiTrack(&MEGALOVANIA, tft);
            break;
        case (1):
            running = playMultiTrack(&LEGEND1, tft, 2, 0);
            running = playMultiTrack(&LEGEND2, tft, 2, running);
            running = playMultiTrack(&LEGEND3, tft, 2, running);
            break;
        case (2):
            playMonoTrack(FreedomMotif, tft, 4);
            break;
        case (5):
            for (int i = 0; i < NUM_TRACKS; i++) displayTrackInfo(Tracks[i], tft); // fall-through to refresh menu
        default:
            userSelectSong(chosenSong, tft);
    }
}

void userSelectSong(int defaultChoice, TFT_eSPI *tft) {
    int prevUp = 0, prevDown = 0, currUp = 1, currDown = 1;
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
        if (prevUp && !currUp) {
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
        } else if (prevDown && !currDown) {
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

void convertTrack(Track *usong, TFT_eSPI *tft) {
    int currFreq;
    int minFreq = DS8;
    int maxFreq = NOTE_B0;
    int time = 0; // Convert note durations to absolute end time
    for (int i = 0; i < usong->size; i++) {
        if (!usong->converted) {
            time += usong->notes[i].time;
            usong->notes[i].time = time;
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
    usong->minFreq = (Pitch)minFreq;
    usong->maxFreq = (Pitch)maxFreq;
    usong->lo = minN;
    usong->hi = maxN;
}

void displayTrackInfo(Track *song, TFT_eSPI *tft) {
    int songDuration = song->beat * song->numBars * song->bar;
    int minutes = songDuration/60000, seconds = (songDuration/1000)%60;
    tft->setCursor(0,0);
    if (HEADER_WIDTH > 20) {
        tft->printf("%s\n[%d] (%dm%02ds)\n\n", song->name, song->size, minutes, seconds);
        tft->printf("T0=%dms bars=%dx%d\nlo=%dHz hi=%dHz [%02d-%02d]\n\n",
            song->beat, song->numBars, song->bar, song->minFreq, song->maxFreq, song->lo, song->hi);
    } else {
        tft->printf("%s [%d] (%dm%02ds)\n\n", song->name, song->size, minutes, seconds);
        tft->printf("T0=%dms bars=%dx%d lo=%dHz hi=%dHz [%02d-%02d]\n\n",
            song->beat, song->numBars, song->bar, song->minFreq, song->maxFreq, song->lo, song->hi);
    }
    for (int k = 0; k < song->bar; k++) {
        tft->printf("%02d:{%4d,%3s,%3d}  ", k, song->notes[k].pitch, song->notes[k].name, song->notes[k].time);
        if (HEADER_WIDTH > 20 || k % 2 == 1) tft->printf("\n");
    }
    int prevUp = 0, prevDown = 0, currUp = 1, currDown = 1;
    while (true) {
        currUp = !digitalRead(UP_BUTTON);
        currDown = !digitalRead(DOWN_BUTTON);
        if (prevUp && !currUp || prevDown && !currDown) break;
        prevUp = currUp;
        prevDown = currDown;
        delay(100);
    }
    tft->fillScreen(BG_COLOUR);
}

unsigned long playMonoTrack(Track song, TFT_eSPI *tft, int barsToDisplay, unsigned long prevElapsed) {
    int freq = 0, n = 1;
    const int minN = song.lo;
    const int maxN = song.hi;
    const int T0 = song.beat;
    const int divisions = song.bar * barsToDisplay;
    const int dx = ((SCREEN_LENGTH < T_DISPLAY_COLS) ? 160 : 320)/divisions;
    const int x0 = (HEADER_WIDTH > 20) ? 5 : 0;
    const int dy = (SCREEN_WIDTH-HEADER_WIDTH)/(maxN-minN);
    const char *noteName = song.notes[0].name;
    tft->setTextColor(HEADER_COLOUR, BG_COLOUR);
    tft->setCursor(HEADER_DATUM,HEADER_DATUM);
    tft->printf("%d:%02d  --/--  ---  ", prevElapsed/60000, (prevElapsed/1000)%60);
    if (HEADER_WIDTH > 20) tft->printf("\n\n %.27s", song.name);
    else tft->printf("%s", song.name);
    tft->drawFastHLine(0, HEADER_WIDTH, SCREEN_LENGTH, HEADER_COLOUR);
    int now = 0, next = 0, bars = 0, i = 0, j = 0;
    int stop = song.notes[song.size-1].time;
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
            int duration = (i > 0) ? (song.notes[i].time - song.notes[i-1].time) : song.notes[i].time;
            freq = song.notes[i].pitch;
            ledcWriteTone(TREBLE, freq);
            if (freq > 0) {
                for (n = minN ; n <= maxN ; n++) if (freq == TONE_INDEX[n]) break;
                tft->drawFastHLine(x0+(now%divisions)*dx, max(HEADER_WIDTH+2, SCREEN_WIDTH-1-dy*(n-minN)), dx*duration-1, HI_COLOUR);
                noteName = song.notes[i].name;
                tft->setCursor(HEADER_DATUM,HEADER_DATUM);
                tft->printf("%d:%02d  %2d/%-2d  %-3s", minutes, seconds, bars, song.numBars, noteName);
            }
            next = song.notes[i++].time;
        }
        if (millis() - prevTick >= T0) {
            movedBar = false, busy = false;
            now++;
            prevTick = millis();
        }
    }
    return millis() - startTime + prevElapsed;
}

unsigned long playMultiTrack(MultiTrack *m, TFT_eSPI *tft, int barsToDisplay, unsigned long prevElapsed) {
    int hi = 1, lo = NUM_FREQS-1;
    const int NUM_CHANNELS = m->size;
    for (int k = 0; k < NUM_CHANNELS; k++) {
        if (m->tracks[k]->lo < lo) lo = m->tracks[k]->lo;
        if (m->tracks[k]->hi > hi) hi = m->tracks[k]->hi;
    }
    const int T0 = m->tracks[0]->beat, bar = m->tracks[0]->bar;
    const int divisions = bar * barsToDisplay;
    const int dx = ((SCREEN_LENGTH < T_DISPLAY_COLS) ? 160 : 320)/divisions;
    const int x0 = (HEADER_WIDTH > 20) ? 5 : 0;
    const int dy = (SCREEN_WIDTH-HEADER_WIDTH)/(hi - lo);
    const char *melodyNote = m->tracks[0]->notes[0].name;
    const char *bassNote = m->tracks[1]->notes[0].name;
    // Print header information
    tft->setTextColor(HEADER_COLOUR, BG_COLOUR);
    tft->setCursor(HEADER_DATUM,HEADER_DATUM);
    tft->printf("%d:%02d  --/--  ---.---  ", prevElapsed/60000, (prevElapsed/1000)%60);
    if (HEADER_WIDTH > 20) tft->printf("\n\n %.27s", m->name);
    else tft->printf("%s", m->name);
    tft->drawFastHLine(0, HEADER_WIDTH, SCREEN_LENGTH, HEADER_COLOUR);
    // Set up required variables for track playback
    bool movedBar = false;
    bool busy[NUM_CHANNELS] = {0, 0};
    int freq[NUM_CHANNELS] = {0, 0};
    int ptrs[NUM_CHANNELS] = {0, 0};
    int next[NUM_CHANNELS] = {0, 0};
    int now = 0, bars = 0, n = 1;
    const int stop = m->tracks[0]->notes[m->tracks[0]->size-1].time;
    int minutes = prevElapsed/60000, seconds = (prevElapsed/1000)%60, prevSeconds = -1;
    unsigned long prevTick = millis(), startTime = millis();
    while (now < stop) {
        minutes = (millis() - startTime + prevElapsed) / 60000;
        seconds = ((millis() - startTime + prevElapsed) / 1000) % 60;
        if (seconds != prevSeconds) {
            tft->setCursor(HEADER_DATUM,HEADER_DATUM);
            tft->printf("%d:%02d", minutes, seconds);
            prevSeconds = seconds;
        }
        if (now % bar == 0 && !movedBar) {
            if (now % divisions == 0) tft->fillRect(0, HEADER_WIDTH+1, SCREEN_LENGTH, SCREEN_WIDTH-HEADER_WIDTH-1, BG_COLOUR); 
            bars++;
            tft->setCursor(HEADER_DATUM,HEADER_DATUM);
            tft->printf("%d:%02d  %2d/%-2d", minutes, seconds, bars, m->tracks[0]->numBars);
            movedBar = true;
        }
        for (int k = 0; k < NUM_CHANNELS; k++) {
            if (next[k] == now && !busy[k]) {
                int i = ptrs[k];
                busy[k] = true;
                int delta = (i > 0) ? (m->tracks[k]->notes[i].time - m->tracks[k]->notes[i-1].time) : m->tracks[k]->notes[i].time;
                freq[k] = m->tracks[k]->notes[i].pitch;
                ledcWriteTone(m->channels[k], freq[k]);
                if (freq[k] > 0) {
                    for (n = lo ; n <= hi ; n++) if (freq[k] == TONE_INDEX[n]) break;
                    tft->drawFastHLine(x0+(now%divisions)*dx, max(HEADER_WIDTH+2, SCREEN_WIDTH-1-dy*(n-lo)), dx*delta-1, m->colours[k]);
                    if (k == 0) melodyNote = m->tracks[k]->notes[i].name;
                    else if (k == 1) bassNote = m->tracks[k]->notes[i].name;
                    tft->setCursor(HEADER_DATUM,HEADER_DATUM);
                    tft->printf("%d:%02d  %2d/%-2d  %-3s.%-3s", minutes, seconds, bars, m->tracks[k]->numBars, melodyNote, bassNote);
                }
                next[k] = m->tracks[k]->notes[i].time;
                ptrs[k]++;
            }
        }
        if (millis() - prevTick >= T0) {
            movedBar = false;
            for (int k = 0; k < NUM_CHANNELS; k++) busy[k] = 0;
            now++;
            prevTick = millis();
        }
    }
    ledcWriteTone(TREBLE, 0);
    ledcWriteTone(BASS, 0);
    return millis() - startTime + prevElapsed;
}