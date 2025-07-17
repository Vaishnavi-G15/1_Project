#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_DIMENSION 100

extern char** maze;
extern int** visited;
extern int rows, cols;
extern int start_row, start_col;

// Enum for terrain types
enum Terrain {
    TERRAIN_EMPTY,
    TERRAIN_WALL,
    TERRAIN_GOAL,
    TERRAIN_CRUMB
};

// Function prototypes
bool is_valid(int row, int col);
bool is_goal(int row, int col);
bool ao_star(int row, int col);
void mark_ao_path();
void write_ao_solution(const char* file_name);

// Check if a position is valid in the maze
bool is_valid(int row, int col) {
    return row >= 0 && row < rows && col >= 0 && col < cols && visited[row][col] != TERRAIN_WALL;
}

// Check if the current cell is the goal
bool is_goal(int row, int col) {
    return visited[row][col] == TERRAIN_GOAL;
}

// AO* Search Algorithm
bool ao_star(int row, int col) {
    if (!is_valid(row, col)) {
        return false;
    }

    if (is_goal(row, col)) {
        return true;
    }

    // Mark as visited to avoid revisiting
    if (visited[row][col] == TERRAIN_EMPTY) {
        visited[row][col] = TERRAIN_CRUMB;
    }

    // Explore all AND (branching) options
    bool left = ao_star(row, col - 1);
    bool right = ao_star(row, col + 1);
    bool up = ao_star(row - 1, col);
    bool down = ao_star(row + 1, col);

    // If any valid path exists, return true
    if (left || right || up || down) {
        visited[row][col] = TERRAIN_CRUMB;  // Mark as part of the solution
        return true;
    }

    // Otherwise, backtrack
    visited[row][col] = TERRAIN_EMPTY;
    return false;
}

// Mark the AO* path in the maze
void mark_ao_path() {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (visited[i][j] == TERRAIN_CRUMB) {
                maze[i][j] = '.';
            }
        }
    }
}

// Write the AO* solution to a file
void write_ao_solution(const char* file_name) {
    FILE* file = fopen(file_name, "w");
    if (!file) {
        printf("Error: Cannot open file %s.\n", file_name);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(file, "%c", maze[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

void read_maze(const char* file_name);
void initialize_visited();
void free_memory();
bool ao_star(int row, int col);
void mark_ao_path();
void write_ao_solution(const char* file_name);

int main() {
    // Read the maze from the file
    read_maze("maze.txt");

    // Initialize visited matrix
    initialize_visited();

    // Perform AO* search to solve the maze
    if (!ao_star(start_row, start_col)) {
        printf("No path to the goal could be found using AO*.\n");
    } else {
        mark_ao_path();
        write_ao_solution("ao_sol.txt");
    }

    // Free allocated memory
    free_memory();

    return 0;
}
