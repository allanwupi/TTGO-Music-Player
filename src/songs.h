#ifndef SONGS_H
#define SONGS_H
#include "pitches.h"

#define NUM_SONGS 6
#define NUM_TRACKS 9
#define NUM_CHANNELS 2

// LEDC channels
#define TREBLE 1
#define BASS 2
#define LEDC_FREQUENCY 20000
#define LEDC_RESOLUTION 16

extern const char *SONG_DESCRIPTIONS[NUM_SONGS+1];

typedef struct {
    unsigned int pitch; // 0 for NO PITCH
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
    int minFreq; // Used for scaling TFT, converts to index into TONE_INDEX
    int maxFreq; // Used for scaling TFT, converts to index into TONE_INDEX
    bool converted; // Notes must be converted before playing
} Track;

typedef struct {
    const char *name; // Song name (27 characters maximum)
    Track *tracks[NUM_CHANNELS]; // Array of track pointers (converted)
    int channels[NUM_CHANNELS]; // Array of LEDC channels
    int numChannels; // Number of channels used
    int beat; // Millisecond duration of shortest note
    int bar; // Number of shortest note durations in 1 bar
    int numBars; // Length of the song in bars
    int lo; // Freqency index
    int hi; // Frequency index
} Song;

extern int DEFAULT_CHANNELS[NUM_CHANNELS];

extern Track Megalovania;
extern Track MegalovaniaBass;
extern Track TheLegend1;
extern Track TheLegendBass1;
extern Track TheLegend2;
extern Track TheLegendBass2;
extern Track TheLegend3;
extern Track TheLegendBass3;
extern Track FreedomMotif;

extern Song MEGALOVANIA;
//extern Song THE_LEGEND;

extern Track *Tracks[NUM_TRACKS];

#endif