# Micromouse
## Setup
This project uses [the code templates][CodeTmpl] in the [assignment instructions][Assign7]. This project slightly deviates from the setup instructions which you can find [here for Windows][WinInstall], and [here for Mac OS or Linux][NixInstall]. The main difference is that I used a Makefile to make building the project more streamlined as I wanted to add my own headers. The Makefile is cross platform, but does not work out of the box on Mac OS (because I do not have any Mac to test on). Go to [Run it on Mac OS](#run-it-on-mac-os) section if you want to make it work there.

Anyway, the niceties that the Makefile provide is that in the `Build Command` of the simulator I can simply call `make` instead of `gcc -o a.exe API.c main.c solver.c` and not only it is much more compact, but it also compiles only what changes (which makes compilation faster). As a suggestion you can place the simulator and this project in the same directory and if you do, you can use this as the `Run Command`: `..\mms-floodfill\build\release\app\maze-solver.exe`. The make file guaratees that this is the output relative address of the executable. If you are in not in Windows remove the "exe" part and it works the same.

Additionally, the main extra components of the project are the following:

- `grid.h`: [the cpp of grid][grid-c] implements the buffer used to keep track of all the cells in the maze. Each cell is of a custom struct type called `CellState`. This is to streamline the process of adding properties to the cells, but be wary that each element in that `struct` is multiplied by the number of cells. For example, if the maze is 16x16, adding a new `int` entry will require `4 * 16 * 16 = 1024` extra bytes of memory. Finally, it also has the main BFS (breath first search) algorithm that finds the manhattan distances. It's main public interfaces are `updateCellState` and `makeMove`.
  - `updateCellState` calls the functions in charge of detecting walls and updates their state accordingly in the grid. Additionally, it automatically calls the BFS algorithm to update the floodfill.
  - `makeMove` should be called right after `updateCellState` to let the `makeMove` function return the correct `Action` to perform.
- `queue.h`: [the cpp queue][queue-c] implements the circular queue data structure. I initialize the array with `initQueue` which creates the provided required number of array elements. It can safely be called multiple times since it exits immediately, if the `queue` int pointer is non-zero. It uses a left and right node which wrap around the end of the array and correctly let you push and pop items.
- `vector.h`: [the cpp of vector][vector-c] implements a struct valled Vec2i which is nothing more than a 2D vector of ints. This allows to easily assign "hashes" to the locations in the Maze as well as easily convert between NORTH, EAST, WEST, and SOUTH to 2D direction vectors and viceversa.
- `enums.h`: [this is a header-only file][enums-h] that simply puts all the important enums in a single place. This allows me to include them where I need them without having to worry about polluting my global space and creating weird circular dependencies.

The rest of the files serve the same purpose as in the template. Further, because of these abstractions the final code that the solver function calls (`floodFillWalker`) is very compact.

## Run it on Mac OS

If you want to adapt the Makefile to work in Mac OS be my guest. Here is how do do it. When you go the Makefile you have these lines at the very top:

```Makefile
ifeq ($(OS),Windows_NT)
	detected_OS := Windows
else
	detected_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown')
endif

ifeq ($(detected_OS),Windows)
	RM = del /Q /S
	FixPath = $(subst /,\,$1)
	MD = if not exist $1 mkdir $1
	FixQuotes = $(subst /,\,$(subst ",,$1))
	EXT = .exe
endif
ifeq ($(detected_OS),Linux)
	RM = rm -rf
	FixPath = $1
	MD = mkdir -p $1
	FixQuotes = $1
endif
```
In here all you need to do is run `make info` in the terminal and see what it outputs in the entry
```sh
$ make info
# ...other output
[*] Detected OS: ...
```
Then you can create the following snippet right below the `ifeq` block that says linux. For example, if your Detected OS entry says something like `MacOS` then you do:

```Makefile
ifeq ($(detected_OS),Linux)
	# ...
endif
ifeq ($(detected_OS),MacOS)
	RM = rm -rf
	FixPath = $1
	MD = mkdir -p $1
	FixQuotes = $1
endif
```

Mac OS should have Linux compatible commands so there is no need to change what's inside the if-statement. If you manage to make this change feel free to open a PR and I will merge your changes. Alternatively, you can just share with me what the `make info` command outputs and I can update accordingly.

<!-- links -->
[Simulator]: https://github.com/mackorone/mms?tab=readme-ov-file#mms- "Micromouse simulator"
[Assign7]: https://docs.google.com/document/d/13vvfNG68HPjm_CHOhFpS4xBLJJHgJ9ge7JoztDkSK6A/edit "Assignment 7"
[WinInstall]: https://docs.google.com/presentation/d/1tr_2cUcouLl3fvlSyOth7XXo3jRbpBw1TfRX8yWRETY/edit?usp=sharing "Installation on Windows"
[NixInstall]: https://docs.google.com/presentation/d/18rHwcIJNPsIRkun7N9Wq5YYAiFSc-rkyNcAQBOEolg4/edit?usp=share_link "Installation on Mac OS / Linux"
[CodeTmpl]: https://drive.google.com/drive/folders/1VVWKLsxCGwiIFyi4MxaQTgTUnfjIyABy?usp=drive_link "Code templates"
[grid-c]: src/grid.c
[queue-c]: src/queue.c
[vector-c]: src/vector.c
[enums-h]: include/enums.h
