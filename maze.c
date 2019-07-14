#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

typedef struct coordinates_s{

	int x, y;
}coordinates_t;

typedef struct node_s {

	int priority;			// g(n) in UCS, f(n) in A*. Higher priority comes with lower values.
	coordinates_t cell;		// Nodes place in the maze.
	struct node_s *next;
}node_t;

// -----------------------------------
// 			Priority Queue
// -----------------------------------

node_t *new_node(coordinates_t new_cell, int new_priority) {

	node_t *new = NULL;

	new = (node_t *)malloc(sizeof(node_t));
	if (new == NULL) {

		printf("Trouble allocating memory for new node.\n");
		exit(1);
	}

	new -> cell = new_cell;
	new -> priority = new_priority;
	new -> next = NULL;

	return new;
}

void push(node_t **head, coordinates_t new_cell, int new_priority) {

	node_t *start = (*head);	// 'head' represents the last pushed node.

	node_t *temp = new_node(new_cell, new_priority);

	// If the head of the list has lesser priority than the new node,
	// insert the new node before head and change head node.
	if ((*head) -> priority > new_priority) {

		temp -> next = *head;
		(*head) = temp;
	}
	else {
		// Traverse the list to find new nodes appropriate position (based on the priority of each node).
		while (start -> next != NULL && start -> next -> priority < new_priority) {

			start = start -> next;
		}

		temp -> next = start -> next;
		start -> next = temp;
	}
}

void pop(node_t **head) {

	node_t *temp = *head;
	(*head) = (*head) -> next;

	free(temp);
}

coordinates_t peek(node_t **head) {	// Return heads coordinates.

	return (*head) -> cell;
}

int is_empty(node_t **head) {

	return (*head) == NULL;
}

// -----------------------------------
// 				Maze
// -----------------------------------

void get_maze_specs(int *maze_size, double *p) {

	printf("Please, insert maze size: ");
	scanf("%d", maze_size);

	printf("Please, insert cell vecancy probability (0 to 1): ");
	scanf("%lf", p);

	if (*p < 0 || *p > 1) {

		printf("Vecancy probability should be greater than zero and less than one (0 <= p <= 1).\n");
		exit(1);
	}

	return;
}

int **create_maze(int maze_size) {

	int i;
	int **maze = NULL;

	maze = (int **)malloc(maze_size * sizeof(int*));
	if (maze == NULL) {

		printf("Trouble allocating memory for maze.\n");
		exit(1);
	}

	for(i = 0; i < maze_size; i++) {

		maze[i] = (int *)malloc(maze_size * sizeof(int));
		if (maze[i] == NULL) {

			printf("Trouble allocating memory for maze (column: %d).\n", i);
			exit(1);
		}
	}

	return maze;
}

void get_coordinates(coordinates_t *S, coordinates_t *G1, coordinates_t *G2, int maze_size) {

	printf("\nStarting cell:\n");
	while (1) {

		printf("Please, insert x coordinate: ");
		scanf("%d", &(S->x));

		if (S -> x > maze_size - 1 || S -> x < 0) {

			printf("Improper value. 0 <= x < %d\n", maze_size);
			continue;
		}
		else { break; }
	}

	while (1) {

		printf("Please, insert y coordinate: ");
		scanf("%d", &(S->y));

		if (S -> y > maze_size - 1 || S -> y < 0) {

			printf("Improper value. 0 <= y < %d\n", maze_size);
			continue;
		}
		else { break; }
	}

	printf("\nExit cell 1:\n");
	while (1) {

		printf("Please, insert x coordinate: ");
		scanf("%d", &(G1->x));

		if (G1 -> x > maze_size - 1 || G1 -> x < 0) {

			printf("Improper value. 0 <= x < %d\n", maze_size);
			continue;
		}
		else { break; }
	}

	while (1) {

		printf("Please, insert y coordinate: ");
		scanf("%d", &(G1->y));

		if (G1 -> y > maze_size - 1 || G1 -> y < 0) {

			printf("Improper value. 0 <= y < %d\n", maze_size);
			continue;
		}
		else { break; }
	}

	printf("\nExit cell 2:\n");
	while (1) {

		printf("Please, insert x coordinate: ");
		scanf("%d", &(G2->x));

		if (G2 -> x > maze_size - 1 || G2 -> x < 0) {

			printf("Improper value. 0 <= x < %d\n", maze_size);
			continue;
		}
		else { break; }
	}

	while (1) {

		printf("Please, insert y coordinate: ");
		scanf("%d", &(G2->y));

		if (G2 -> y > maze_size - 1 || G2 -> y < 0) {

			printf("Improper value. 0 <= y < %d\n", maze_size);
			continue;
		}
		else { break; }
	}

	return;
}

