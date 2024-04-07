#include "solver.h"
#include "API.h"

struct vec2 {
    int x;
    int y;
};

Action solver() {
    return leftWallFollower();
}

void turnLeft(struct vec2 *dir) {
    int x = dir->x, y = dir->y;
    dir->x = -y;
    dir->y = x;
}

void turnRight(struct vec2 *dir) {
    int x = dir->x, y = dir->y;
    dir->x = y;
    dir->y = -x;
}

struct vec2 getLeftTurn(struct vec2 dir) {
    return (struct vec2){-dir.y, dir.x};
}

struct vec2 getRightTurn(struct vec2 dir) {
    return (struct vec2){dir.y, -dir.x};
}

void advance(const struct vec2 * const dir, struct vec2 *pos) {
    pos->x += dir->x;
    pos->y += dir->y;
}

int serializeDir(const struct vec2 dir) {
    // I derived the following matrix like this
    // https://www.desmos.com/calculator/tkyccfu43a

    // 3: E
    // 2: N
    // 0: W
    // 1: S
    return (3 * dir.x + dir.y + 3) / 2;
}

char compass(const struct vec2 dir) {
  switch (serializeDir(dir)) {
  case 3:
    return 'e';
  case 2:
    return 'n';
  case 0:
    return 'w';
  case 1:
    return 's';
  default:
    return '\0';
  }
}

// This is an example of a simple left wall following algorithm.
Action leftWallFollower() {
    static struct vec2 pos = {0, 0};
    static struct vec2 dir = {0, 1};
    static int goLeft = 0;

    Action outState;

    dblog("x: %d, y: %d, compass: %c (x: %d, y: %d)\n", pos.x, pos.y, compass(dir), dir.x, dir.y);

    if (goLeft) {
        advance(&dir, &pos);
        outState = FORWARD;
        goLeft = 0;
        return outState;
    }

    if(API_wallLeft()) {
        API_setWall(pos.x, pos.y, compass(getLeftTurn(dir)));
        if(API_wallFront()) {
            turnRight(&dir);
            outState = RIGHT;
        } else {
            advance(&dir, &pos);
            outState = FORWARD;
        }
    } else {
        turnLeft(&dir);
        outState = LEFT;
        goLeft = 1;
    }

    return outState;
}


// Put your implementation of floodfill here!
Action floodFill() {
    return IDLE;
}
