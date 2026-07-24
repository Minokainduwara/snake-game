# 🐍 Snake Game

A classic Snake game built with **C** and **raylib** — cross-platform, with audio, difficulty settings, and save/resume support.

![Language](https://img.shields.io/badge/language-C-blue)
![Library](https://img.shields.io/badge/library-raylib-green)
![Platform](https://img.shields.io/badge/platform-macOS%20|%20Linux%20|%20Windows-lightgrey)
![Build](https://github.com/Minokainduwara/snake-game/actions/workflows/build-windows.yml/badge.svg)

---

## 📋 Features

- **6 Game Modes** — Classic, Time Attack, Survival, Maze, No-Wall, Hardcore
- **Classic Snake gameplay** — eat food, grow, avoid walls and yourself
- **Main Menu Dashboard** — Play, Resume, Game Mode, Difficulty, Options, High Scores
- **4 Difficulty Levels** — Easy, Medium, Hard, Expert
- **Sound Effects** — programmatically generated tones for eating, dying, and starting
- **Options Menu** — toggle sound ON/OFF, toggle grid lines ON/OFF
- **Save & Resume** — press ESC to pause and save, resume anytime from the menu
- **Cross-session Persistence** — game state saved to disk, continue after app restart
- **High Score Tracking** — best score saved automatically
- **Keyboard + Mouse** — navigate menus with arrows/WASD + Enter, or click with mouse
- **Cross-platform** — Windows, macOS, Linux with native installers

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

### Android

**Option A — Download APK (Recommended)**

Download the latest `SnakeGame-Android.tar.gz` from:
- **[Google Drive](https://drive.google.com/drive/folders/1gLkAr7Y5-7bxZy72AbD9YwjZ_4aN_ZjO?usp=sharing)** — direct download
- **[GitHub Actions](https://github.com/Minokainduwara/snake-game/actions)** — automated build artifacts

```bash
tar -xzf SnakeGame-Android.tar.gz
cd SnakeGame-Android
# Transfer snake-android to your Android device and run with Termux or similar
```

**Option B — Build from source**

Requires Android NDK and raylib built for Android.

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
| Linux | gcc | `sudo apt install libraylib-dev` |
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
│   ├── game.c / .h     # Snake logic, difficulty system
│   ├── scene.c / .h    # Menu, settings, high scores screens
│   ├── save.c / .h     # File I/O for settings & game state
│   └── audio.c / .h    # Programmatic sound generation
├── installer/
│   └── setup.nsi        # NSIS Windows installer script
├── .github/workflows/
│   └── build-windows.yml # GitHub Actions CI
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
| 🤖 Android | `SnakeGame-Android` | Archive (`.tar.gz`) |

Go to **[Actions tab](https://github.com/Minokainduwara/snake-game/actions)** to download the latest builds.

---

## 📄 License

Copyright © 2026 **Minoka Wickramasinghe**. All rights reserved.

This project and its source code are protected under copyright law. Unauthorized copying, modification, distribution, or use of this software, via any medium, is strictly prohibited without prior written permission from the copyright holder.