void set_maze_layout(int **maze, int maze_size, double p, coordinates_t S, coordinates_t G1, coordinates_t G2) {

	/*	Fills each maze cell with a 0 or a 1 based on a given probability.
		If a cell is 0, it represents an obstacle and the agent can't pass through.
		if a cell is 1, it's free.
	*/
	int random_probability_i;
	double random_probability_d;
	int i, j;
	
	srand(time(NULL));

	for (i = 0; i < maze_size; i++) {
		for (j = 0; j < maze_size; j++) {

			random_probability_i = rand() % 100;
			random_probability_d = (double)random_probability_i / 100;

			if (random_probability_d < p) {

				maze[i][j] = 1;
			}
			else {

				maze[i][j] = 0;
			}
		}
	}

	if (maze[S.x][S.y] == 0) {

		maze[S.x][S.y] = 1;
	}

	if (maze[G1.x][G1.y] == 0) {

		maze[G1.x][G1.y] = 1;
	}

	if (maze[G2.x][G2.y] == 0) {

		maze[G2.x][G2.y] = 1;
	}

	return;
}

void print_maze(int **maze, int maze_size) {

	int i, j;

	for (i = 0; i < maze_size; i++) {
		for (j = 0; j < maze_size; j++) {
			
			printf("%d ", maze[i][j]);
		}

		printf("\n");
	}
}

int find_min(int **maze, int **cell_priority, int maze_size, int i_index, int j_index) {		// Find the minimum cost of neighboring cells.

	int min_cost = maze_size;

	// Cell above.
	if (i_index != 0) {

		if (cell_priority[i_index - 1][j_index] != 0 && cell_priority[i_index - 1][j_index] < min_cost) {

			min_cost = cell_priority[i_index - 1][j_index];
		}
	}

	// Cell below.
	if (i_index != maze_size - 1) {

		if (cell_priority[i_index + 1][j_index] != 0 && cell_priority[i_index + 1][j_index] < min_cost) {

			min_cost = cell_priority[i_index + 1][j_index];
		}
	}

	// Cell to the right.
	if (j_index != maze_size - 1) {

		if (cell_priority[i_index][j_index + 1] != 0 && cell_priority[i_index][j_index + 1] < min_cost) {

			min_cost = cell_priority[i_index][j_index + 1];
		}
	}

	// Cell to the left.
	if (j_index != 0) {

		if (cell_priority[i_index][j_index - 1] != 0 && cell_priority[i_index][j_index - 1] < min_cost) {

			min_cost = cell_priority[i_index][j_index - 1];
		}
	}

	// Cell to the upper right.
	if (i_index != 0 && j_index != maze_size - 1) {

		if (cell_priority[i_index - 1][j_index + 1] != 0 && cell_priority[i_index - 1][j_index + 1] < min_cost) {

			min_cost = cell_priority[i_index - 1][j_index + 1];
		}
	}

	// Cell to the bottom right.
	if (i_index != maze_size - 1 && j_index != maze_size - 1) {

		if (cell_priority[i_index + 1][j_index + 1] != 0 && cell_priority[i_index + 1][j_index + 1] < min_cost) {

			min_cost = cell_priority[i_index + 1][j_index + 1];
		}
	}

	// Cell to the bottom left.
	if (i_index != maze_size - 1 && j_index != 0) {

		if (cell_priority[i_index + 1][j_index - 1] != 0 && cell_priority[i_index + 1][j_index - 1] < min_cost) {

			min_cost = cell_priority[i_index + 1][j_index - 1];
		}
	}

	// Cell to the upper left.
	if (i_index != 0 && j_index != 0) {

		if (cell_priority[i_index - 1][j_index - 1] != 0 && cell_priority[i_index - 1][j_index - 1] < min_cost) {

			min_cost = cell_priority[i_index - 1][j_index - 1];
		}
	}

	return min_cost;
}

