#ifndef SONGS_H
#define SONGS_H
#include "pitches.h"

#define NUM_PLAYABLE_SONGS 3
#define NUM_MENU_OPTIONS 6
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

#define NOTE_NAME_LEN 3 // e.g. "Bb4"
#define NUM_NOTES_IN_SCALE 12 // Chromatic scale

extern const char *MENU[NUM_MENU_OPTIONS+1];

extern const char *C_SCALE[NUM_NOTES_IN_SCALE];

typedef struct {
    int pitch; // 0 indicates no note (REST), use PITCH enum
    unsigned int time; // TODO: implement special behaviour for 0?
} Note;

typedef struct {
    const char *name; // Song name (27 characters maximum)
    Note *notes; // Array of notes {pitch, name, duration}
    int size; // Size of note array
    int beat; // Base note duration in milliseconds (shortest note)
    int bar; // Number of shortest notes that make up 1 bar
    const char *scale[NUM_NOTES_IN_SCALE]; // Array of 12 strings
    // Remaining members are set during conversion: do NOT initialise
    int numBars; // Length of the song in bars
    int minFreq; // Used to draw lines in note visualiser
    int maxFreq; // Used to draw lines in note visualiser
    int lo; // Array index into TONE_INDEX
    int hi; // Array index into TONE_INDEX
    bool converted; // Notes must be converted before playing
} Track;

typedef struct {
    const char *name; // Song name (27 characters maximum)
    Track *tracks[MAX_CHANNELS]; // Array of pointers to converted tracks
    int size; // Number of channels actually used
    int channels[MAX_CHANNELS]; // Array of LEDC channels
    int colours[MAX_CHANNELS]; // Array of TFT colours
    const char *scale[NUM_NOTES_IN_SCALE]; // Array of 12 strings
} MultiTrack;

extern MultiTrack MEGALOVANIA;
extern MultiTrack LEGEND1;
extern MultiTrack LEGEND2;
extern MultiTrack LEGEND3;
extern MultiTrack FREEDOM_MOTIF;

extern Track *Tracks[NUM_TRACKS];

extern Track Megalovania_T;
extern Track Megalovania_B;
extern Track TheLegend1_T;
extern Track TheLegend1_B;
extern Track TheLegend2_T;
extern Track TheLegend2_B;
extern Track TheLegend3_T;
extern Track TheLegend3_B;
extern Track FreedomMotif;

extern Note megalovania_t[];
extern Note megalovania_b[];
extern Note legend1_t[];
extern Note legend1_b[];
extern Note legend2_t[];
extern Note legend2_b[];
extern Note legend3_t[];
extern Note legend3_b[];
extern Note freedomMotif[];

#endif