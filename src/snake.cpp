#include "snake.h"

Snake::Snake() { reset(); }

void Snake::reset() {
    body_.clear();
    int midX = BOARD_W / 2;
    int midY = BOARD_H / 2;
    // Head at midX facing west, body extends to the right
    body_.push_back({midX,     midY});  // head
    body_.push_back({midX + 1, midY});
    body_.push_back({midX + 2, midY});  // tail
    dir_     = Dir::West;
    nextDir_ = Dir::West;
    dead_    = false;
}

void Snake::setDirection(Dir d) {
    // Prevent 180-degree reversals
    if (d == Dir::North && dir_ == Dir::South) return;
    if (d == Dir::South && dir_ == Dir::North) return;
    if (d == Dir::East  && dir_ == Dir::West)  return;
    if (d == Dir::West  && dir_ == Dir::East)  return;
    nextDir_ = d;
}

bool Snake::step(Point food) {
    dir_ = nextDir_;

    Point head = body_.front();
    switch (dir_) {
        case Dir::North: head.y--; break;
        case Dir::South: head.y++; break;
        case Dir::East:  head.x++; break;
        case Dir::West:  head.x--; break;
    }

    // Wall collision — the playfield is bordered, walls are at 0 and W/H-1
    if (head.x <= 0 || head.x >= BOARD_W - 1 ||
        head.y <= 0 || head.y >= BOARD_H - 1) {
        dead_ = true;
        return false;
    }

    // Self collision — exclude the tail since it moves away this tick
    for (int i = 0; i < static_cast<int>(body_.size()) - 1; ++i) {
        if (body_[i] == head) {
            dead_ = true;
            return false;
        }
    }

    body_.push_front(head);

    bool ate = (head == food);
    if (!ate) body_.pop_back();  // No growth — remove tail
    return ate;
}