int **assign_cost(int **maze, int maze_size, coordinates_t S) {		// Assign the g(n) value of every free maze cell.

	int i, i_index, j_index;
	int **cell_priority = NULL;
	int cost = 0, min_neighboring_cost = 0;

	cell_priority = (int **)malloc((maze_size * maze_size) * sizeof(int *));
	if (cell_priority == NULL) {

		printf("Trouble allocating memory for cell_priority array.\n");
		exit(1);
	}

	for (i = 0; i < maze_size; i++) {

		cell_priority[i] = (int *)malloc(maze_size * sizeof(int));
		if (cell_priority[i] == NULL) {

			printf("Trouble allocating memory for cell_priority[%d].\n", i);
			exit(1);
		}
	}

	if (S.x != 0 && S.y != maze_size) {		// Up right diagonal.

		i_index = S.x - 1;
		j_index = S.y + 1;
		cost = 1; 

		cell_priority[i_index][j_index] = 0;

		while (i_index >= 0 && j_index < maze_size) {

			if (maze[i_index][j_index] == 1) {

				(cell_priority[i_index][j_index]) = cost;

			}
			cost++;
			i_index--;
			j_index++;
		}
	}

	if (S.x != maze_size && S.y != maze_size) {		// Down right diagonal.

		i_index = S.x + 1;
		j_index = S.y + 1;
		cost = 1;

		while (i_index < maze_size && j_index < maze_size) {

			if (maze[i_index][j_index] == 1) {

				(cell_priority[i_index][j_index]) = cost;
			}
			cost++;
			i_index++;
			j_index++;
		}
	}
	
	if (S.x != maze_size && S.y != 0) {		// Down left diagonal.

		i_index = S.x + 1;
		j_index = S.y - 1;
		cost = 1;

		while (i_index < maze_size - 1 && j_index >= 0) {

			if (maze[i_index][j_index] == 1) {

				(cell_priority[i_index][j_index]) = cost;
			}		
			cost++;
			i_index++;
			j_index--;
		}
	}

	if (S.x != 0 && S.y != 0) {		// Up left diagonal.

		i_index = S.x - 1;
		j_index = S.y - 1;
		cost = 1;

		while (i_index >= 0 && j_index >= 0) {

			if (maze[i_index][j_index] == 1) {

				(cell_priority[i_index][j_index]) = cost;
			}
			cost++;
			i_index--;
			j_index--;
		}
	}

	if (S.x != 0) {				// Up.

		i_index = S.x - 1;
		j_index = S.y;
		cost = 1;

		while (i_index >= 0) {

			if (maze[i_index][j_index] == 1) {

				(cell_priority[i_index][j_index]) = cost;
			}
			cost++;
			i_index--;
		}
	}

	if (S.x != maze_size) {		// Down.

		i_index = S.x + 1;
		j_index = S.y;
		cost = 1;

		while (i_index < maze_size) {

			if (maze[i_index][j_index] == 1) {

				(cell_priority[i_index][j_index]) = cost;
			}
			cost++;
			i_index++;
		}
	}

	if (S.y != 0) {		// Left.

		i_index = S.x;
		j_index = S.y - 1;
		cost = 1;

		while (j_index >= 0) {

			if (maze[i_index][j_index] == 1) {

				(cell_priority[i_index][j_index]) = cost;
			}
			cost++;
			j_index--;
		}
	}

	if (S.y != maze_size) {		// Right.

		i_index = S.x;
		j_index = S.y + 1;
		cost = 1;

		while (j_index < maze_size) {

			if (maze[i_index][j_index] == 1) {

				(cell_priority[i_index][j_index]) = cost;
			}
			cost++;
			j_index++;
		}
	}

	for (i_index = 0; i_index < S.x; i_index++) {					// Upper right quarter.
		for (j_index = S.y + 1; j_index < maze_size; j_index++) {

			if (maze[i_index][j_index] == 1 && cell_priority[i_index][j_index] == 0) {

				min_neighboring_cost = find_min(maze, cell_priority, maze_size, i_index, j_index);
				cell_priority[i_index][j_index] = min_neighboring_cost + 1;
				//printf("Assigning cell_priority[%d][%d] to %d.\n", i_index, j_index, cell_priority[i_index][j_index]);
			}
		}
	}

	for (i_index = 0; i_index < S.x; i_index++) {					// Upper left quarter.
		for (j_index = S.y - 1; j_index >= 0; j_index--) {

			if (maze[i_index][j_index] == 1 && cell_priority[i_index][j_index] == 0) {

				min_neighboring_cost = find_min(maze, cell_priority, maze_size, i_index, j_index);
				cell_priority[i_index][j_index] = min_neighboring_cost + 1;
				//printf("Assigning cell_priority[%d][%d] to %d.\n", i_index, j_index, cell_priority[i_index][j_index]);
			}
		}
	}

	for (i_index = S.x + 1; i_index < maze_size; i_index++) {		// Bottom right quarter.
		for (j_index = S.y + 1; j_index < maze_size; j_index++) {

			if (maze[i_index][j_index] == 1 && cell_priority[i_index][j_index] == 0) {

				min_neighboring_cost = find_min(maze, cell_priority, maze_size, i_index, j_index);
				cell_priority[i_index][j_index] = min_neighboring_cost + 1;
				//printf("Assigning cell_priority[%d][%d] to %d.\n", i_index, j_index, cell_priority[i_index][j_index]);
			}
		}
	}

	for (i_index = S.x + 1; i_index < maze_size; i_index++) {		// Bottom left quarter.
		for (j_index = S.y - 1; j_index >= 0; j_index--) {

			if (maze[i_index][j_index] == 1 && cell_priority[i_index][j_index] == 0) {

				min_neighboring_cost = find_min(maze, cell_priority, maze_size, i_index, j_index);
				cell_priority[i_index][j_index] = min_neighboring_cost + 1;
				//printf("Assigning cell_priority[%d][%d] to %d.\n", i_index, j_index, cell_priority[i_index][j_index]);
			}
		}		
	}
	/*
	printf("Cell Priority:\n");

	for (i_index = 0; i_index < maze_size; i_index++) {
		for (j_index = 0; j_index < maze_size; j_index++) {

			printf("%d ", cell_priority[i_index][j_index]);
		}
		printf("\n");
	}
	*/
	return cell_priority;
}

// -----------------------------------
// 	  UCS, A* utility functions
// -----------------------------------

