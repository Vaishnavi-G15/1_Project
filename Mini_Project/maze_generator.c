#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WALL 'x'
#define PATH ' '
#define START 's' // start of the maze
#define GOAL 'g' // goal point of maze

// Directions: up, down, left, right
int dx[] = {0, 0, -2, 2};
int dy[] = {-2, 2, 0, 0};

// Function prototypes
void initializeMaze(char **maze, int rows, int cols);
void carvePath(char **maze, int rows, int cols, int x, int y);
void placeStartAndGoal(char **maze, int rows, int cols, int *startRow, int *startCol, int *goalRow, int *goalCol);
void writeMazeToFile(char **maze, int rows, int cols, const char *filename);
void generateMazeWithObstacles(char **maze, int rows, int cols, int obstacleCount);
char **allocateMaze(int rows, int cols);

int main() {
    int rows, cols;
    char **maze;

    printf("Enter maze dimensions (rows cols): ");
    scanf("%d %d", &rows, &cols);

    // Ensure odd dimensions for proper maze generation
    if (rows % 2 == 0) rows++;
    if (cols % 2 == 0) cols++;

    // Allocate and initialize the maze
    maze = allocateMaze(rows, cols);
    initializeMaze(maze, rows, cols);

    // Seed the random number generator
    srand(time(NULL));

    int choice;
    printf("Choose the type of maze to generate:\n");
    printf("1. Simple Maze\n");
    printf("2. Maze with Obstacles\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    // Generate the chosen type of maze
    if (choice == 1) {
        carvePath(maze, rows, cols, 1, 1);
    } else if (choice == 2) {
        carvePath(maze, rows, cols, 1, 1);
        int obstacleCount = (rows * cols) / 10; // Example: 10% of the cells will be obstacles
        generateMazeWithObstacles(maze, rows, cols, obstacleCount);
    } else {
        printf("Invalid choice! Generating a simple maze by default.\n");
        carvePath(maze, rows, cols, 1, 1);
    }

    // Place start ('s') and goal ('g') in the maze
    int startRow, startCol, goalRow, goalCol;
    placeStartAndGoal(maze, rows, cols, &startRow, &startCol, &goalRow, &goalCol);

    // Write the generated maze to a file
    writeMazeToFile(maze, rows, cols, "maze.txt");

    printf("Maze generated and saved to 'maze.txt'.\n");

    // Free allocated memory
    for (int i = 0; i < rows; i++) {
        free(maze[i]);
    }
    free(maze);

    return 0;
}

// Allocate memory for the maze
char **allocateMaze(int rows, int cols) {
    char **maze = (char **)malloc(rows * sizeof(char *));
    for (int i = 0; i < rows; i++) {
        maze[i] = (char *)malloc(cols * sizeof(char));
    }
    return maze;
}

// Initialize the maze with walls
void initializeMaze(char **maze, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            maze[i][j] = WALL;
        }
    }
}

// Recursive function to carve paths in the maze
void carvePath(char **maze, int rows, int cols, int x, int y) {
    maze[x][y] = PATH;

    // Shuffle directions for randomness
    for (int i = 0; i < 4; i++) {
        int r = rand() % 4;
        int tempX = dx[i], tempY = dy[i];
        dx[i] = dx[r], dy[i] = dy[r];
        dx[r] = tempX, dy[r] = tempY;
    }

    for (int i = 0; i < 4; i++) {
        int newX = x + dx[i];
        int newY = y + dy[i];
        int betweenX = x + dx[i] / 2;
        int betweenY = y + dy[i] / 2;

        if (newX > 0 && newX < rows && newY > 0 && newY < cols && maze[newX][newY] == WALL) {
            maze[betweenX][betweenY] = PATH; // Carve through the wall
            carvePath(maze, rows, cols, newX, newY);
        }
    }
}

// Generate a maze with additional obstacles
void generateMazeWithObstacles(char **maze, int rows, int cols, int obstacleCount) {
    for (int i = 0; i < obstacleCount; i++) {
        int randRow = rand() % rows;
        int randCol = rand() % cols;

        // Place an obstacle if it's not the start, goal, or already a wall
        if (maze[randRow][randCol] == PATH) {
            maze[randRow][randCol] = WALL;
        }
    }
}

// Place the start ('s') and goal ('g') in the maze
void placeStartAndGoal(char **maze, int rows, int cols, int *startRow, int *startCol, int *goalRow, int *goalCol) {
    // Place start at the top-left corner of the maze
    *startRow = 1;
    *startCol = 1;
    maze[*startRow][*startCol] = START;

    // Place goal at the bottom-right corner of the maze
    *goalRow = rows - 2;
    *goalCol = cols - 2;
    maze[*goalRow][*goalCol] = GOAL;
}

// Write the maze to a file
void writeMazeToFile(char **maze, int rows, int cols, const char *filename) {
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
