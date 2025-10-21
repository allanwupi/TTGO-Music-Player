#ifndef SONGS_H
#define SONGS_H
#include "pitches.h"

#define NUM_SONGS 6
#define NUM_TRACKS 9

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
    int minFreq; // Used for scaling TFT, converts to index into TONE_INDEX
    int maxFreq; // Used for scaling TFT, converts to index into TONE_INDEX
    bool converted; // Notes must be converted before playing
} Song_t;

extern Note megalovania[];
extern Note legend1[];
extern Note legendBass1[];
extern Note legend2[];
extern Note legendBass2[];
extern Note legend3[];
extern Note legendBass3[];
extern Note freedomMotif[];

extern Song_t Megalovania;
extern Song_t TheLegend1;
extern Song_t TheLegendBass1;
extern Song_t TheLegend2;
extern Song_t TheLegendBass2;
extern Song_t TheLegend3;
extern Song_t TheLegendBass3;
extern Song_t FreedomMotif;

extern Song_t MegalovaniaBass;
extern Song_t *SongPtrs[NUM_TRACKS];

#endif