void print_frontier(node_t **search_frontier) {

	node_t *head = (*search_frontier);

	printf("Search Frontier: \n");
	while (head -> next != NULL) {

		printf("[Priority: %d] [%d][%d]\n", head -> priority, (head -> cell).x, (head -> cell).y);
		head = head -> next;
	}

	//printf("[Priority: %d] [%d][%d]\n", head -> priority, (head -> cell).x, (head -> cell).y);
}

int check_frontier(node_t **search_frontier, coordinates_t current_coordinates) {

	node_t *head = (*search_frontier);

	while (head -> next != NULL) {

		if ((head -> cell).x == current_coordinates.x && (head -> cell).y == current_coordinates.y) {

			//printf("[%d][%d] is part of the Search Frontier.\n", (head -> cell).x, (head -> cell).y);
			return 1;
		}

		head = head -> next;
	}

	return 0;
}

int is_goal(coordinates_t current_node, coordinates_t G1, coordinates_t G2) {

	if (current_node.x == G1.x && current_node.y == G1.y) {

		return 1;
	}
	else if (current_node.x == G2.x && current_node.y == G2.y) {

		return 2;
	}

	return 0;
}

int is_visited(coordinates_t current_node, coordinates_t *visited, int visited_count) {

	int i;

	for (i = 0; i < visited_count; i++) {

		if (current_node.x == (visited[i]).x && current_node.y == (visited[i]).y) {

			return 1;
		}
	}

	return 0;
}

void push_visited(coordinates_t current_node, coordinates_t **visited, int *visited_count) {

	(*visited)[(*visited_count)] = current_node;
	(*visited_count)++;
}

void print_path(coordinates_t *path, int path_count) {

	int i;

	printf("Path:\n");

	for (i = 0; i < path_count; i++) {

		if (i > 0 && i % 5 == 0) {

			printf("\n");
		}

		if (i == path_count - 1) {

			printf("[%d][%d]\n", (path[i]).x, (path[i]).y);
			break;
		}

		printf("[%d][%d] -> ", (path[i]).x, (path[i]).y);
	}

	printf("Path cost: %d.\n", path_count - 1);
}

int heuristic(coordinates_t current_coordinates, coordinates_t goal_coordinates) {

	int dx, dy;
	int D = 1;		// Minimum cost for moving from one node to an adjacent node (moving in 4 directions, not counting diagonal movement).
	int D2 = 1;		// Minimum cost for moving from one node to an adjacent node diagonally.
	int min;

	dx = abs(current_coordinates.x - goal_coordinates.x);
	dy = abs(current_coordinates.y - goal_coordinates.y);

	if (dx < dy) { min = dx; }
	else { min = dy; }

	return D * (dx + dy) + (D2 - (2 * D)) * min;
}

int min_heuristic(int h1, int h2) {

	if (h1 < h2) { return h1; }

	return h2;
}

// -----------------------------------
// 		  Uniform Cost Search
// -----------------------------------

