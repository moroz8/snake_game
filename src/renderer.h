#pragma once
#include "snake.h"

class Renderer {
public:
    Renderer();
    void init();
    void draw(const Snake& snake, Point food, int score, int highScore);
    void drawGameOver(int score);
    void drawWelcome();
    void clear();

private:
    void setCursor(int x, int y) const;
    void setColor(int attr) const;
    void resetColor() const;
};
