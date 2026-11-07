# Ping

A two-player paddle game written in C++17 with SDL2. Bounce the ball past your opponent to
score; the ball speeds up with every hit until someone scores. Includes a title screen, a
square-wave soundtrack, and a `config.json` file for tuning sizes, speeds and music without
rebuilding.

## Controls

- Title screen: any key starts the game
- Left player: **W** / **S**
- Right player: **Up** / **Down** arrows
- **Esc** or closing the window quits.

## Download and play

Download the archive for your platform from the
[Releases](https://github.com/ajay-sr/Ping/releases) page and extract it. Each archive contains
the game, `config.json`, this README and the license.

- **Linux (x86_64):** install SDL2, then run the game from the extracted folder:
  ```sh
  sudo apt install libsdl2-2.0-0   # Debian/Ubuntu; other distros: install the "SDL2" package
  ./ping
  ```
- **Windows (64-bit):** double-click `ping.exe`. Windows may show a SmartScreen warning because
  the game isn't signed; choose **More info**, then **Run anyway**.
- **macOS (Apple silicon, macOS 14 or later):** the game isn't signed by Apple, so macOS blocks
  it at first. In Terminal, from the extracted folder, run:
  ```sh
  xattr -dr com.apple.quarantine .
  ./ping
  ```

## Building from source

### Linux / WSL

```sh
sudo apt install build-essential cmake libsdl2-dev
cmake -S . -B build
cmake --build build
./build/ping
```

### macOS

```sh
brew install cmake sdl2
cmake -S . -B build
cmake --build build
./build/ping
```

### Windows

With Visual Studio (C++ workload), CMake and [vcpkg](https://vcpkg.io) installed:

```powershell
vcpkg install sdl2:x64-windows
cmake -S . -B build -A x64 -DCMAKE_TOOLCHAIN_FILE="<vcpkg root>/scripts/buildsystems/vcpkg.cmake"
cmake --build build --config Release
.\build\Release\ping.exe
```

Builds are optimized (`Release`) by default; pass `-DCMAKE_BUILD_TYPE=Debug` for a debug build.

## Configuration

Settings are read from `config.json` when the game starts, so they can be changed without
rebuilding. The game uses the first of these it finds:

1. a path given as the first argument, e.g. `./build/ping my-config.json`
2. `config.json` in the current directory
3. `config.json` next to the executable

Missing keys keep their defaults; if no file is found, all defaults are used.

| Key | Default | Meaning |
|---|---|---|
| `windowWidth` | 800 | Window width in pixels |
| `windowHeight` | 680 | Window height in pixels, including the score bar |
| `targetFps` | 60 | Frames per second (at least 1). Game speed stays the same at any frame rate, but slows down if the computer can't keep up |
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

## Making a release

The [Release workflow](.github/workflows/release.yml) builds Linux, Windows and macOS archives
on GitHub Actions. To publish a release, push a version tag:

```sh
git tag v1.0.0
git push origin v1.0.0
```

The workflow builds all three platforms and creates a GitHub release for the tag with the
archives attached. To test the builds without publishing, run the workflow by hand from the
repository's **Actions** tab and download the archives from the run's **Artifacts** section.

## License

Ping is released under the [MIT License](LICENSE).
