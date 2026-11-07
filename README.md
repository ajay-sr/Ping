# Pong

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
./build/pong
```

## Configuration

Settings are read from `config.json` in the current directory when the game starts (or from
a path given as the first argument, e.g. `./build/pong my-config.json`), so they can be changed
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

All other sizes (paddles, ball, score bar) are fractions of the window size.

Controls:
- Title screen: any key starts the game
- Left player: **W** / **S**
- Right player: **Up** / **Down** arrows
- **Esc** or closing the window quits.
