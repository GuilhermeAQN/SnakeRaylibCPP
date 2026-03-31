# 🐍 RetroSnake

A classic Snake game built with **C++** and **Raylib**, developed as a hands-on project to learn the C++ language from scratch.

![Raylib](https://img.shields.io/badge/Raylib-v5.5-darkgreen?style=flat-square)
![C++](https://img.shields.io/badge/C++-17-blue?style=flat-square)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey?style=flat-square)

---

## 🎓 Tutorial Reference

This project was initially built by following the tutorial below, 
then expanded with my own modifications and improvements:

- [Snake Game in C++ with Raylib – by GamesWithGabe](https://youtu.be/LGqsnM_WEK4?list=PLwR6ZGPvjVOSRywn9VCQ3yrRVruxzzuo9)

---

## 🎮 Gameplay

You control a snake across a 2D grid using the **arrow keys**. An apple appears randomly on the grid — eat it to grow your snake and increase your score. The snake wraps around the screen edges, appearing on the opposite side when crossing a border. The game ends if the snake's head collides with any part of its own body.

### Controls

| Key | Action |
|---|---|
| ↑ Arrow | Move Up |
| ↓ Arrow | Move Down |
| ← Arrow | Move Left |
| → Arrow | Move Right |

## 🚀 How to Run

### Requirements
- [Raylib v5.5](https://www.raylib.com/) installed in your environment
- Any C++ compiler (MSVC, GCC, Clang)
- IDE that supports C++

### Steps
1. Clone the repository:
   ```bash
   git clone https://github.com/GuilhermeAQN/retro-snake.git
   ```
2. Open the project in Visual Studio 2022
3. Make sure Raylib is linked to your project
4. Build and run

---

## 🛠️ Development Environment

- **Language:** C++ 17
- **IDE:** Visual Studio Community 2022
- **Graphics Library:** [Raylib v5.5](https://www.raylib.com/)
- **Key C++ concepts applied:**
  - Classes and Object-Oriented Programming
  - STL containers (`std::deque`)
  - Functions and scope
  - Variables and type casting
  - Conditionals and loops
  - Third-party library integration

---

## 📁 Project Structure

```
RetroSnake/
├── graphics/
│   └── food.png          # Food sprite
├── sounds/
│   ├── eat.mp3           # Sound played when food is eaten
│   └── wall.mp3          # Sound played on game over
├── main.cpp              # Full game source code
└── README.md
```

---

## 🔮 Future Work

### Planned Features
- [ ] **Individual snake sprites** — separate head, body, tail and corner sprites, similar to the original Nokia Snake game
- [ ] **In-body food indicator** — a visual marker showing where the eaten food is traveling through the snake's body, just like the original Nokia game
- [ ] **Main menu screen** — title screen with play, options and quit buttons
- [ ] **Stage system** — multiple levels with increasing speed and new obstacle layouts
- [ ] **Gameplay timer** — track and display how long the player survives each run
- [ ] **Improved scoring system** — combo multipliers for eating food in quick succession, bonus points for surviving longer

### Ideas for the Future
- [ ] **High score board** — save and display the top scores locally
- [ ] **Obstacles** — static walls inside the grid that increase in number each stage
- [ ] **Power-ups** — temporary effects like slow motion, score multiplier or invincibility
- [ ] **Ghost mode** — a limited-time ability to pass through your own body
- [ ] **Web build** — compile to WebAssembly with Emscripten so anyone can play in the browser without installing anything

---

## 🎯 Purpose

This project was built as my **first C++ project**, with the goal of learning the language through practical game development. Raylib was chosen for its simplicity, allowing me to focus on C++ fundamentals rather than low-level graphics boilerplate.

---

## 📚 Useful Resources

- [Raylib Official Website](https://www.raylib.com/)
- [Raylib Cheatsheet v5.5](https://www.raylib.com/cheatsheet/cheatsheet.html)
- [W3Schools C++](https://www.w3schools.com/cpp/)
- [cppreference.com](https://en.cppreference.com/)

---

## 📄 License

This project is open source and available under the [MIT License](LICENSE).

---

*Developed by [Guilherme Nogueira](https://github.com/GuilhermeAQN)*

---

## 📸 Screenshot

> <img width="898" height="929" alt="image" src="https://github.com/user-attachments/assets/b7b76e39-5f06-414e-b5eb-80867c42b029" />

---
