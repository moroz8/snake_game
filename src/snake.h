#pragma once
#include <deque>

const int BOARD_W = 40;
const int BOARD_H = 20;

enum class Dir { North, South, East, West };

struct Point {
    int x, y;
    bool operator==(const Point& o) const { return x == o.x && y == o.y; }
};

class Snake {
public:
    Snake();
    void reset();
    void setDirection(Dir d);

    // Returns true if the snake ate food this tick (caller should spawn new food)
    bool step(Point food);

    bool isDead() const { return dead_; }
    int  length() const { return static_cast<int>(body_.size()); }
    const std::deque<Point>& body() const { return body_; }
    Point head() const { return body_.front(); }

private:
    std::deque<Point> body_;
    Dir dir_;
    Dir nextDir_;
    bool dead_;
};
