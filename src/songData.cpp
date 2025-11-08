#include "songs.h"
#include "pitches.h"
#include <TFT_eSPI.h>

const char *SONG_DESCRIPTIONS[NUM_SONGS+1] = {
    "1. MEGALOVANIA (2m37s)",
    "2. THE LEGEND (1m48s)",
    "3. FREEDOM MOTIF (0m13s)",
    "4. -------------",
    "5. -------------",
    "6. View Track Info",
    "7. Rotate Screen",
};

Track Megalovania_T = {
    .name = "MEGALOVANIA - Toby Fox",
    .notes = megalovania_t,
    .size = 766,
    .beat = 63,
    .bar = 32,
};

Track Megalovania_B = {
    .name = "MEGALOVANIA (Bass Line)",
    .notes = megalovania_b,
    .size = 978, 
    .beat = 63,
    .bar = 32,
};

Track TheLegend1_T = {
    .name = "THE LEGEND - Toby Fox (1/3)",
    .notes = legend1_t,
    .size = 33,
    .beat = 272,
    .bar = 8,
};

Track TheLegend1_B = {
    .name = "THE LEGEND (Bass Line) (1/3)",
    .notes = legend1_b,
    .size = 20,
    .beat = 272,
    .bar = 8,
};

Track TheLegend2_T = {
    .name = "THE LEGEND - Toby Fox (2/3)",
    .notes = legend2_t,
    .size = 134,
    .beat = 136,
    .bar = 16,
};

Track TheLegend2_B = {
    .name = "THE LEGEND (Bass Line) (2/3)",
    .notes = legend2_b,
    .size = 157,
    .beat = 136,
    .bar = 16,
};

Track TheLegend3_T = {
    .name = "THE LEGEND - Toby Fox (3/3)",
    .notes = legend3_t,
    .size = 50,
    .beat = 46,
    .bar = 24,
};

Track TheLegend3_B = {
    .name = "THE LEGEND (Bass Line) (3/3)",
    .notes = legend3_b,
    .size = 55,
    .beat = 46,
    .bar = 24,
};

Track FreedomMotif = {
    .name = "FREEDOM MOTIF",
    .notes = freedomMotif,
    .size = 31,
    .beat = 107,
    .bar = 16,
};

Track *Tracks[NUM_TRACKS] = {
    &Megalovania_T, &Megalovania_B, 
    &TheLegend1_T, &TheLegend1_B,
    &TheLegend2_T, &TheLegend2_B,
    &TheLegend3_T, &TheLegend3_B, 
    &FreedomMotif,
};

MultiTrack MEGALOVANIA = {
    .name = "MEGALOVANIA - Toby Fox",
    .tracks = {&Megalovania_T, &Megalovania_B},
    .size = 2,
    .channels = {TREBLE, BASS},
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

MultiTrack FREEDOM_MOTIF = {
    .name = "FREEDOM MOTIF – Toby Fox",
    .tracks = {&FreedomMotif},
    .size = 1,
    .channels = {TREBLE},
    .colours = {TFT_GOLD}
};