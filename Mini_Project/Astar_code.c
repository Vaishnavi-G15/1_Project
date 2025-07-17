#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

// Define a structure to represent each cell in the grid
struct Cell {
    int row, col; // Coordinates of the cell
    int f, g, h;  // A* parameters: f = g + h, g = cost to start, h = heuristic to destination
};

// Function prototypes
bool isValid(int row, int col, int numRows, int numCols);
bool isUnblocked(int **grid, int row, int col);
bool isDestination(int row, int col, struct Cell dest);
int calculateHValue(int row, int col, struct Cell dest);
void tracePath(int **grid, struct Cell **cellDetails, struct Cell dest, int numRows, int numCols, struct Cell start);
void aStarSearch(int **grid, int numRows, int numCols, struct Cell start, struct Cell dest);
int **readGridFromFile(const char *filename, int *numRows, int *numCols, struct Cell *start, struct Cell *dest);
void printGrid(int **grid, int numRows, int numCols);
void freeMemory(int **grid, int numRows, struct Cell **cellDetails, bool **openList, bool **closedList);


// Function to check if a cell is within grid bounds
bool isValid(int row, int col, int numRows, int numCols) {
    return (row >= 0 && row < numRows && col >= 0 && col < numCols);
}

// Function to check if a cell is not blocked
bool isUnblocked(int **grid, int row, int col) {
    return grid[row][col] == 0;
}

// Function to check if a cell is the destination
bool isDestination(int row, int col, struct Cell dest) {
    return row == dest.row && col == dest.col;
}

// Heuristic function: Manhattan distance
int calculateHValue(int row, int col, struct Cell dest) {
    return abs(row - dest.row) + abs(col - dest.col);
}

// Function to trace the path
void tracePath(int **grid, struct Cell **cellDetails, struct Cell dest, int numRows, int numCols, struct Cell start) {
    printf("Tracing the path...\n");
    int row = dest.row, col = dest.col;

    FILE *fp = fopen("sol.txt", "w");

    while (!(cellDetails[row][col].row == row && cellDetails[row][col].col == col)) {
        grid[row][col] = 2; // Mark as path
        int tempRow = cellDetails[row][col].row;
        int tempCol = cellDetails[row][col].col;
        row = tempRow;
        col = tempCol;
    }
    grid[row][col] = 2; // Mark start as part of the path

    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            if (i == start.row && j == start.col) {
                fprintf(fp, "s");
            } else if (i == dest.row && j == dest.col) {
                fprintf(fp, "g");
            } else if (grid[i][j] == 0) {
                fprintf(fp, " ");
            } else if (grid[i][j] == 1) {
                fprintf(fp, "x");
            } else if (grid[i][j] == 2) {
                fprintf(fp, ".");
            }
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

// Function to read grid from a file
int **readGridFromFile(const char *filename, int *numRows, int *numCols, struct Cell *start, struct Cell *dest) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Error opening file!\n");
        exit(EXIT_FAILURE);
    }

    fscanf(fp, "%d,%d\n", numRows, numCols);

    int **grid = (int **)malloc(*numRows * sizeof(int *));
    for (int i = 0; i < *numRows; i++) {
        grid[i] = (int *)malloc(*numCols * sizeof(int));
    }

    char temp;
    for (int i = 0; i < *numRows; i++) {
        for (int j = 0; j < *numCols; j++) {
            fscanf(fp, "%c", &temp);
            if (temp == 's') {
                start->row = i;
                start->col = j;
                grid[i][j] = 0;
            } else if (temp == 'g') {
                dest->row = i;
                dest->col = j;
                grid[i][j] = 0;
            } else if (temp == 'x') {
                grid[i][j] = 1;
            } else if (temp == ' ') {
                grid[i][j] = 0;
            } else if (temp == '\n') {
                j--;
            }
        }
    }
    fclose(fp);
    return grid;
}

// Function to print the grid
void printGrid(int **grid, int numRows, int numCols) {
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            printf("%d ", grid[i][j]);
        }
        printf("\n");
    }
}

// A* search algorithm
void aStarSearch(int **grid, int numRows, int numCols, struct Cell start, struct Cell dest) {
    // Initialization
    struct Cell **cellDetails = (struct Cell **)malloc(numRows * sizeof(struct Cell *));
    for (int i = 0; i < numRows; i++) {
        cellDetails[i] = (struct Cell *)malloc(numCols * sizeof(struct Cell));
    }

    bool **closedList = (bool **)calloc(numRows, sizeof(bool *));
    bool **openList = (bool **)calloc(numRows, sizeof(bool *));
    for (int i = 0; i < numRows; i++) {
        closedList[i] = (bool *)calloc(numCols, sizeof(bool));
        openList[i] = (bool *)calloc(numCols, sizeof(bool));
    }

    // Initialize start cell
    int i = start.row, j = start.col;
    cellDetails[i][j] = (struct Cell){.row = i, .col = j, .f = 0, .g = 0, .h = 0};
    openList[i][j] = true;

    // Main algorithm
    while (true) {
        int minF = INT_MAX;
        int minRow = -1, minCol = -1;

        // Find the cell with minimum f
        for (int r = 0; r < numRows; r++) {
            for (int c = 0; c < numCols; c++) {
                if (openList[r][c] && cellDetails[r][c].f < minF) {
                    minF = cellDetails[r][c].f;
                    minRow = r;
                    minCol = c;
                }
            }
        }

        if (minRow == -1) {
            printf("Path not found!\n");
            break;
        }

        openList[minRow][minCol] = false;
        closedList[minRow][minCol] = true;

        // Check adjacent cells
        int rowOffsets[] = {-1, 0, 1, 0};
        int colOffsets[] = {0, 1, 0, -1};

        for (int d = 0; d < 4; d++) {
            int adjRow = minRow + rowOffsets[d];
            int adjCol = minCol + colOffsets[d];

            if (isValid(adjRow, adjCol, numRows, numCols) && isUnblocked(grid, adjRow, adjCol)) {
                if (isDestination(adjRow, adjCol, dest)) {
                    cellDetails[adjRow][adjCol].row = minRow;
                    cellDetails[adjRow][adjCol].col = minCol;
                    printf("Path found!\n");
                    tracePath(grid, cellDetails, dest, numRows, numCols, start);
                    return;
                }

                if (!closedList[adjRow][adjCol]) {
                    int gNew = cellDetails[minRow][minCol].g + 1;
                    int hNew = calculateHValue(adjRow, adjCol, dest);
                    int fNew = gNew + hNew;

                    if (!openList[adjRow][adjCol] || cellDetails[adjRow][adjCol].f > fNew) {
                        openList[adjRow][adjCol] = true;
                        cellDetails[adjRow][adjCol] = (struct Cell){.row = minRow, .col = minCol, .g = gNew, .h = hNew, .f = fNew};
                    }
                }
            }
        }
    }

    freeMemory(grid, numRows, cellDetails, openList, closedList);
}

// Free dynamically allocated memory
void freeMemory(int **grid, int numRows, struct Cell **cellDetails, bool **openList, bool **closedList) {
    for (int i = 0; i < numRows; i++) {
        free(cellDetails[i]);
        free(openList[i]);
        free(closedList[i]);
    }
    free(cellDetails);
    free(openList);
    free(closedList);
}