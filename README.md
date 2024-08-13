# Micromouse
## Setup
You can find the [simulator here][Simulator]. The Makefile provides the `Build Command` of the simulator. You can simply call `make` instead of `gcc -o a.exe API.c main.c solver.c` and not only it is much more compact, but it also compiles only what changes (which makes compilation faster). As a suggestion you can place the simulator and this project in the same directory and if you do, you can use this as the `Run Command`: `..\mms-floodfill\build\release\app\maze-solver`. The make file guaratees that this is the output relative address of the executable. If you get errors in Windows add the ".exe" to the address, but this should normally not be an issue.

Additionally, the main extra components of the project are the following:

- `grid.h`: [the cpp of grid][grid-c] implements the buffer used to keep track of all the cells in the maze. Each cell is of a custom struct type called `CellState`. This is to streamline the process of adding properties to the cells, but be wary that each element in that `struct` is multiplied by the number of cells. For example, if the maze is 16x16, adding a new `int` entry will require `4 * 16 * 16 = 1024` extra bytes of memory. Finally, it also has the main BFS (breath first search) algorithm that finds the manhattan distances. It's main public interfaces are `updateCellState` and `makeMove`.
  - `updateCellState` calls the functions in charge of detecting walls and updates their state accordingly in the grid. Additionally, it automatically calls the BFS algorithm to update the floodfill.
  - `makeMove` should be called right after `updateCellState` to let the `makeMove` function return the correct `Action` to perform.
- `queue.h`: [the cpp queue][queue-c] implements the circular queue data structure. I initialize the array with `initQueue` which creates the provided required number of array elements. It can safely be called multiple times since it exits immediately, if the `queue` int pointer is non-zero. It uses a left and right node which wrap around the end of the array and correctly let you push and pop items.
- `vector.h`: [the cpp of vector][vector-c] implements a struct valled Vec2i which is nothing more than a 2D vector of ints. This allows to easily assign "hashes" to the locations in the Maze as well as easily convert between NORTH, EAST, WEST, and SOUTH to 2D direction vectors and viceversa.
- `enums.h`: [this is a header-only file][enums-h] that simply puts all the important enums in a single place. This allows me to include them where I need them without having to worry about polluting my global space and creating weird circular dependencies.

The rest of the files serve the same purpose as in the template. Further, because of these abstractions the final code that the solver function calls (`floodFillWalker`) is very compact.

<!-- links -->
[Simulator]: https://github.com/mackorone/mms?tab=readme-ov-file#mms- "Micromouse simulator"
[grid-c]: src/grid.c
[queue-c]: src/queue.c
[vector-c]: src/vector.c
[enums-h]: include/enums.h
