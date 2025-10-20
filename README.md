# TTGO Music Player
I found a passive piezo buzzer and I wanted it to play Megalovania, so I programmed my T-Display S3 to do that.

### Current Features
- Play sequences of notes (integer pitch and duration) on a connected piezo buzzer
- Two medium-length tracks have been painstakingly implemented
- Top line shows array index (max. 999), note pitch, and title (max. 13 characters)
- Rest of the screen (320x150 pixels) is taken up by a 'MIDI'-like visualiser
- Can re-orientate screen on startup menu

### Planned Features
- Completely rework song player into a non-blocking discrete-time simulation a la VHDL
- Add accurate time keeping and display the play time (will require the rework)
- Add support for chords and a bass line (will require the rework)
- Design or modify a text format for music notation, similar to ABC music notation
- Program should be able to generate songs by parsing a string of the defined format
