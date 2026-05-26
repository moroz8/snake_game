#define NOMINMAX          // prevent windows.h from defining min/max macros
#include "snake.h"
#include "renderer.h"
#include <windows.h>
#include <conio.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>

// Returns a random food position not occupied by the snake
static Point spawnFood(const Snake& snake, std::mt19937& rng) {
    std::vector<Point> free;
    free.reserve((BOARD_W - 2) * (BOARD_H - 2));

    for (int y = 1; y < BOARD_H - 1; ++y)
        for (int x = 1; x < BOARD_W - 1; ++x) {
            Point p{x, y};
            bool occupied = false;
            for (auto& s : snake.body())
                if (s == p) { occupied = true; break; }
            if (!occupied) free.push_back(p);
        }

    std::uniform_int_distribution<int> dist(0, static_cast<int>(free.size()) - 1);
    return free[dist(rng)];
}

int main() {
    std::mt19937 rng(std::random_device{}());
    Renderer renderer;
    renderer.init();

    // Welcome screen
    renderer.drawWelcome();
    while (!_kbhit()) Sleep(50);
    _getch();

    int highScore = 0;
    {
        std::ifstream in("highscore.txt");
        if (in) in >> highScore;
    }

    // Outer loop — lets the player restart without relaunching
    while (true) {
        Snake snake;
        Point food = spawnFood(snake, rng);
        int score  = 0;
        bool quit  = false;

        renderer.clear();

        // Game loop
        while (!snake.isDead() && !quit) {
            // Input
            while (_kbhit()) {
                int key = _getch();
                if (key == 224 || key == 0) {   // Extended key prefix
                    key = _getch();
                    switch (key) {
                        case 72: snake.setDirection(Dir::North); break;
                        case 80: snake.setDirection(Dir::South); break;
                        case 77: snake.setDirection(Dir::East);  break;
                        case 75: snake.setDirection(Dir::West);  break;
                    }
                } else {
                    switch (tolower(key)) {
                        case 'w': snake.setDirection(Dir::North); break;
                        case 's': snake.setDirection(Dir::South); break;
                        case 'd': snake.setDirection(Dir::East);  break;
                        case 'a': snake.setDirection(Dir::West);  break;
                        case 'q': quit = true; break;
                    }
                }
            }

            if (quit) break;

            // Update
            bool ate = snake.step(food);
            if (ate) {
                score += 10;
                if (score > highScore) highScore = score;
                food = spawnFood(snake, rng);
            }

            // Render
            renderer.draw(snake, food, score, highScore);
            // Speed increases every 50 points (min 60ms)
            int delay = std::max(60, 130 - (score / 50) * 10);
            Sleep(delay);
        }

        if (quit) break;

        // Game-over screen — wait for R or Q
        renderer.draw(snake, food, score, highScore);
        renderer.drawGameOver(score);

        bool decided = false;
        while (!decided) {
            if (_kbhit()) {
                int key = tolower(_getch());
                if (key == 'r') decided = true;       // restart
                else if (key == 'q') { quit = true; decided = true; }
            }
            Sleep(50);
        }
        {
            std::ofstream out("highscore.txt");
            out << highScore;
        }
        if (quit) break;
    }

    // Restore console
    renderer.clear();
    HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {0, 0};
    SetConsoleCursorPosition(hCon, pos);
    CONSOLE_CURSOR_INFO ci = {1, TRUE};
    SetConsoleCursorInfo(hCon, &ci);
    SetConsoleTextAttribute(hCon, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    std::cout << "Thanks for playing! Final score: "
              << "See you next time.\n";
    return 0;
}