void ucs(int **maze, int maze_size, coordinates_t S, coordinates_t G1, coordinates_t G2) {

	node_t *search_frontier = NULL;
	coordinates_t *visited = NULL;						// List of already expanded nodes, in other words, "closed set".
	coordinates_t *path = NULL, *new_path = NULL;
	coordinates_t current_coordinates, new_coordinates;
	int **priority = NULL;		// The g(n) value of each node.
	int visited_count = 0;
	int expansion_count = 0, path_count = 0;

	search_frontier = new_node(S, 0);

	priority = assign_cost(maze, maze_size, S);

	visited = (coordinates_t *)malloc((maze_size * maze_size) * sizeof(coordinates_t));
	if (visited == NULL) {

		printf("[UCS] Trouble allocating memory for 'visited' list.\n");
		exit(1);
	}

	path = (coordinates_t *)malloc(sizeof(coordinates_t));
	if (path == NULL) {

		printf("[UCS] Trouble allocating memory for 'path' array.\n");
		exit(1);
	}

	while (!is_empty(&search_frontier)) {

		current_coordinates = peek(&search_frontier);

		if (is_visited(current_coordinates, visited, visited_count)) {

			pop(&search_frontier);
			continue;
		}

		path[path_count] = current_coordinates;
		path_count++;
		new_path = (coordinates_t *)realloc(path, (path_count + 1) * sizeof(coordinates_t));
		if (new_path == NULL) {

			printf("[UCS] Trouble reallocating memory for path.\n");
			exit(1);
		}
		path = new_path;

		if (is_goal(current_coordinates, G1, G2) == 1) {

			printf("Reached G1[%d][%d]\n", G1.x, G1.y);

			print_path(path, path_count);
			printf("Number of expansions: %d.\n", expansion_count);
			return;
		}
		else if (is_goal(current_coordinates, G1, G2) == 2) {

			printf("Reached G2[%d][%d]\n", G2.x, G2.y);

			print_path(path, path_count);
			printf("Number of expansions: %d.\n", expansion_count);
			return;
		}

		// Apply Transition Operators for each node of the Search Frontier (clockwise).

		// Move upwards.
		if (current_coordinates.x > 0) {

			if (maze[current_coordinates.x - 1][current_coordinates.y] == 1) {

				new_coordinates.x = current_coordinates.x - 1;
				new_coordinates.y = current_coordinates.y;
				if (!check_frontier(&search_frontier, new_coordinates) && !is_visited(new_coordinates, visited, visited_count)) {
					
					push(&search_frontier, new_coordinates, priority[new_coordinates.x][new_coordinates.y]);
					expansion_count++;
				}
			}
		}

		// Upper right diagonal.
		if (current_coordinates.x > 0 && current_coordinates.y < maze_size - 1) {

			if (maze[current_coordinates.x - 1][current_coordinates.y + 1] == 1) {

				new_coordinates.x = current_coordinates.x - 1;
				new_coordinates.y = current_coordinates.y + 1;
				if (!check_frontier(&search_frontier, new_coordinates) && !is_visited(new_coordinates, visited, visited_count)) {
					
					push(&search_frontier, new_coordinates, priority[new_coordinates.x][new_coordinates.y]);
					expansion_count++;
				}
			}
		}

		// Move to the right.
		if (current_coordinates.y < maze_size - 1) {

			if (maze[current_coordinates.x][current_coordinates.y + 1] == 1) {

				new_coordinates.x = current_coordinates.x;
				new_coordinates.y = current_coordinates.y + 1;
				if (!check_frontier(&search_frontier, new_coordinates) && !is_visited(new_coordinates, visited, visited_count)) {
					
					push(&search_frontier, new_coordinates, priority[new_coordinates.x][new_coordinates.y]);
					expansion_count++;
				}
			}
		}

		// Down right diagonal.
		if (current_coordinates.x < maze_size - 1 && current_coordinates.y < maze_size - 1) {

			if (maze[current_coordinates.x + 1][current_coordinates.y + 1] == 1) {

				new_coordinates.x = current_coordinates.x + 1;
				new_coordinates.y = current_coordinates.y + 1;
				if (!check_frontier(&search_frontier, new_coordinates) && !is_visited(new_coordinates, visited, visited_count)) {
					
					push(&search_frontier, new_coordinates, priority[new_coordinates.x][new_coordinates.y]);
					expansion_count++;
				}
			}
		}

		// Move downwards.
		if (current_coordinates.x < maze_size - 1) {

			if (maze[current_coordinates.x + 1][current_coordinates.y] == 1) {

				new_coordinates.x = current_coordinates.x + 1;
				new_coordinates.y = current_coordinates.y;
				if (!check_frontier(&search_frontier, new_coordinates) && !is_visited(new_coordinates, visited, visited_count)) {
					
					push(&search_frontier, new_coordinates, priority[new_coordinates.x][new_coordinates.y]);
					expansion_count++;
				}
			}
		}

		// Down left diagonal.
		if (current_coordinates.x < maze_size - 1 && current_coordinates.y > 0) {

			if (maze[current_coordinates.x + 1][current_coordinates.y - 1] == 1) {

				new_coordinates.x = current_coordinates.x + 1;
				new_coordinates.y = current_coordinates.y - 1;
				if (!check_frontier(&search_frontier, new_coordinates) && !is_visited(new_coordinates, visited, visited_count)) {
					
					push(&search_frontier, new_coordinates, priority[new_coordinates.x][new_coordinates.y]);
					expansion_count++;
				}
			}
		}

		// Move to the left.
		if (current_coordinates.y > 0) {

			if (maze[current_coordinates.x][current_coordinates.y - 1] == 1) {

				new_coordinates.x = current_coordinates.x;
				new_coordinates.y = current_coordinates.y - 1;
				if (!check_frontier(&search_frontier, new_coordinates) && !is_visited(new_coordinates, visited, visited_count)) {
					
					push(&search_frontier, new_coordinates, priority[new_coordinates.x][new_coordinates.y]);
					expansion_count++;
				}
			}
		}
		
		// Upper left diagonal.
		if (current_coordinates.x > 0 && current_coordinates.y > 0) {

			if (maze[current_coordinates.x - 1][current_coordinates.y - 1] == 1) {

				new_coordinates.x = current_coordinates.x - 1;
				new_coordinates.y = current_coordinates.y - 1;
				if (!check_frontier(&search_frontier, new_coordinates) && !is_visited(new_coordinates, visited, visited_count)) {
					
					push(&search_frontier, new_coordinates, priority[new_coordinates.x][new_coordinates.y]);
					expansion_count++;
				}
			}
		}

		push_visited(current_coordinates, &visited, &visited_count);

		pop(&search_frontier);
	}

	printf("Unable to reach G1[%d][%d] nor G2[%d][%d].\n", G1.x, G1.y, G2.x, G2.y);

	free(search_frontier);
	free(visited);
	free(path);
	free(priority);

	return;
}

// -----------------------------------
// 				  A*
// -----------------------------------

