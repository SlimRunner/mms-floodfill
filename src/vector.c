#include "vector.h"
#include "enums.h"

void rotLeft(Vec2i *vec) {
    int x = vec->x, y = vec->y;
    vec->x = -y;
    vec->y = x;
}

void rotRight(Vec2i *vec) {
    int x = vec->x, y = vec->y;
    vec->x = y;
    vec->y = -x;
}

Vec2i getLeftRot(Vec2i vec) {
    return (Vec2i){-vec.y, vec.x};
}

Vec2i getRightRot(Vec2i vec) {
    return (Vec2i){vec.y, -vec.x};
}

void advance(const Vec2i * const dir, Vec2i *pos) {
    pos->x += dir->x;
    pos->y += dir->y;
}

int serializeVec(const Vec2i vec) {
    // I derived the following matrix like this
    // https://www.desmos.com/calculator/ecuq826hkn

    // 0: W
    // 1: S
    // 2: N
    // 3: E
    return (3 * vec.x + vec.y + 3) / 2;
}

char compass(int dirSerial) {
  switch (dirSerial) {
  case WEST:
    return CHAR_WEST;
  case SOUTH:
    return CHAR_SOUTH;
  case NORTH:
    return CHAR_NORTH;
  case EAST:
    return CHAR_EAST;
  default:
    return '\0';
  }
}

int inRange(const Vec2i * const dir, Vec2i low, Vec2i high) {
  return (
    low.x <= dir->x && dir->x <= high.x &&
    low.y <= dir->y && dir->y <= high.y
  );
}
