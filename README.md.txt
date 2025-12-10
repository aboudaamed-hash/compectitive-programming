# 🧠 ARENA DE DÉFIS ALGORITHMIQUES – C

A terminal-based C application that brings together multiple classic
algorithmic challenges in one interactive arena.

---

## 🎮 Features

The program offers 5 algorithmic challenges via a menu-driven interface.

### 1️⃣ Le Compte Est Bon
- Given 6 numbers and a target integer
- Uses recursive backtracking with arithmetic operations
- Displays a valid expression if a solution exists

### 2️⃣ Mastermind Algorithmique
Two modes:
- Human mode: you guess the computer’s code
- Computer mode: the computer solves your code

Parameters:
- 4 positions
- Colors numbered from 1 to 6

### 3️⃣ Course de Robots (Labyrinthe)
- Grid-based labyrinth
- Shortest path search using Breadth-First Search (BFS)
- Start: 'S' / Goal: 'G'
- Path output using directions: N S E O

### 4️⃣ Tri Ultime
- Selection sort with instrumentation
- Counts comparisons and swaps
- Optional operation limit to interrupt sorting

### 5️⃣ Classement
- Scores saved to a file (scores.txt)
- Leaderboard sorted by score (descending)

---

## 🛠️ Compilation

Use GCC:

gcc main.c -o arena -lm

The -lm flag is required for math functions.

---

## ▶️ Execution

./arena

---

## 📂 Files

main.c        -> main source code  
scores.txt    -> score storage (auto-created)  
ascii_art.txt -> optional ASCII art intro  
README.md     -> project documentation  

---

## ⌨️ Input Handling

- Uses fgets + sscanf in menus to avoid input buffer issues
- Input buffers are cleared after scanf when required
- Prevents accidental invalid or skipped input

---

## 🧩 Algorithms Used

- Recursive backtracking
- Constraint filtering
- Breadth-First Search (BFS)
- Selection sort
- File I/O
- Dynamic memory allocation

---

## ✅ Sample Menu

===== ARENA DE DEFIS ALGORITHMIQUES =====  
1. Defi - Le Compte est Bon  
2. Mastermind Algorithmique  
3. Course de Robots (Labyrinthe)  
4. Tri Ultime  
5. Voir le classement  
0. Quitter  

---

## 🧪 Tested On

- GCC (Linux / Windows MinGW)
- C99 standard

---

## ✨ Possible Improvements

- Performance timing
- Smarter Mastermind strategy (Knuth)
- Score system per game
- Visual maze rendering
- Full input parsing with strtol

---

## 👤 Author

Developed by Abdel Karim  
Language: C  
Project type: Educational / Algorithmic Practice