void a_star(int **maze, int maze_size, coordinates_t S, coordinates_t G1, coordinates_t G2) {

	node_t *search_frontier = NULL;
	coordinates_t *visited = NULL;
	coordinates_t *path = NULL, *new_path = NULL;
	int *score = NULL;		// The result of f(n) = g(n) + h(n). Its index is synchronized with "visited" lists index "visited_count".
	int **cost = NULL;		// The g(n) value of each node.
	coordinates_t current_coordinates, new_coordinates;
	int visited_count = 0, path_count = 0;
	int current_heuristic = 0, expansion_count = 0;

	visited = (coordinates_t *)malloc((maze_size * maze_size) * sizeof(coordinates_t));
	if (visited == NULL) {

		printf("[A*] Trouble allocating memory for 'visited' list.\n");
		exit(1);
	}

	score = (int *)malloc((maze_size * maze_size) * sizeof(int));
	if (score == NULL) {

		printf("[A*] Trouble allocating memory for 'score' list.\n");
		exit(1);
	}

	path = (coordinates_t *)malloc(sizeof(coordinates_t));
	if (path == NULL) {

		printf("[A*] Trouble allocating memory for 'path' list.\n");
		exit(1);
	}

	cost = assign_cost(maze, maze_size, S);

	search_frontier = new_node(S, min_heuristic(heuristic(S, G1), heuristic(S, G2)) + cost[S.x][S.y]);

	while (!is_empty(&search_frontier)) {

		current_coordinates = peek(&search_frontier);

		current_heuristic = min_heuristic(heuristic(current_coordinates, G1), heuristic(current_coordinates, G2)) + cost[current_coordinates.x][current_coordinates.y];

		if (is_visited(current_coordinates, visited, visited_count)) {

			if (score[visited_count] < current_heuristic) {		// FIX || Wrong index.

				pop(&search_frontier);
				continue;
			}
			else {

				score[visited_count] = current_heuristic;		// FIX || Wrong index.
			}											
		}

		path[path_count] = current_coordinates;
		path_count++;
		new_path = (coordinates_t *)realloc(path, (path_count + 1) * sizeof(coordinates_t));
		if (new_path == NULL) {

			printf("[A*] Trouble reallocating memory for 'path' list.\n");
			exit(1);
		}
		path = new_path;

		if (is_goal(current_coordinates, G1, G2) == 1) {

			printf("Reached G1[%d][%d].\n", G1.x, G1.y);

			print_path(path, path_count);
			printf("Number of expansions: %d.\n", expansion_count);

			return;
		}
		else if (is_goal(current_coordinates, G1, G2) == 2) {

			printf("Reached G2[%d][%d].\n", G2.x, G2.y);

			print_path(path, path_count);
			printf("Number of expansions: %d.\n", expansion_count);

			return;
		}

		// Apply Transition Operators for each node of the Search Frontier (clockwise).

		// Move upwards.
		if (current_coordinates.x > 0) {

			if (maze[current_coordinates.x - 1][current_coordinates.y] == 1) {

				new_coordinates.x = current_coordinates.x - 1;
				new_coordinates.y = current_coordinates.y;
				if (!check_frontier(&search_frontier, new_coordinates) && !is_visited(new_coordinates, visited, visited_count)) {

					push(&search_frontier, new_coordinates, min_heuristic(heuristic(new_coordinates, G1), heuristic(new_coordinates, G2)) + cost[new_coordinates.x][new_coordinates.y]);
					expansion_count++;
					//printf("Pushing new node with coordinates[%d][%d] of priority %d.\n", new_coordinates.x, new_coordinates.y, priority[new_coordinates.x][new_coordinates.y]);
				}
			}
		}

		// Upper right diagonal.
		if (current_coordinates.x > 0 && current_coordinates.y < maze_size - 1) {

			if (maze[current_coordinates.x - 1][current_coordinates.y + 1] == 1) {

				new_coordinates.x = current_coordinates.x - 1;
				new_coordinates.y = current_coordinates.y + 1;
				if (!check_frontier(&search_frontier, new_coordinates) && !is_visited(new_coordinates, visited, visited_count)) {
					
					if (!check_frontier(&search_frontier, new_coordinates) && !is_visited(new_coordinates, visited, visited_count)) {

						push(&search_frontier, new_coordinates, min_heuristic(heuristic(new_coordinates, G1), heuristic(new_coordinates, G2)) + cost[new_coordinates.x][new_coordinates.y]);
						expansion_count++;
						//printf("Pushing new node with coordinates[%d][%d] of priority %d.\n", new_coordinates.x, new_coordinates.y, priority[new_coordinates.x][new_coordinates.y]);
					}
				}
			}
		}

		// Move to the right.
		if (current_coordinates.y < maze_size - 1) {

			if (maze[current_coordinates.x][current_coordinates.y + 1] == 1) {

				new_coordinates.x = current_coordinates.x;
				new_coordinates.y = current_coordinates.y + 1;
				if (!check_frontier(&search_frontier, new_coordinates) && !is_visited(new_coordinates, visited, visited_count)) {
					
					if (!check_frontier(&search_frontier, new_coordinates) && !is_visited(new_coordinates, visited, visited_count)) {

						push(&search_frontier, new_coordinates, min_heuristic(heuristic(new_coordinates, G1), heuristic(new_coordinates, G2)) + cost[new_coordinates.x][new_coordinates.y]);
						expansion_count++;
						//printf("Pushing new node with coordinates[%d][%d] of priority %d.\n", new_coordinates.x, new_coordinates.y, priority[new_coordinates.x][new_coordinates.y]);
					}
				}
			}
		}

		// Down right diagonal.
		if (current_coordinates.x < maze_size - 1 && current_coordinates.y < maze_size - 1) {

			if (maze[current_coordinates.x + 1][current_coordinates.y + 1] == 1) {

				new_coordinates.x = current_coordinates.x + 1;
				new_coordinates.y = current_coordinates.y + 1;
				if (!check_frontier(&search_frontier, new_coordinates) && !is_visited(new_coordinates, visited, visited_count)) {
					
					if (!check_frontier(&search_frontier, new_coordinates) && !is_visited(new_coordinates, visited, visited_count)) {

						push(&search_frontier, new_coordinates, min_heuristic(heuristic(new_coordinates, G1), heuristic(new_coordinates, G2)) + cost[new_coordinates.x][new_coordinates.y]);
						expansion_count++;
						//printf("Pushing new node with coordinates[%d][%d] of priority %d.\n", new_coordinates.x, new_coordinates.y, priority[new_coordinates.x][new_coordinates.y]);
					}
				}
			}
		}

		// Move downwards.
		if (current_coordinates.x < maze_size - 1) {

			if (maze[current_coordinates.x + 1][current_coordinates.y] == 1) {

				new_coordinates.x = current_coordinates.x + 1;
				new_coordinates.y = current_coordinates.y;
				if (!check_frontier(&search_frontier, new_coordinates) && !is_visited(new_coordinates, visited, visited_count)) {
					
					if (!check_frontier(&search_frontier, new_coordinates) && !is_visited(new_coordinates, visited, visited_count)) {

						push(&search_frontier, new_coordinates, min_heuristic(heuristic(new_coordinates, G1), heuristic(new_coordinates, G2)) + cost[new_coordinates.x][new_coordinates.y]);
						expansion_count++;
						//printf("Pushing new node with coordinates[%d][%d] of priority %d.\n", new_coordinates.x, new_coordinates.y, priority[new_coordinates.x][new_coordinates.y]);
					}
				}
			}
		}

		// Down left diagonal.
		if (current_coordinates.x < maze_size - 1 && current_coordinates.y > 0) {

			if (maze[current_coordinates.x + 1][current_coordinates.y - 1] == 1) {

				new_coordinates.x = current_coordinates.x + 1;
				new_coordinates.y = current_coordinates.y - 1;
				if (!check_frontier(&search_frontier, new_coordinates) && !is_visited(new_coordinates, visited, visited_count)) {
					
					if (!check_frontier(&search_frontier, new_coordinates) && !is_visited(new_coordinates, visited, visited_count)) {

						push(&search_frontier, new_coordinates, min_heuristic(heuristic(new_coordinates, G1), heuristic(new_coordinates, G2)) + cost[new_coordinates.x][new_coordinates.y]);
						expansion_count++;
						//printf("Pushing new node with coordinates[%d][%d] of priority %d.\n", new_coordinates.x, new_coordinates.y, priority[new_coordinates.x][new_coordinates.y]);
					}
				}
			}
		}

		// Move to the left.
		if (current_coordinates.y > 0) {

			if (maze[current_coordinates.x][current_coordinates.y - 1] == 1) {

				new_coordinates.x = current_coordinates.x;
				new_coordinates.y = current_coordinates.y - 1;
				if (!check_frontier(&search_frontier, new_coordinates) && !is_visited(new_coordinates, visited, visited_count)) {
					
					if (!check_frontier(&search_frontier, new_coordinates) && !is_visited(new_coordinates, visited, visited_count)) {

						push(&search_frontier, new_coordinates, min_heuristic(heuristic(new_coordinates, G1), heuristic(new_coordinates, G2)) + cost[new_coordinates.x][new_coordinates.y]);
						expansion_count++;
						//printf("Pushing new node with coordinates[%d][%d] of priority %d.\n", new_coordinates.x, new_coordinates.y, priority[new_coordinates.x][new_coordinates.y]);
					}
				}
			}
		}
		
		// Upper left diagonal.
		if (current_coordinates.x > 0 && current_coordinates.y > 0) {

			if (maze[current_coordinates.x - 1][current_coordinates.y - 1] == 1) {

				new_coordinates.x = current_coordinates.x - 1;
				new_coordinates.y = current_coordinates.y - 1;
				if (!check_frontier(&search_frontier, new_coordinates) && !is_visited(new_coordinates, visited, visited_count)) {
					
					if (!check_frontier(&search_frontier, new_coordinates) && !is_visited(new_coordinates, visited, visited_count)) {

						push(&search_frontier, new_coordinates, min_heuristic(heuristic(new_coordinates, G1), heuristic(new_coordinates, G2)) + cost[new_coordinates.x][new_coordinates.y]);
						expansion_count++;
						//printf("Pushing new node with coordinates[%d][%d] of priority %d.\n", new_coordinates.x, new_coordinates.y, priority[new_coordinates.x][new_coordinates.y]);
					}
				}
			}
		}

		push_visited(current_coordinates, &visited, &visited_count);
		score[visited_count] = current_heuristic;

		pop(&search_frontier);
	}

	printf("Unable to reach G1[%d][%d] nor G2[%d][%d].\n", G1.x, G1.y, G2.x, G2.y);

	free(search_frontier);
	free(visited);
	free(path);
	free(score);
	free(cost);
}

