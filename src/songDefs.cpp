#include "songs.h"
#include "pitches.h"
#include <TFT_eSPI.h>

const char *MENU[NUM_MENU_OPTIONS+1] = {
    "1. MEGALOVANIA (2m37s)",
    "2. THE LEGEND (1m49s)",
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
    .scale = {"C","Db","D","Eb","E","F","F#","G","Ab","A","Bb","B"},
};

Track Megalovania_B = {
    .name = "MEGALOVANIA (Bass Line)",
    .notes = megalovania_b,
    .size = 978, 
    .beat = 63,
    .bar = 32,
    .scale = {"C","Db","D","Eb","E","F","F#","G","Ab","A","Bb","B"},
};

Track TheLegend1_T = {
    .name = "THE LEGEND - Toby Fox (1/3)",
    .notes = legend1_t,
    .size = 33,
    .beat = 272,
    .bar = 8,
    .scale = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"},
};

Track TheLegend1_B = {
    .name = "THE LEGEND (Bass Line) (1/3)",
    .notes = legend1_b,
    .size = 20,
    .beat = 272,
    .bar = 8,
    .scale = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"},
};

Track TheLegend2_T = {
    .name = "THE LEGEND - Toby Fox (2/3)",
    .notes = legend2_t,
    .size = 134,
    .beat = 136,
    .bar = 16,
    .scale = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"},
};

Track TheLegend2_B = {
    .name = "THE LEGEND (Bass Line) (2/3)",
    .notes = legend2_b,
    .size = 157,
    .beat = 136,
    .bar = 16,
    .scale = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"},
};

Track TheLegend3_T = {
    .name = "THE LEGEND - Toby Fox (3/3)",
    .notes = legend3_t,
    .size = 50,
    .beat = 46,
    .bar = 24,
    .scale = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"},
};

Track TheLegend3_B = {
    .name = "THE LEGEND (Bass Line) (3/3)",
    .notes = legend3_b,
    .size = 55,
    .beat = 46,
    .bar = 24,
    .scale = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"},
};

Track FreedomMotif = {
    .name = "FREEDOM MOTIF",
    .notes = freedomMotif,
    .size = 31,
    .beat = 107,
    .bar = 16,
    .scale = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"},
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
    .scale = {"C","Db","D","Eb","E","F","F#","G","Ab","A","Bb","B"},
};

MultiTrack LEGEND1 = {
    .name = "THE LEGEND - Toby Fox (1/3)",
    .tracks = {&TheLegend1_T, &TheLegend1_B},
    .size = 2,
    .channels = {TREBLE, BASS},
    .scale = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"},
};

MultiTrack LEGEND2 = {
    .name = "THE LEGEND - Toby Fox (2/3)",
    .tracks = {&TheLegend2_T, &TheLegend2_B},
    .size = 2, 
    .channels = {TREBLE, BASS},
    .scale = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"},
};

MultiTrack LEGEND3 = {
    .name = "THE LEGEND - Toby Fox (3/3)",
    .tracks = {&TheLegend3_T, &TheLegend3_B},
    .size = 2,
    .channels = {TREBLE, BASS},
    .scale = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"},
};

MultiTrack FREEDOM_MOTIF = {
    .name = "FREEDOM MOTIF – Toby Fox",
    .tracks = {&FreedomMotif},
    .size = 1,
    .channels = {TREBLE},
    .colours = {TFT_GOLD},
    .scale = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"},
};