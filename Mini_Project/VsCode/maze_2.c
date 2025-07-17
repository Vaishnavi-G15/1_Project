#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

/* ================================================================= */
/*           METHODS, CONSTANTS, AND STRUCTS USED IN THE PROGRAM     */
/* ================================================================= */

#define MAX_PATH 256

typedef struct {
    int x, y;
} Coordinate;

Coordinate new_coordinate(int x, int y) {
    Coordinate ret = {x, y};
    return ret;
}

char equals_coordinates(Coordinate coord1, Coordinate coord2) {
    return (coord1.x == coord2.x && coord1.y == coord2.y) ? 1 : 0;
}

/* Constants for cell types */
#define WALL 'X'
#define PATH ' '
#define ENTRANCE 'I'
#define EXIT 'O'
#define VISITED '*'

/* ================================================================= */
/*                      HELPER FUNCTIONS                             */
/* ================================================================= */

/* Function to read the maze from a file */
char** read_maze(const char* path, int* rows, int* cols) {
    FILE* file = fopen(path, "r");
    if (!file) {
        printf("Error: Could not open file %s.\n", path);
        return NULL;
    }

    fscanf(file, "%d %d", rows, cols);

    char** maze = (char**)malloc((*rows) * sizeof(char*));
    for (int i = 0; i < *rows; i++) {
        maze[i] = (char*)malloc((*cols) * sizeof(char));
        for (int j = 0; j < *cols; j++) {
            maze[i][j] = fgetc(file);
            if (maze[i][j] == '\n') maze[i][j] = fgetc(file);
        }
    }
    fclose(file);
    return maze;
}

/* Print the maze to the console */
void print_maze(char** maze, int rows, int cols) {
    printf("\nMaze Layout:\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%c", maze[i][j]);
        }
        printf("\n");
    }
}

/* Save the maze to a file */
void save_maze(char** maze, int rows, int cols, const char* path) {
    FILE* file = fopen(path, "w");
    if (!file) {
        printf("Error: Could not save maze to %s.\n", path);
        return;
    }
    fprintf(file, "%d\n%d\n", rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(file, "%c", maze[i][j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

/* Deallocate the maze memory */
void free_maze(char** maze, int rows) {
    for (int i = 0; i < rows; i++) {
        free(maze[i]);
    }
    free(maze);
}

/* ================================================================= */
/*                      MAZE SOLVER FUNCTIONS                        */
/* ================================================================= */

/* Get the entrance to the maze */
Coordinate get_entrance(char** maze, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        if (maze[i][0] == ENTRANCE) return new_coordinate(i, 0);
        if (maze[i][cols - 1] == ENTRANCE) return new_coordinate(i, cols - 1);
    }
    for (int j = 0; j < cols; j++) {
        if (maze[0][j] == ENTRANCE) return new_coordinate(0, j);
        if (maze[rows - 1][j] == ENTRANCE) return new_coordinate(rows - 1, j);
    }
    return new_coordinate(-1, -1); // No entrance found
}

/* Recursive solver for the maze */
Coordinate solve(Coordinate current, char** maze, int rows, int cols) {
    if (current.x == -1 || current.y == -1) return current;
    if (maze[current.x][current.y] == EXIT) return new_coordinate(-2, -2);

    maze[current.x][current.y] = VISITED;

    Coordinate directions[] = {
        new_coordinate(current.x + 1, current.y), // Down
        new_coordinate(current.x - 1, current.y), // Up
        new_coordinate(current.x, current.y + 1), // Right
        new_coordinate(current.x, current.y - 1)  // Left
    };

    for (int i = 0; i < 4; i++) {
        Coordinate next = directions[i];
        if (next.x >= 0 && next.x < rows && next.y >= 0 && next.y < cols &&
            (maze[next.x][next.y] == PATH || maze[next.x][next.y] == EXIT)) {
            Coordinate result = solve(next, maze, rows, cols);
            if (result.x == -2) return result;
        }
    }

    maze[current.x][current.y] = PATH; // Backtrack
    return current;
}

/* Solve the maze interactively */
void interactive_solver(char** maze, int rows, int cols) {
    Coordinate entrance = get_entrance(maze, rows, cols);
    Coordinate current = entrance;

    while (1) {
        print_maze(maze, rows, cols);
        printf("Current Position: (%d, %d)\n", current.x, current.y);
        printf("Move (W/A/S/D to navigate, Q to quit): ");
        char move;
        scanf(" %c", &move);

        Coordinate next = current;
        if (toupper(move) == 'W' && current.x > 0) next.x--;
        else if (toupper(move) == 'S' && current.x < rows - 1) next.x++;
        else if (toupper(move) == 'A' && current.y > 0) next.y--;
        else if (toupper(move) == 'D' && current.y < cols - 1) next.y++;
        else if (toupper(move) == 'Q') break;

        if (maze[next.x][next.y] == WALL) {
            printf("Cannot move into a wall!\n");
        } else {
            current = next;
            if (maze[current.x][current.y] == EXIT) {
                printf("Congratulations! You found the exit!\n");
                break;
            }
        }
    }
}

/* ================================================================= */
/*                              MAIN                                 */
/* ================================================================= */

int main(void) {
    char path[MAX_PATH];
    int rows, cols;

    printf("Enter the path to the maze file: ");
    scanf("%255s", path);

    char** maze = read_maze(path, &rows, &cols);
    if (!maze) return 1;

    print_maze(maze, rows, cols);

    Coordinate entrance = get_entrance(maze, rows, cols);
    if (entrance.x == -1) {
        printf("No valid entrance found in the maze.\n");
    } else {
        printf("Entrance found at (%d, %d).\n", entrance.x, entrance.y);
        solve(entrance, maze, rows, cols);
        print_maze(maze, rows, cols);
    }

    printf("Would you like to interactively explore the maze? (Y/N): ");
    char answer;
    scanf(" %c", &answer);
    if (toupper(answer) == 'Y') {
        interactive_solver(maze, rows, cols);
    }

    save_maze(maze, rows, cols, "solved_maze.txt");
    free_maze(maze, rows);
    return 0;
}
