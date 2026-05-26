#include "renderer.h"
#include <windows.h>
#include <iostream>
#include <string>

// Console color attributes
#define C_WHITE   (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
#define C_BRIGHT  (C_WHITE | FOREGROUND_INTENSITY)
#define C_GREEN   (FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define C_DKGREEN (FOREGROUND_GREEN)
#define C_RED     (FOREGROUND_RED | FOREGROUND_INTENSITY)
#define C_YELLOW  (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define C_CYAN    (FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY)

static HANDLE hCon = INVALID_HANDLE_VALUE;

Renderer::Renderer() {}

void Renderer::init() {
    hCon = GetStdHandle(STD_OUTPUT_HANDLE);

    // Hide cursor
    CONSOLE_CURSOR_INFO ci = {1, FALSE};
    SetConsoleCursorInfo(hCon, &ci);

    // Size the window and buffer to fit the board + status lines
    COORD bufSize = {BOARD_W + 2, BOARD_H + 4};
    SetConsoleScreenBufferSize(hCon, bufSize);
    SMALL_RECT win = {0, 0, BOARD_W + 1, BOARD_H + 3};
    SetConsoleWindowInfo(hCon, TRUE, &win);

    // Set console title
    SetConsoleTitleA("Simple Snake Game");
}

void Renderer::setCursor(int x, int y) const {
    COORD pos = {static_cast<SHORT>(x), static_cast<SHORT>(y)};
    SetConsoleCursorPosition(hCon, pos);
}

void Renderer::setColor(int attr) const {
    SetConsoleTextAttribute(hCon, static_cast<WORD>(attr));
}

void Renderer::resetColor() const {
    setColor(C_WHITE);
}

void Renderer::clear() {
    COORD origin = {0, 0};
    DWORD written;
    DWORD size = (BOARD_W + 2) * (BOARD_H + 4);
    FillConsoleOutputCharacterA(hCon, ' ', size, origin, &written);
    FillConsoleOutputAttribute(hCon, C_WHITE, size, origin, &written);
}

void Renderer::draw(const Snake& snake, Point food, int score, int highScore) {
    // Build a character grid
    char grid[BOARD_H][BOARD_W];
    int  attr[BOARD_H][BOARD_W];
    for (int y = 0; y < BOARD_H; ++y)
        for (int x = 0; x < BOARD_W; ++x) {
            grid[y][x] = ' ';
            attr[y][x] = C_WHITE;
        }

    // Border
    for (int x = 0; x < BOARD_W; ++x) {
        grid[0][x]          = '#'; attr[0][x]          = C_BRIGHT;
        grid[BOARD_H-1][x]  = '#'; attr[BOARD_H-1][x]  = C_BRIGHT;
    }
    for (int y = 0; y < BOARD_H; ++y) {
        grid[y][0]          = '#'; attr[y][0]          = C_BRIGHT;
        grid[y][BOARD_W-1]  = '#'; attr[y][BOARD_W-1]  = C_BRIGHT;
    }

    // Food
    grid[food.y][food.x] = '@';
    attr[food.y][food.x] = C_RED;

    // Snake body (iterate back-to-front so head wins if overlap ever happens)
    const auto& body = snake.body();
    for (int i = static_cast<int>(body.size()) - 1; i >= 0; --i) {
        auto& p = body[i];
        if (i == 0) {
            grid[p.y][p.x] = 'O'; attr[p.y][p.x] = C_GREEN;
        } else {
            grid[p.y][p.x] = 'o'; attr[p.y][p.x] = C_DKGREEN;
        }
    }

    // Flush grid to console
    for (int y = 0; y < BOARD_H; ++y) {
        setCursor(0, y);
        for (int x = 0; x < BOARD_W; ++x) {
            setColor(attr[y][x]);
            std::cout << grid[y][x];
        }
    }

    // Status bar
    resetColor();
    setCursor(0, BOARD_H + 1);
    std::cout << "Score: " << score << "   Best: " << highScore << "        ";
    setCursor(0, BOARD_H + 2);
    std::cout << "Arrow keys / WASD to move   Q to quit";
}

void Renderer::drawGameOver(int score) {
    int cx = BOARD_W / 2 - 5;
    int cy = BOARD_H / 2 - 1;

    setColor(C_RED);
    setCursor(cx, cy);
    std::cout << "  GAME OVER  ";
    setCursor(cx, cy + 1);
    std::cout << "  Score: " << score << "      ";
    resetColor();
    setCursor(cx - 2, cy + 2);
    std::cout << "R = restart   Q = quit";
}

void Renderer::drawWelcome() {
    clear();
    int cx = BOARD_W / 2 - 8;
    int cy = BOARD_H / 2 - 3;

    setColor(C_YELLOW);
    setCursor(cx, cy);     std::cout << " **** SNAKE GAME ****";
    resetColor();
    setCursor(cx, cy + 2); std::cout << "  Arrow keys or WASD";
    setCursor(cx, cy + 3); std::cout << "  Eat @ to grow";
    setCursor(cx, cy + 4); std::cout << "  Avoid walls & yourself";
    setColor(C_GREEN);
    setCursor(cx, cy + 6); std::cout << "  Press any key to start";
    resetColor();
}
