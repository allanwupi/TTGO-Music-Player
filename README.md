# TTGO Music Player
I found a passive piezo buzzer and I wanted it to play Megalovania, so I programmed my T-Display S3 to do that.

### Current Features
- Play sequences of notes (integer pitch and duration) on a connected piezo buzzer
- Megalovania (melody and bass line) is completely playable, more songs to come
- Top line shows play time, bar number, note pitch, and song title (max. 30 characters)
- Rest of the screen (320x150 pixels) is taken up by a 'MIDI'-like visualiser
- Can re-orientate screen on startup menu

### Planned Features
- Completely rework song player into a non-blocking discrete-time simulation a la VHDL (in progress)
- Support an arbitrary number of channels playing at once for chords and such
- Design or modify a text format for music notation, similar to ABC music notation
- Program should be able to generate songs by parsing a string of the defined format
