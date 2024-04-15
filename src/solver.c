#include "solver.h"
#include "API.h"
#include "vector.h"
#include "grid.h"

Action solver(int success) {
    return floodFillWalker(success);
    // return leftWallFollower(crashed);
}

// This is an example of a simple left wall following algorithm.
Action leftWallFollower(int success) {
    static Vec2i prevPos = {0, 0};
    static Vec2i pos = {0, 0};
    static Vec2i dir = {0, 1};
    static int goLeft = 0;

    if (!success) {
        pos = prevPos;
    }

    Action outState;

    fdebug_log("x: %d, y: %d, compass: %c (x: %d, y: %d)\n", pos.x, pos.y, compass(flattenStdBasis(dir)), dir.x, dir.y);

    if (goLeft) {
        advance(&dir, &pos);
        goLeft = 0;
        outState = FORWARD;
        return outState;
    }

    if(API_wallLeft()) {
        API_setWall(pos.x, pos.y, compass(flattenStdBasis(getLeftRot(dir))));
        if(API_wallFront()) {
            rotRight(&dir);
            outState = RIGHT;
        } else {
            prevPos = pos;
            advance(&dir, &pos);
            outState = FORWARD;
        }
    } else {
        rotLeft(&dir);
        outState = LEFT;
        goLeft = 1;
    }

    return outState;
}


// Put your implementation of floodfill here!
Action floodFillWalker(int success) {
    static Vec2i prevPos = {0, 0};
    static Vec2i pos = {0, 0};
    static Vec2i dir = {0, 1};
    int state;

    if (!success) {
        pos = prevPos;
    }

    if (updateCellState(pos, dir, &state)) {
        fdebug_log("state: %d\n", state);
    } else {
        debug_log("Bad position");
    }
    prevPos = pos;
    // advance(&dir, &pos);
    // return FORWARD;
    rotRight(&dir);
    return RIGHT;
}
