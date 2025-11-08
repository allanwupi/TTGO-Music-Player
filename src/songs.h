#ifndef SONGS_H
#define SONGS_H
#include "pitches.h"

#define NUM_SONGS 6
#define NUM_TRACKS 9
#define MAX_CHANNELS 8

// Buzzer pins
#define TREBLE_BUZZER 1    // Output pin 1
#define BASS_BUZZER 2      // Output pin 2

// LEDC channels
#define TREBLE 1
#define BASS 2
#define LEDC_FREQUENCY 20000
#define LEDC_RESOLUTION 16

extern const char *SONG_DESCRIPTIONS[NUM_SONGS+1];

typedef struct {
    Pitch pitch; // 0 for NO PITCH
    const char *name; // 2-3 characters
    unsigned int time; // TODO: implement special behaviour for 0?
} Note;

typedef struct {
    const char *name; // Song name (27 characters maximum)
    Note *notes; // Array of notes {freqIndex, time}
    int size; // Size of note array
    int beat; // Millisecond duration of shortest note
    int bar; // Number of shortest note durations in 1 bar
    int numBars; // Length of the song in bars
    Pitch minFreq; // Used for scaling TFT, converts to index into TONE_INDEX
    Pitch maxFreq; // Used for scaling TFT, converts to index into TONE_INDEX
    int lo; // Frequency index
    int hi; // Frequency index
    bool converted; // Notes must be converted before playing
} Track;

typedef struct {
    const char *name; // Song name (27 characters maximum)
    Track *tracks[MAX_CHANNELS]; // Array of pointers to converted tracks
    int size; // Number of channels actually used
    int channels[MAX_CHANNELS]; // Array of LEDC channels
    int colours[MAX_CHANNELS]; // Array of TFT colours
} MultiTrack;

extern Note megalovania_t[];
extern Note megalovania_b[];
extern Note legend1_t[];
extern Note legend1_b[];
extern Note legend2_t[];
extern Note legend2_b[];
extern Note legend3_t[];
extern Note legend3_b[];
extern Note freedomMotif[];

extern Track Megalovania_T;
extern Track Megalovania_B;
extern Track TheLegend1_T;
extern Track TheLegend1_B;
extern Track TheLegend2_T;
extern Track TheLegend2_B;
extern Track TheLegend3_T;
extern Track TheLegend3_B;
extern Track FreedomMotif;

extern Track *Tracks[NUM_TRACKS];

#endif