// -----------------------------------
// 				Tests
// -----------------------------------

void test_visited() {

	coordinates_t *visited = NULL;
	coordinates_t current_coordinates;
	int i;

	visited = (coordinates_t *)malloc(5 * sizeof(coordinates_t));
	if (visited == NULL) {

		printf("Trouble allocating memory for 'visited' list.\n");
		exit(1);
	}

	for (i = 1; i < 5; i++) {

		(visited[i]).x = i;
		(visited[i]).y = i;
	}

	for (i = 0; i < 5; i++) {

		printf("visited[%d] = [%d][%d]\n", i, (visited[i]).x, (visited[i]).y);
	}

	current_coordinates.x = 3;
	current_coordinates.y = 3;

	if (is_visited(current_coordinates, visited, 5)) {

		printf("[%d][%d] is visited.\n", current_coordinates.x, current_coordinates.y);
	}
	else {

		printf("[%d][%d] is not visited.\n", current_coordinates.x, current_coordinates.y);
	}
}

void test_push_visited() {

	coordinates_t *visited = NULL;
	coordinates_t current_coordinates, new_coordinates;
	int i, visited_count = 0;

	visited = (coordinates_t *)malloc(sizeof(coordinates_t));
	if (visited == NULL) {

		printf("Trouble allocating memory for 'visited' list.\n");
		exit(1);
	}

	new_coordinates.x = 5;
	new_coordinates.y = 5;

	push_visited(new_coordinates, &visited, &visited_count);

	printf("Number of visited nodes: %d\n", visited_count);

	for (i = 0; i < visited_count; i++) {

		printf("Visited[%d] = [%d][%d]\n", i, (visited[i]).x, (visited[i]).y);
	}
}

