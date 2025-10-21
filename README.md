# TTGO Music Player
I found a passive piezo buzzer and I wanted it to play Megalovania, so I programmed my T-Display S3 to do that.

### Current Features
- Play sequences of notes (treble and bass tracks) on two connected piezo buzzers
- Two complete songs (melody and bass parts): MEGALOVANIA and THE LEGEND by Toby Fox
- Top line shows play time, bar number, note pitch, and song title (max. 30 characters)
- Rest of the screen (320x170 pixels) is taken up by a 'MIDI'-like visualiser
- Switch screen orientation between horizontal and vertical (too see more note length vs. pitch)
- User preferences: remembers screen orientation and previous menu selection

### Short-Term Plans
- Completely rework song player into a non-blocking discrete-time simulation a la VHDL (in progress)
- Support a greater number of tracks (4?) to play chords and such
- Implement repeats in song data to stop copy-pasting the same sections and save memory

### Long-Term Plans
- Design or modify a text format for music notation, similar to ABC music notation
- Program should be able to generate songs by parsing a string of the defined format
