#ifndef SONGS_H
#define SONGS_H
#include "pitches.h"

#define NUM_SONGS 6
#define NUM_TRACKS 9

extern const char *SONG_DESCRIPTIONS[NUM_SONGS+1];

typedef struct {
    unsigned int pitch; // 0 for NO PITCH
    const char *noteName; // 2-3 characters
    unsigned int noteLength; // TODO: implement special behaviour for 0
} Note;

typedef struct {
    const char *name; // Song name (27 characters maximum)
    Note *notes; // Array of notes {freqIndex, noteLength}
    int numNotes; // Size of note array
    int period; // Millisecond duration of shortest note
    int bar; // Number of shortest note durations in 1 bar
    int numBars; // Length of the song in bars
    int minFreq; // Used for scaling TFT, converts to index into TONE_INDEX
    int maxFreq; // Used for scaling TFT, converts to index into TONE_INDEX
    bool converted; // Notes must be converted before playing
} Track;

extern Track Megalovania;
extern Track MegalovaniaBass;
extern Track TheLegend1;
extern Track TheLegendBass1;
extern Track TheLegend2;
extern Track TheLegendBass2;
extern Track TheLegend3;
extern Track TheLegendBass3;
extern Track FreedomMotif;

extern Track *SongPtrs[NUM_TRACKS];

#endif