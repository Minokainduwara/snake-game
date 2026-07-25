# 🐍 Snake Game

A classic Snake game built with **C** and **raylib** — cross-platform, with audio, difficulty settings, powerups, and save/resume support.

![Language](https://img.shields.io/badge/language-C-blue)
![Library](https://img.shields.io/badge/library-raylib-green)
![Platform](https://img.shields.io/badge/platform-macOS%20|%20Windows-lightgrey)
![Build](https://github.com/Minokainduwara/snake-game/actions/workflows/build-windows.yml/badge.svg)

---

## 📋 Features

- **6 Game Modes** — Classic, Time Attack, Survival, Maze, No-Wall, Hardcore
- **5 Powerups** — Golden Apple, Speed Boost, Ice, Poison, Heart
- **Classic Snake gameplay** — eat food, grow, avoid walls and yourself
- **Main Menu Dashboard** — Play, Resume, Game Mode, Difficulty, Options, High Scores, Game Manual
- **4 Difficulty Levels** — Easy, Medium, Hard, Expert
- **Sound Effects** — programmatically generated tones for eating, dying, and starting
- **Options Menu** — toggle sound ON/OFF, toggle grid lines ON/OFF
- **Save & Resume** — press ESC to pause and save, resume anytime from the menu
- **Cross-session Persistence** — game state saved to disk, continue after app restart
- **High Score Tracking** — best score saved automatically
- **In-Game Manual** — full game manual accessible from the dashboard
- **Keyboard + Mouse** — navigate menus with arrows/WASD + Enter, or click with mouse
- **Cross-platform** — Windows and macOS with native installers

---

## 🎮 Controls

| Action | Keys |
|--------|------|
| Move Up | `↑` or `W` |
| Move Down | `↓` or `S` |
| Move Left | `←` or `A` |
| Move Right | `→` or `D` |
| Pause & Save | `ESC` |
| Restart (game over) | `R` |
| Menu Navigation | `↑` `↓` or `W` `S` |
| Select | `ENTER` or `SPACE` or `Mouse Click` |
| Go Back | `ESC` |

---

## 📖 How to Play

1. **Navigate the menu** using arrow keys/WASD + Enter, or click with your mouse
2. **Select a game mode** and **difficulty** from the menu
3. **Start a new game** or **resume** a saved game
4. **Control your snake** using arrow keys or WASD
5. **Eat the red apple** to grow and score points (+10 per apple)
6. **Watch for special food** that appears periodically — each has unique effects (see Powerups below)
7. **Avoid walls, obstacles, and yourself** — hitting any of these ends the game
8. **Read the Game Manual** from the dashboard for full details on modes, powerups, and tips
9. **Press ESC** to pause and save your progress at any time
10. **Press R** to restart after game over

### HUD (Heads-Up Display)

During gameplay, the bottom of the screen shows:
- **Score** — current points
- **Time remaining** (Time Attack mode only) — turns red when below 10 seconds
- **Level** (Survival mode only) — increases every 30 seconds
- **Powerup bars** — yellow bar for Speed Boost, sky blue bar for Ice Slow effect

---

## 🖥️ Platforms

### macOS

**Option A — Download DMG (Recommended)**

Download the latest `.dmg` from:
- **[Google Drive](https://drive.google.com/drive/folders/1gLkAr7Y5-7bxZy72AbD9YwjZ_4aN_ZjO?usp=sharing)** — direct download
- **[GitHub Actions](https://github.com/Minokainduwara/snake-game/actions)** — automated build artifacts

Open `SnakeGame-macOS.dmg` and drag the app to your Applications folder.

**Option B — Build from source**

```bash
brew install raylib
make
make run
```

### Windows

**Option A — Download Installer (Recommended)**

Download the latest setup from:
- **[Google Drive](https://drive.google.com/drive/folders/1gLkAr7Y5-7bxZy72AbD9YwjZ_4aN_ZjO?usp=sharing)** — direct download
- **[GitHub Actions](https://github.com/Minokainduwara/snake-game/actions)** — automated build artifacts

Run `SnakeGame-Setup-1.0.0.exe` on Windows 11.

**Option B — Build with MSYS2/MinGW**

```bash
pacman -S mingw-w64-x86_64-raylib
make -f Makefile.windows
```

---

## 🛠️ Build from Source

### Prerequisites

| Platform | Compiler | raylib |
|----------|----------|--------|
| macOS | clang | `brew install raylib` |
| Windows (MSYS2) | MinGW-w64 | `pacman -S mingw-w64-x86_64-raylib` |

### Build

```bash
# Clone the repo
git clone https://github.com/Minokainduwara/snake-game.git
cd snake-game

# Build
make

# Run
make run
```

### Clean

```bash
make clean
```

---

## 📁 Project Structure

```
snake-game/
├── src/
│   ├── main.c          # Scene-based game loop
│   ├── game.c / .h     # Snake logic, difficulty system, powerups
│   ├── scene.c / .h    # Menu, settings, high scores, manual screens
│   ├── save.c / .h     # File I/O for settings & game state
│   └── audio.c / .h    # Programmatic sound generation
├── installer/
│   └── setup.nsi        # NSIS Windows installer script
├── .github/workflows/
│   ├── build-windows.yml # Windows CI
│   └── build-macos.yml   # macOS CI
├── Makefile              # macOS / Linux build
├── Makefile.windows      # Windows MinGW cross-compile
└── saves/                # Settings & save data (auto-created)
```

---

## ⚙️ Difficulty Levels

| Level | Tick Rate | Description |
|-------|-----------|-------------|
| 🟢 Easy | 0.20s | Slow — great for beginners |
| 🟡 Medium | 0.15s | Normal speed (default) |
| 🟠 Hard | 0.10s | Fast |
| 🔴 Expert | 0.06s | Very fast — for pros |

## 🎲 Game Modes

| Mode | Description |
|------|-------------|
| 🟦 Classic | Standard snake — walls kill, constant speed |
| ⏱️ Time Attack | 2-minute countdown, eat as much as possible |
| 📈 Survival | Speed increases every 30 seconds |
| 🧱 Maze | 15 random obstacles spawn on the grid |
| 🔄 No-Wall | Snake wraps around edges (Pac-Man style) |
| 💀 Hardcore | Fixed high speed, one mistake = game over |

## ⚡ Powerups

Special food appears periodically during gameplay. Each type has a unique effect:

| Powerup | Points | Effect |
|---------|--------|--------|
| 🟡 **Golden Apple** | +50 | Grows snake by 1 segment |
| 🟠 **Speed Boost** | +10 | 2x speed for a short time |
| 🔵 **Ice** | +10 | Slows down for a short time |
| 🟤 **Poison** | -10 | Loses 2 segments (minimum length 3) |
| ❤️ **Heart** | +20 | Grows snake by 1 segment |

**Special Food Mechanics:**
- First special food appears after 5 seconds
- Special food stays on screen for 8 seconds
- After eating or expiring, the next one appears in 8–15 seconds
- 5 possible types are randomly selected (regular apple excluded)

**Powerup Duration Scaling:**

| Difficulty | Speed Boost | Ice |
|------------|-------------|-----|
| 🟢 Easy | 8.0s | 8.0s |
| 🟡 Medium | 5.0s | 5.0s |
| 🟠 Hard | 3.75s | 3.75s |
| 🔴 Expert | 2.5s | 2.5s |

**Visual Indicators:**
- Active powerups show a health bar in the HUD
- **SPEED** bar (yellow) — depletes as the boost runs out
- **SLOW** bar (sky blue) — depletes as the slow effect runs out

---

## 💾 Save System

- **Settings** saved to `saves/snake_save.dat` (difficulty, sound, grid, high score)
- **Game state** saved to `saves/snake_game.dat` when you press ESC during gameplay
- **Resume** option appears on the main menu when a saved game exists
- Game state is cleared on game over or when starting a new game

---

## 🧪 GitHub Actions

Every push to `main` triggers automated builds for all platforms:

| Platform | Artifact | Format |
|----------|----------|--------|
| 🪟 Windows | `SnakeGame-Setup` | NSIS Installer (`.exe`) |
| 🍎 macOS | `SnakeGame-macOS` | Disk Image (`.dmg`) |

Go to **[Actions tab](https://github.com/Minokainduwara/snake-game/actions)** to download the latest builds.

---

## 📄 License

Copyright © 2026 **Minoka Wickramasinghe**. All rights reserved.

This project and its source code are protected under copyright law. Unauthorized copying, modification, distribution, or use of this software, via any medium, is strictly prohibited without prior written permission from the copyright holder.