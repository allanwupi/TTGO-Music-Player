# TTGO Music Player

Motivation
------------
I found a passive piezo buzzer and I wanted it to play Megalovania, so I programmed my T-Display S3 to do that.

## Current Features
- Play sequences of notes on a connected piezo buzzer
- Two medium-length tracks have been painstakingly implemented
- Screen shows basic 'MIDI'-like visualiser
- Can re-orientate screen on startup menu

## Display Limitations
- The T-Display S3 screen is 320x170 pixels, so room is very limited
- Currently displays array index (max. 999), pitch, and song title (max. 13 characters)

## Planned Features
- Completely rework song player into a non-blocking discrete-time simulation a la VHDL
- Add accurate time keeping and display the play time (will require the rework)
- Add support for chords and a bass line (will require the rework)
- Design or modify a text format for music notation, similar to ABC music notation
- Program should be able to generate songs by parsing a string of the defined format
