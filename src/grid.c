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
  int bfsVisited;
} CellState;

// this is used to "flip" the state of visited cells. It's purpose is to
// avoid clearing the whole grid each time we call the floodfill such
// that each time either the 0 or the 1 is interpreted as the "visited"
// state. This is more akin to double buffering in graphics.
int visitBit = 1;

static CellState **grid;
static WalkMode mmode = WM_EXPLORE;
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
      grid[y][x].bfsVisited = !visitBit;

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

static void updateWalls(Vec2i pos, int serial) {
  Vec2i next;
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
  const int WALL_MASK_PAIR[4] = {
    WALL_WEST , // right
    WALL_SOUTH, // up
    WALL_EAST , // left
    WALL_NORTH  // down
  };
  grid[pos.y][pos.x].walls = serial;
  for (int i = 0; i < 4; ++i) {
    next = addVectors(pos, NBS[i]);
    if (
      inRange(&next, LOW_BOUND, UPP_BOUND) &&
      grid[pos.y][pos.x].walls & WALL_MASK[i]
    ) {
      grid[next.y][next.x].walls |= WALL_MASK_PAIR[i];
    }
  }
}

int updateCellState(Vec2i pos, Vec2i dir, int *state) {
  if (inRange(&pos, LOW_BOUND, UPP_BOUND)) {
    const int wallFront = API_wallFront();
    const int wallLeft  = API_wallLeft();
    const int wallRight = API_wallRight();

    // TODO: make it so that this does not change further once the cell
    // has been visited by the mouse.
    *state = grid[pos.y][pos.x].walls;

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

    updateWalls(pos, *state);
    floodFill();
    return 1;
  }
  return 0;
}

static int isCloserToGoal(const Vec2i *const pos, const Vec2i *const dir, int *dist) {
  Vec2i there = addVectors(*dir, *pos);
  if (
    inRange(&there, LOW_BOUND, UPP_BOUND) &&
    *dist > grid[there.y][there.x].dist
  ) {
    *dist = grid[there.y][there.x].dist;
    return 1;
  }

  return 0;
}

static void cycleWalkMode() {
  switch (mmode) {
  case WM_EXPLORE:
    mmode = WM_RETURN;
    break;
  case WM_RETURN:
    mmode = WM_EXPLORE;
    break;
  case WM_SPEEDRUN:
    mmode = WM_SPEEDRUN;
    break;
  }
}

Action makeMove(Vec2i *pos, Vec2i *dir) {
  Vec2i ldir = getLeftRot(*dir);
  Vec2i rdir = getRightRot(*dir);
  Vec2i bdir = (Vec2i){-dir->x, -dir->y};
  int dirHash = flattenStdBasis(*dir);
  int lftHash = flattenStdBasis(ldir);
  int rgtHash = flattenStdBasis(rdir);
  int walls = grid[pos->y][pos->x].walls;
  int dist = grid[pos->y][pos->x].dist;
  int boxedIn = 1;
  Vec2i target;

  if (!dist) {
    cycleWalkMode();
    return IDLE;
  }

  fdebug_log("makeMove (dirhash: %d, rgtHash: %d, lftHash: %d)\n\n", dirHash,
             rgtHash, lftHash);

  // TODO: this algorithm goes forward unless it is at a dead end. If so
  // it looks right, then left. If it still cannot go there it just
  // turns around left. I still need to do the logic to choose the path
  // of greatest distance descent.
  if (
    !(walls & (1 << dirHash)) &&
    isCloserToGoal(pos, dir, &dist)
  ) {
    target = *dir;
    boxedIn = 0;
  }
  if (
    !(walls & (1 << rgtHash)) &&
    isCloserToGoal(pos, &rdir, &dist)
  ) {
    target = rdir;
    boxedIn = 0;
  }
  if (
    !(walls & (1 << lftHash)) &&
    isCloserToGoal(pos, &ldir, &dist)
  ) {
    target = ldir;
    boxedIn = 0;
  }
  if (
    boxedIn &&
    isCloserToGoal(pos, &bdir, &dist)
  ) {
    // there can never be a wall behind you unless you start boxed in
    // turn around
    target = bdir;
  }

  // to decide the move I will use a combination of the dot product and
  // the two-point-slope equation at the origin.
  // https://www.desmos.com/calculator/u9gmoingmu

  fdebug_log("%d, %d\n", target.x, target.y);
  fdebug_log("%d, %d\n", dir->x, dir->y);
  switch (dotProd(target, *dir)) {
  case 1:
    debug_log("FORWARD");
    advance(dir, pos);
    return FORWARD;
  case -1:
    // arbitrary decision when facing the opposite way
    debug_log("DEAD END -> LEFT");
    rotLeft(dir);
    return LEFT;
  default:
    // nothing to do
    // need to decide how to rotate
    break;
  }

  switch (getWinding(target, *dir)) {
  case 1:
    // go clockwise (positive rotation)
    debug_log("LEFT");
    rotLeft(dir);
    return LEFT;
  case -1:
    // go counter-clockwise (negative rotation)
    debug_log("RIGHT");
    rotRight(dir);
    return RIGHT;
  default:
    debug_log("This is bad\n");
    // this should technically never happen
    debug_log("IDLE");
    return IDLE;
  }
}

static int enqueue(int x, int y, int dist) {
  grid[y][x].dist = dist;
  grid[y][x].bfsVisited = visitBit;

  #ifdef SIM_API
  // 10 digits + 1 possible sign + null
  char cdist[12];
  sprintf(cdist, "%d", dist);
  API_setText(x, y, cdist);
  #endif

  return qpush(x + y * WIDTH);
}

static void floodFill() {
  initQueue(2 * (WIDTH + HEIGHT));

  // push goal onto queue
  if (mmode != WM_RETURN) {
    enqueue(MID_GOAL.x    , MID_GOAL.y    , 0);
    enqueue(MID_GOAL.x + 1, MID_GOAL.y    , 0);
    enqueue(MID_GOAL.x    , MID_GOAL.y + 1, 0);
    enqueue(MID_GOAL.x + 1, MID_GOAL.y + 1, 0);
  } else {
    enqueue(0, 0, 0);
  }

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
      next = addVectors(pos, NBS[i]);
      if (
        inRange(&next, LOW_BOUND, UPP_BOUND) &&
        grid[next.y][next.x].bfsVisited != visitBit &&
        !(here.walls & WALL_MASK[i])
      ) {
        enqueue(next.x, next.y, here.dist + 1);
      }
    }
  }

  visitBit = !visitBit;
}
