# Ping

A C++17 / SDL2 game skeleton: opens a window with two paddles (`Player` objects), one at
each side of the screen, and a ball that bounces between them. A score bar above the play field counts the points
each player wins when the ball passes the opposite edge. The game loop is capped at 60 frames per second.

## Dependencies (Ubuntu / WSL)

```sh
sudo apt install build-essential cmake libsdl2-dev
```

## Build & run

```sh
cmake -S . -B build
cmake --build build
./build/ping
```

Controls:
- Title screen: any key starts the game
- Left player: **W** / **S**
- Right player: **Up** / **Down** arrows
- **Esc** or closing the window quits.

## Configuration

Settings are read from `config.json` in the current directory when the game starts (or from
a path given as the first argument, e.g. `./build/ping my-config.json`), so they can be changed
without rebuilding. Missing keys keep their defaults; if the file is missing, all defaults are used.

| Key | Default | Meaning |
|---|---|---|
| `windowWidth` | 800 | Window width in pixels |
| `windowHeight` | 680 | Window height in pixels, including the score bar |
| `ballSpeedX` | 0.375 | Horizontal ball speed, in play field widths per second |
| `ballSpeedY` | 0.84 | Vertical ball speed after a hit on a paddle's top or bottom edge, in field heights per second |
| `paddleSpeed` | 0.67 | Paddle speed, in field heights per second |
| `paddleSections` | 9 | Number of equal paddle sections; each sends the ball off with a vertical direction spaced evenly from -1 (top) to 1 (bottom), times `ballSpeedY` |
| `ballSpeedUpPerHit` | 0.05 | Fraction the ball speeds up on each paddle hit (0 turns it off); resets when a point is scored |
| `ballMaxSpeedMultiplier` | 3.0 | Cap on the speed-up, as a multiple of the ball's base speed (at least 1) |
| `musicVolume` | 0.25 | Music volume, from 0 (silent) to 1 (full) |
| `musicBpm` | 60 | Music tempo in beats per minute |
| `musicMelody` | 10 notes | Array of frequencies in Hz, one per beat (0 = rest), looped as a square wave |

All other sizes (paddles, ball, score bar) are fractions of the window size.

## Creating music

The jukebox plays `musicMelody` from `config.json` on a single square-wave channel, from the
title screen onwards, looping forever. Edit the file and restart the game to hear changes; no
rebuild is needed.

### How a melody is read

- Each entry in `musicMelody` is **one beat**, and is the frequency of the note in Hz.
- `0` is a rest (one beat of silence).
- `musicBpm` sets how many beats play per minute, so at the default 60 BPM each entry lasts one
  second.
- The loop is as long as the list: 10 entries make a 10-beat loop, 16 entries a 16-beat loop.
- Each note sounds for the first 90% of its beat and is silent for the rest, so two equal notes
  in a row are heard as two separate notes.
- Only one note plays at a time; there are no chords.

### Note frequencies (Hz)

| Note | Octave 3 | Octave 4 | Octave 5 |
|---|---|---|---|
| C | 130.81 | 261.63 | 523.25 |
| C# / Db | 138.59 | 277.18 | 554.37 |
| D | 146.83 | 293.66 | 587.33 |
| D# / Eb | 155.56 | 311.13 | 622.25 |
| E | 164.81 | 329.63 | 659.26 |
| F | 174.61 | 349.23 | 698.46 |
| F# / Gb | 185.00 | 369.99 | 739.99 |
| G | 196.00 | 392.00 | 783.99 |
| G# / Ab | 207.65 | 415.30 | 830.61 |
| A | 220.00 | 440.00 | 880.00 |
| A# / Bb | 233.08 | 466.16 | 932.33 |
| B | 246.94 | 493.88 | 987.77 |

Octave 4 contains middle C (C4). To move a note up an octave, double its frequency; to move it
down an octave, halve it. Any frequency from 0 to 20000 Hz is accepted, but square waves sound
best between about 100 and 1000 Hz.

### Short and long notes

Every entry lasts exactly one beat, so note lengths are set through the tempo:

- **Shorter notes:** raise `musicBpm` and give each original beat several entries. For example,
  at 120 BPM each entry is half a second, so a one-second note takes two entries and a
  half-second note takes one.
- **Longer notes:** a note can't be held across beats, because each entry restarts the note.
  Follow it with rests (`0`) to give it room, or use a lower `musicBpm`.

### Example

A 16-beat loop at 120 BPM (8 seconds) that climbs up and back down a C major chord, then a
D minor chord, with a rest at the end of each phrase:

```json
"musicBpm": 120,
"musicMelody": [
    261.63, 329.63, 392.00, 523.25, 392.00, 329.63, 261.63, 0,
    293.66, 349.23, 440.00, 587.33, 440.00, 349.23, 293.66, 0
]
```

### Tips

- Set `musicVolume` to `0` to turn the music off without deleting the melody.
- Keep `musicVolume` at about 0.3 or below; square waves sound harsh at full volume.
- A rest at the end of the list makes the start of each loop easy to hear.
- If the game reports an error on startup, check that `musicMelody` is in square brackets,
  that its numbers are separated by commas, and that there is no comma after the last number.

## If you have a cat

If you have a cat, set the music volume to 0 in `config.json`:

```json
"musicVolume": 0
```

Square waves are harsh and repetitive, and the melody loops for as long as the game is open.
Turning the volume to 0 silences the music without removing the melody, so you can turn it back
up later.
