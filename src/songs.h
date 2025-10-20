#ifndef SONGS_H
#define SONGS_H
#include "pitches.h"

// Note: we can only fit 5 songs onto the screen at once
#define NUM_SONGS 5
#define NUM_TRACKS 7

extern const char *SONG_DESCRIPTIONS[NUM_SONGS+1];

typedef struct {
    unsigned int pitch; // 0 for NO PITCH
    const char *noteName;
    unsigned int noteLength;
} Note;

typedef struct {
    const char *name; // Song name (30 characters maximum)
    Note *notes; // Array of notes {freqIndex, noteLength}
    int numNotes; // Size of note array
    int period; // Millisecond duration of shortest note
    int bar; // Number of shortest note durations in 1 bar
    int numBars; // Length of the song in bars
    int minFreq; // Only used for scaling TFT
    int maxFreq; // Only used for scaling TFT
} Song_t;

extern Note megalovania[];
extern Note legend0[];
extern Note legend1[];
extern Note legend2[];
extern Note legend3[];
extern Note freedomMotif[];

extern Song_t Megalovania;
extern Song_t TheLegend0;
extern Song_t TheLegend1;
extern Song_t TheLegend2;
extern Song_t TheLegend3;
extern Song_t FreedomMotif;

extern Song_t MegalovaniaBass;
extern Song_t *SongPtrs[NUM_TRACKS];

#endif