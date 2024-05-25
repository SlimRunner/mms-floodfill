#include "solver.h"
#include "API.h"
#include "vector.h"
#include "grid.h"

Action solver(int success) {
    return floodFillWalker(success);
    // return leftWallFollower(success);
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
    int walls;

    if (isRoundTrip()) {
        if (resumeTrip(API_wasReset())) {
            // REMOVE THIS IF STATEMENT WHEN RUNNING ON MICE
            API_ackReset();
            dir.x = 0;
            dir.y = 1;
        }
        return IDLE;
    }

    if (!success) {
        debug_log("CRASHED RECORDED");
        pos = prevPos;
    }

    if (updateCellState(pos, dir, &walls)) {
        API_setColor(pos.x, pos.y, 'B');
        fdebug_log(
            "\nAt (x: %d, y: %d),\n    compass: %c (x: %d, y: %d),\n    walls: 0x%x,\n    "
            , pos.x, pos.y, compass(flattenStdBasis(dir)), dir.x, dir.y, walls);
    } else {
        debug_log("Bad position");
    }
    prevPos = pos;
    return makeMove(&pos, &dir);
}
