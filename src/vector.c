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

Vec2i getMove(Vec2i p1 , Vec2i p2) {
  return (Vec2i){p2.x + p1.x, p2.y + p1.y};
}

int flattenStdBasis(const Vec2i vec) {
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

int serialize(const Vec2i * const dir, int width) {
  return dir->x + dir->y * width;
}

Vec2i deserialize(int hash, int width) {
  return (Vec2i){hash % width, hash / width};
}
