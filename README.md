# GameDealFinder

**A simple price-comparison tool written in C++**  
Compare game prices across multiple platforms and find the best deal.  
Supports bulk add/update, partial-name deletion with confirmation, and price editing.

---

## Features

- Search games by full or partial name  
- List all available games  
- Bulk add
- Delete games  
- Update the price of an existing game  
- All data is stored in plain `.txt` files (one per platform)

---

## Installation (Windows)

> Run the installer as Administrator the first time to avoid permission issues.

1. Download and run `GameDealFinder_Setup.exe`  
2. Follow the installation steps  
3. Launch GameDealFinder from the Start Menu or Desktop

---

## Usage

When you launch the program, you’ll see the following menu:

```
1) Search for a game
2) List all available games
3) Bulk add
4) Delete a game
5) Update the price of a game
6) Exit
```

### Bulk Add/Update

When adding multiple games (option 3), use the following format:

```
GTA V;steam;29.99
The Witcher 3;gog;49.99
Cyberpunk 2077;epic;59.99
Hades;steam;21.99
```

- Leave an **empty line** to finish input  
- If a game already exists for a platform, the price will be updated

### Delete Games

- Enter part of the game name (e.g., "grand")  
- Matching titles will be listed  
- You can:
  - Select specific titles to delete
  - Choose which platforms to delete from
  - Or type `0` to delete all matched games  
- Deleting all will require confirmation before proceeding

---

## Data

- All game data is stored in the `data/` folder  
- Each platform has its own `.txt` file:
  - `steam.txt`
  - `epic.txt`
  - `gog.txt`
  - `microsoft.txt`

---

## Developer

This project was developed by **Hüseyin Can Çaltı**  
Website: [https://huseyincancalti.github.io/karakedidub/](https://huseyincancalti.github.io/karakedidub/)
