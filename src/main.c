#include <stdio.h>
#include "solver.h"
#include "API.h"


// You do not need to edit this file.
// This program just runs your solver and passes the choices
// to the simulator.
int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    int success = 1;
    debug_log("Running...");
    initGrid();
    while (1) {
        Action nextMove = solver(success);
        switch(nextMove){
            case FORWARD:
                success = API_moveForward();
                break;
            case LEFT:
                success = API_turnLeft();
                break;
            case RIGHT:
                success = API_turnRight();
                break;
            case IDLE:
                break;
        }
    }
    // I know this is never going to happen, but just in case
    freeGrid();
    return 0;
}
