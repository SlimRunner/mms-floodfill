#include "grid.h"
#include "API.h"
#include "enums.h"
#include "vector.h"

#include <stdlib.h>

static void floodFill();

typedef struct CellState {
  int weight;
  int walls;
} CellState;

static CellState **grid;
static Vec2i LOW_BOUND = {0, 0};
static Vec2i UPP_BOUND = {-1, -1};
static int WIDTH = -1;
static int HEIGHT = -1;
static int initialized = 0;

void initGrid() {
  (void)floodFill; // REMOVE LATER
  if (initialized) return;

  WIDTH = API_mazeWidth();
  HEIGHT = API_mazeHeight();
  UPP_BOUND.x = WIDTH - 1;
  UPP_BOUND.y = HEIGHT - 1;
  grid = (CellState **)malloc(sizeof(CellState) * HEIGHT);
  for (int y = 0; y < HEIGHT; ++y) {
    grid[y] = (CellState *)malloc(sizeof(CellState) * WIDTH);
    for (int x = 0; x < WIDTH; ++x) {
      grid[y][x].walls = (
                        !x * WALL_WEST  |
                        !y * WALL_SOUTH |
        !(UPP_BOUND.y - y) * WALL_NORTH |
        !(UPP_BOUND.x - x) * WALL_EAST
      );
      if (grid[y][x].walls & WALL_WEST) {
        API_setWall(x, y, 'w');
      }
      if (grid[y][x].walls & WALL_SOUTH) {
        API_setWall(x, y, 's');
      }
      if (grid[y][x].walls & WALL_NORTH) {
        API_setWall(x, y, 'n');
      }
      if (grid[y][x].walls & WALL_EAST) {
        API_setWall(x, y, 'e');
      }
    }
  }
  initialized = 1;
}

void terminateGrid() {
  if (!initialized) return;

  for (int y = 0; y < HEIGHT; ++y) {
    free(grid[y]);
  }
  free(grid);
  initialized = 0;
}

int getCellState(Vec2i pos, int * state) {
  if (inRange(&pos, LOW_BOUND, UPP_BOUND)) {
    *state = grid[pos.y][pos.x].walls;
    return 1;
  }
  return 0;
}

int updateCellState(Vec2i pos, Vec2i dir, int *state) {
  if (inRange(&pos, LOW_BOUND, UPP_BOUND)) {
    const int wallFront = API_wallFront();
    const int wallLeft  = API_wallLeft();
    const int wallRight = API_wallRight();

    *state = 0;

    if (wallFront) {
      const int serial = serializeVec(dir);
      API_setWall(pos.x, pos.y, compass(serial));
      *state |= 1 << serial;
    }

    if (wallLeft) {
      const Vec2i dirL = getLeftRot(dir);
      const int serial = serializeVec(dirL);
      API_setWall(pos.x, pos.y, compass(serial));
      *state |= 1 << serial;
    }

    if (wallRight) {
      const Vec2i dirR = getRightRot(dir);
      const int serial = serializeVec(dirR);
      API_setWall(pos.x, pos.y, compass(serial));
      *state |= 1 << serial;
    }

    grid[pos.y][pos.x].walls = *state;
    return 1;
  }
  return 0;
}

static void floodFill() {
  // create a buffer of fixed size (W*H for now we can reduce it later)
  // which is going to have three properties: leftnode, rightnode, and
  // size. If leftnode == rightnode, size == 0. This will work as a
  // queue for BFS. With this I do not have to dynamically allocate
  // memory while traversing the grid. Additionally, if leftnode ==
  // rightnode but size != 0 then we have overflow.
  return;
}
