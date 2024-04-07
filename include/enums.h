#ifndef ENUMS_H
#define ENUMS_H

typedef enum Action {
  LEFT,
  FORWARD, 
  RIGHT,
  IDLE
} Action;

typedef enum Heading {
  WEST,
  SOUTH,
  NORTH,
  EAST
} Heading;

typedef enum HeadingChar {
  CHAR_WEST ='w',
  CHAR_SOUTH = 's',
  CHAR_NORTH = 'n',
  CHAR_EAST = 'e'
} HeadingChar;

typedef enum Walls {
  WALL_NONE = 0,
  WALL_WEST = 1 << WEST,
  WALL_SOUTH = 1 << SOUTH,
  WALL_NORTH = 1 << NORTH,
  WALL_EAST = 1 << EAST
} Walls;

#endif // ENUMS_H