void test_queue(coordinates_t S) {

	node_t *search_frontier = NULL;
	int i, priority = 1;

	search_frontier = new_node(S, priority);

	for (i = 0; i < 5; i++) {

		S.x++;
		S.y++;
		priority++;

		push(&search_frontier, S, priority);
	}

	print_frontier(&search_frontier);

	for (i = 0; i < 4; i++) {

		pop(&search_frontier);

		print_frontier(&search_frontier);
	}
}

void test_check_frontier(coordinates_t S) {

	node_t *search_frontier = NULL;
	coordinates_t current_coordinates;
	int i, priority = 1;

	search_frontier = new_node(S, priority);

	for (i = 0; i < 5; i++) {

		S.x++;
		S.y++;
		priority++;

		push(&search_frontier, S, priority);
	}

	print_frontier(&search_frontier);

	current_coordinates.x = 3;
	current_coordinates.y = 3;

	check_frontier(&search_frontier, current_coordinates);
}

void test_assign_cost(int **maze, int maze_size, coordinates_t S) {

	int **cell_priority = NULL;
	int i, j;

	cell_priority = assign_cost(maze, maze_size, S);

	printf("\n");

	for (i = 0; i < maze_size; i++) {
		for (j = 0; j < maze_size; j++) {

			printf("%d ", cell_priority[i][j]);
		}
		printf("\n");
	}

	printf("\n");
}

void test_heuristic() {

	coordinates_t current_coordinates, goal_coordinates;

	current_coordinates.x = 1;
	current_coordinates.y = 3;

	goal_coordinates.x = 4;
	goal_coordinates.y = 4;

	printf("Heuristic of [%d][%d] with goal node [%d][%d]: %d\n", current_coordinates.x, current_coordinates.y, goal_coordinates.x, goal_coordinates.y, heuristic(current_coordinates, goal_coordinates));
}

// -----------------------------------
// 				Main
// -----------------------------------

int main() {

	int **new_maze = NULL;
	int maze_size;
	double p;			// Probability of a cell being free. 0 <= p <= 1.
	coordinates_t S;	// Coordinates of starting cell.
	coordinates_t G1;	// Coordinates of final (exit) cell 1.
	coordinates_t G2;	// Coordinates of final (exit) cell 2.

	get_maze_specs(&maze_size, &p);

	new_maze = create_maze(maze_size);

	get_coordinates(&S, &G1, &G2, maze_size);

	set_maze_layout(new_maze, maze_size, p, S, G1, G2);

	printf("\nUniform Cost Search\n");

	ucs(new_maze, maze_size, S, G1, G2);

	print_maze(new_maze, maze_size);

	printf("\nA*\n");

	a_star(new_maze, maze_size, S, G1, G2);

	print_maze(new_maze, maze_size);

	free(new_maze);
	return 0;
}