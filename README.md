# TTGO Music Player
I found a passive piezo buzzer and I wanted it to play Megalovania, so I programmed my T-Display S3 to do that.

### Current Features
- Play sequences of notes (integer pitch and duration) on a connected piezo buzzer
- Two fully playable songs (melody and bass line): MEGALOVANIA, THE LEGEND
- Top line shows play time, bar number, note pitch, and song title (max. 30 characters)
- Rest of the screen (320x150 pixels) is taken up by a 'MIDI'-like visualiser
- Preferences: remembers screen orientation and last menu selection

### Short-Term Plans
- Be able to switch between horizontal (currently working) and vertical layouts
- Completely rework song player into a non-blocking discrete-time simulation a la VHDL (in progress)
- Support a greater number of tracks (4?) to play chords and such
- Implement repeats in song data to stop copy-pasting the same sections and save memory

### Long-Term Plans
- Design or modify a text format for music notation, similar to ABC music notation
- Program should be able to generate songs by parsing a string of the defined format
