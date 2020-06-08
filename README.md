# Discription
Implementing UCS and A* search algorithms for optimal path between starting and finishing nodes of a randomly generated maze.

# Data Structure
A priority queue implemented as a linked list.

# Maze Generation
An implementation of a dynamically allocated n * n array representing the maze. Size (n) of maze is requested as input.\
Each cell is declared 0 (representing a wall) or 1 (representing a valid step) based on a requested probability (0 - 1).
Starting and finishing nodes/cells are also requested from the user.

# Running the code
Compile with any major version of gcc:
```
gcc -o maze maze.c
```
and run with:
```
./maze
