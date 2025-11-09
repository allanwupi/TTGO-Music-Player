# TTGO Music Player
I found a passive piezo buzzer and I wanted it to play Megalovania, so I programmed my T-Display S3 to do that.

### Current Features
- Play sequences of notes (treble and bass tracks) on two connected piezo buzzers
- Song player is non-blocking: could add button press functions in the future
- Two complete songs (melody and bass parts): MEGALOVANIA and THE LEGEND by Toby Fox
- Top line shows play time, bar number, note pitch, and song title (max. 30 characters)
- Rest of the screen (320x170 pixels) is taken up by a 'MIDI'-like visualiser
- Switch screen orientation between horizontal and vertical (trade-off: note length vs pitch)
- User preferences: remembers screen orientation and previous menu selection

### Planned Features
- Implement some useful functions for the TTGO left and right buttons (pause/play?)
- Implement repeats in song data to stop copy-pasting the same sections and save memory
- Write an auxiliary tool to convert MIDI files or ABC notation strings into note data code
