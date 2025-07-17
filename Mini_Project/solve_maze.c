#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

#define WALL 'x'
#define PATH ' '
#define START 's'
#define GOAL 'g'

#define MAX_DIMENSION 100

char** maze;
int** visited;
int rows, cols;
int start_row, start_col, goal_row, goal_col;

// Define the Cell struct for A* search
struct Cell {
    int row;
    int col;
    int g_cost;  // Cost from the start node
    int h_cost;  // Heuristic cost to the goal
    int f_cost;  // Total cost (g_cost + h_cost)
    struct Cell* parent;  // Pointer to the parent cell in the path
};

// Declare functions from Astar_code.c
void aStarSearch(int** grid, int rows, int cols, struct Cell start, struct Cell goal);
int** readGridFromFile(const char* filename, int* rows, int* cols, struct Cell* start, struct Cell* goal);
void printGrid(int** grid, int rows, int cols);

// Function to allocate memory for a 2D char array (for the maze)
char** allocate_char_matrix(int rows, int cols) {
    char** matrix = (char**)malloc(rows * sizeof(char*));
    for (int i = 0; i < rows; i++) {
        matrix[i] = (char*)malloc(cols * sizeof(char));
    }
    return matrix;
}

// Function to allocate memory for a 2D int array (for visited array)
int** allocate_int_matrix(int rows, int cols) {
    int** matrix = (int**)malloc(rows * sizeof(int*));
    for (int i = 0; i < rows; i++) {
        matrix[i] = (int*)malloc(cols * sizeof(int));
    }
    return matrix;
}

// High-resolution timer function
double get_time_in_seconds() {
    struct timeval time;
    gettimeofday(&time, NULL);
    return time.tv_sec + (time.tv_usec / 1e6);
}

// Function to read the maze from a file
void read_maze(const char* file_name) {
    FILE* file = fopen(file_name, "r");
    if (!file) {
        printf("Error: Cannot open file %s.\n", file_name);
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%d,%d\n", &rows, &cols);

    maze = allocate_char_matrix(rows, cols);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fscanf(file, "%c", &maze[i][j]);
            if (maze[i][j] == '\n') {
                j--;  // Skip newlines
                continue;
            }
            if (maze[i][j] == 's') {
                start_row = i;
                start_col = j;
            }
            if (maze[i][j] == 'g') {
                goal_row = i;
                goal_col = j;
            }
        }
    }

    fclose(file);
}

// Function to initialize the visited matrix based on the maze
void initialize_visited() {
    visited = allocate_int_matrix(rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (maze[i][j] == 'x') {
                visited[i][j] = 1;  // WALL
            } else if (maze[i][j] == 'g') {
                visited[i][j] = 2;  // GOAL
            } else {
                visited[i][j] = 0;  // Empty space
            }
        }
    }
}

// Function to implement DFS (kept the same)
int dfs(int row, int col) {
    if (row < 0 || row >= rows || col < 0 || col >= cols || visited[row][col] == 1) {
        return 0;
    }

    if (visited[row][col] == 2) {
        return 1;
    }

    if (visited[row][col] == 0) {
        visited[row][col] = 1;  // Mark as visited

        if (dfs(row, col - 1) || dfs(row + 1, col) || dfs(row, col + 1) || dfs(row - 1, col)) {
            visited[row][col] = 3;  // Mark the path
            return 1;
        }
    }

    return 0;
}

// Function to solve the maze using A* algorithm
void solve_with_astar() {
    struct Cell start, dest;

    start.row = start_row;
    start.col = start_col;
    dest.row = goal_row;
    dest.col = goal_col;

    // Read the maze from file and create the grid
    int **grid = readGridFromFile("maze.txt", &rows, &cols, &start, &dest);

    // Display the input grid
    printf("Input Grid:\n");
    printGrid(grid, rows, cols);

    // Perform A* search
    aStarSearch(grid, rows, cols, start, dest);

    // Free the dynamically allocated memory for the grid
    for (int i = 0; i < rows; i++) {
        free(grid[i]);
    }
    free(grid);
}

// Function to write the maze to a file (similar to what was in generate_maze.c)
void write_maze(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error: Cannot write to file %s.\n", filename);
        exit(EXIT_FAILURE);
    }

    fprintf(file, "%d,%d\n", rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(file, "%c", maze[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

// Function to mark the path in the maze (for both DFS and A* solution)
void mark_path_in_maze() {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (visited[i][j] == 3) {  // Mark the path
                maze[i][j] = '.';
            }
        }
    }
}

// Function to ask the user which algorithm to use and solve the maze
int main() {
    // Read the maze from file
    read_maze("maze.txt");

    // Initialize the visited matrix
    initialize_visited();

    // Ask the user which algorithm to use
    int choice;
    double start_time, end_time, time_taken;

    printf("Choose the algorithm to solve the maze:\n");
    printf("1. Depth-First Search (DFS)\n");
    printf("2. A* Search\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    if (choice == 1) {
        start_time = get_time_in_seconds();
        // Perform DFS to solve the maze
        if (!dfs(start_row, start_col)) {
            printf("No path to the goal could be found using DFS.\n");
        } else {
            mark_path_in_maze();
            printf("DFS Path found and maze solved.\n");
        }
        end_time = get_time_in_seconds();
        time_taken = end_time - start_time;
        printf("Time taken by DFS: %.6f seconds\n", time_taken);
    } else if (choice == 2) {
        start_time = get_time_in_seconds();
        // Solve using A* algorithm
        solve_with_astar();
        end_time = get_time_in_seconds();
        time_taken = end_time - start_time;
        printf("Time taken by A*: %.6f seconds\n", time_taken);
    } else {
        printf("Invalid choice!\n");
    }

    // Write the solved maze to a file
    write_maze("sol.txt");

    // Free allocated memory
    for (int i = 0; i < rows; i++) {
        free(maze[i]);
        free(visited[i]);
    }
    free(maze);
    free(visited);

    return 0;
}
