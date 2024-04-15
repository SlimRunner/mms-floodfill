#ifndef SOLVER_H
#define SOLVER_H

#include "grid.h"
#include "enums.h"

Action solver(int success);
Action leftWallFollower(int success);
Action floodFillWalker(int success);

#endif // SOLVER_H
