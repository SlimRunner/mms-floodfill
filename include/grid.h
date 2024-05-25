#ifndef GRID_H
#define GRID_H

#include "vector.h"
#include "enums.h"

/// @brief Initializes a 2D array for the maze
void initGrid();
/// @brief Frees memory initialized for the 2D array
void freeGrid();

/// @brief Determines where are walls persent in given cell
/// @param x x-coordinate of the cell
/// @param y y-coordinate of the cell
/// @param state bitfield that represents present walls
/// @return non-zero if operation was successful
int getCellState(Vec2i pos, int * state);

int updateCellState(Vec2i pos, Vec2i dir, int * state);

Action makeMove(Vec2i *pos, Vec2i *dir);

int isRoundTrip();

int resumeTrip(int go);

#endif // GRID_H
