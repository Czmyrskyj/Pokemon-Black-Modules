


# Pokémon Black Modules

> **Disclaimer:** This project is an independent, non-commercial educational project created strictly for learning software development. It is not affiliated with, endorsed by, or associated with Nintendo, Game Freak, or The Pokémon Company.
UPDATE: This project is unfinished.

>> If you want to contribute to this project, please read `FOR_CONTRIBUTORS.md`.

## About The Project

**Pokémon Black Modules** is a solo-developed turn-based combat game inspired by the
Pokémon Black & White, Pokémon Showdown and Balatro.

The primary goal of this repository is to practice game architecture, state managment, and data-driven design.

## How to play

Instead of exploring an overworld map or walking through tall grass, progression is entirely run-based and encounter-driven:

1. **Select a Starter:** Pick your starter Pokémon to begin a run.
2. **Battle Progression:** Fight through a series of increasingly difficult trainer/wild pokemon battles.
3. **Earn Currency:** Defeat opponents to earn money.
4. **Shop Phase:** Between battles, visit the shop to heal, buy items, or purchase **Modules**.

### The Module System
Modules alter the fundamental rules of combat for your run:
* **`Sharp Grass`:** Increases damage of Grass-type moves by 5%.
* **`Quick Draw`:** Grants a bonus multiplier to your Speed stat on Turn 1.
* **`Overclock`:** Gives electric-type moves a chance to inflict paralysis, but costs currency on use.

Of course, there are far more than that!

## Tech Stack

**Pokémon Black Modules** was built with modern tooling.

* **Language:**: C++26
* **Graphics Library:** [raylib](https://www.raylib.com)
* **Build System:** CMake

> **Building from Source:** CMake uses `FetchContent` to download and compile `raylib` directly into the project during build time—no manual library installations or path configurations are required.

