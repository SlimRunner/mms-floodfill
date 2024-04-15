#include "grid.h"
#include "enums.h"
#include "vector.h"
#include "queue.h"

#include <stdlib.h>

#define SIM_API

#ifdef SIM_API
  #include "API.h"
  #include "stdio.h"
#endif

static void floodFill();

typedef struct CellState {
  int walls;
  int dist;
  int visited;
} CellState;

// this is used to "flip" the state of visited cells. It's purpose is to
// avoid clearing the whole grid each time we call the floodfill such
// that each time either the 0 or the 1 is interpreted as the "visited"
// state. This is more akin to double buffering in graphics.
int visitBit = 1;

static CellState **grid;
static Vec2i LOW_BOUND = {0, 0};
static Vec2i UPP_BOUND = {-1, -1};
static Vec2i MID_GOAL = {-1, -1};
static int WIDTH = -1;
static int HEIGHT = -1;

void initGrid() {
  if (grid) return;

  WIDTH = API_mazeWidth();
  HEIGHT = API_mazeHeight();
  UPP_BOUND.x = WIDTH - 1;
  UPP_BOUND.y = HEIGHT - 1;
  MID_GOAL.x = UPP_BOUND.x / 2;
  MID_GOAL.y = UPP_BOUND.y / 2;
  grid = malloc(HEIGHT * sizeof(CellState *));
  for (int y = 0; y < HEIGHT; ++y) {
    grid[y] = malloc(WIDTH * sizeof(CellState));
    for (int x = 0; x < WIDTH; ++x) {
      grid[y][x].walls = (
                        !x * WALL_WEST  |
                        !y * WALL_SOUTH |
        !(UPP_BOUND.y - y) * WALL_NORTH |
        !(UPP_BOUND.x - x) * WALL_EAST
      );
      grid[y][x].visited = !visitBit;

      #ifdef SIM_API
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
      #endif
    }
  }
}

void freeGrid() {
  if (!grid) return;

  for (int y = 0; y < HEIGHT; ++y) {
    free(grid[y]);
  }
  free(grid);
  grid = NULL;
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
      const int serial = flattenStdBasis(dir);
      #ifdef SIM_API
      API_setWall(pos.x, pos.y, compass(serial));
      #endif
      *state |= 1 << serial;
    }

    if (wallLeft) {
      const Vec2i dirL = getLeftRot(dir);
      const int serial = flattenStdBasis(dirL);
      #ifdef SIM_API
      API_setWall(pos.x, pos.y, compass(serial));
      #endif
      *state |= 1 << serial;
    }

    if (wallRight) {
      const Vec2i dirR = getRightRot(dir);
      const int serial = flattenStdBasis(dirR);
      #ifdef SIM_API
      API_setWall(pos.x, pos.y, compass(serial));
      #endif
      *state |= 1 << serial;
    }

    grid[pos.y][pos.x].walls = *state;
    floodFill();
    return 1;
  }
  return 0;
}

static int enqueue(int x, int y, int dist) {
  grid[y][x].dist = dist;
  grid[y][x].visited = visitBit;

  #ifdef SIM_API
  // 10 digits + 1 possible sign + null
  char cdist[12];
  sprintf(cdist, "%d", dist);
  API_setText(x, y, cdist);
  #endif

  return qpush(x + y * WIDTH);
}

static void floodFill() {
  // create a buffer of fixed size (W*H for now we can reduce it later)
  // which is going to have three properties: leftnode, rightnode, and
  // size. If leftnode == rightnode, size == 0. This will work as a
  // queue for BFS. With this I do not have to dynamically allocate
  // memory while traversing the grid. Additionally, if leftnode ==
  // rightnode but size != 0 then we have overflow.
  initQueue(WIDTH * HEIGHT, sizeof(int));

  // push goal onto queue
  enqueue(MID_GOAL.x    , MID_GOAL.y    , 0);
  enqueue(MID_GOAL.x + 1, MID_GOAL.y    , 0);
  enqueue(MID_GOAL.x    , MID_GOAL.y + 1, 0);
  enqueue(MID_GOAL.x + 1, MID_GOAL.y + 1, 0);

  CellState here;
  int qidx;
  Vec2i pos, next;
  const Vec2i NBS[4] = {
    { 1,  0}, // right
    { 0,  1}, // up
    {-1,  0}, // left
    { 0, -1}  // down
  };
  const int WALL_MASK[4] = {
    WALL_EAST , // right
    WALL_NORTH, // up
    WALL_WEST , // left
    WALL_SOUTH  // down
  };

  while (qpop(&qidx)) {
    pos = deserialize(qidx, WIDTH);
    here = grid[pos.y][pos.x];
    for (int i = 0; i < 4; ++i) {
      next = getMove(pos, NBS[i]);
      if (
        inRange(&next, LOW_BOUND, UPP_BOUND) &&
        !(here.walls & WALL_MASK[i]) &&
        grid[next.y][next.x].visited != visitBit
      ) {
        enqueue(next.x, next.y, here.dist + 1);
      }
    }
  }

  visitBit = !visitBit;
  return;
}
