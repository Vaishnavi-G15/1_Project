#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

// Structure to represent a cell in the grid
struct Cell {
    int row, col; // Row and column indices of the cell
    int f, g, h;  // f = g + h, g = cost from start to current cell, h = heuristic cost from current cell to destination
};

// Function to check if a given cell (row, col) is valid or not
bool isValid(int row, int col, int numRows, int numCols) {
    return (row >= 0) && (row < numRows) && (col >= 0) && (col < numCols);
}

// Function to check if a given cell is blocked or not
bool isUnblocked(int **grid, int row, int col) {
    return grid[row][col] == 0;
}

// Function to check if the destination cell has been reached
bool isDestination(int row, int col, struct Cell dest) {
    return (row == dest.row && col == dest.col);
}

// Function to calculate the heuristic value (Euclidean distance) from the current cell to the destination cell
int calculateHValue(int row, int col, struct Cell dest) {
    return ((row - dest.row) * (row - dest.row)) + ((col - dest.col) * (col - dest.col));
}

// Function to trace the path from the start cell to the destination cell
void tracePath(int**grid, struct Cell **cellDetails, struct Cell dest, int numRows, int numCols, struct Cell start) {
    // printf("\nThe path is:\n");
    FILE *fp2;
    fp2 = fopen("sol.txt", "w");
    int row = dest.row;
    int col = dest.col;

    while (!(cellDetails[row][col].row == row && cellDetails[row][col].col == col)) {
        grid[row][col] = 2;
        int temp_row = cellDetails[row][col].row;
        int temp_col = cellDetails[row][col].col;
        row = temp_row;
        col = temp_col;
    }
    grid[row][col] = 2;
    
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            if (i == start.row && j == start.col)
            {
                fprintf(fp2, "%c", 's');
            }
            else if (i == dest.row && j == dest.col)
            {
                fprintf(fp2, "%c", 'g');
            }
            else if(grid[i][j] == 0){
				fprintf(fp2, "%c", ' ');
			}
            else if(grid[i][j] == 1){
				fprintf(fp2, "%c", 'x');
			}
            else if(grid[i][j] == 2){
				fprintf(fp2, "%c", '.');
			}
        }
        fprintf(fp2, "%c", '\n');
    }

}

// Function to implement the A* algorithm
// Function to implement the A* algorithm without diagonal movement
void aStarSearch(int **grid, int numRows, int numCols, struct Cell start, struct Cell dest) {
    // Array to hold details of each cell
    struct Cell **cellDetails = (struct Cell **)malloc(numRows * sizeof(struct Cell *));
    for (int i = 0; i < numRows; i++) {
        cellDetails[i] = (struct Cell *)malloc(numCols * sizeof(struct Cell));
    }

    // Closed list to track visited cells
    bool **closedList = (bool **)malloc
    (numRows * sizeof(bool *));
    for (int i = 0; i < numRows; i++) {
        closedList[i] = (bool *)calloc(numCols, sizeof(bool));
    }

    // Initialize all cell details
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            cellDetails[i][j].f = INT_MAX;
            cellDetails[i][j].g = INT_MAX;
            cellDetails[i][j].h = INT_MAX;
            cellDetails[i][j].row = -1;
            cellDetails[i][j].col = -1;
        }
    }

    // Initialize the starting cell details
    int i = start.row;
    int j = start.col;
    cellDetails[i][j].f = 0;
    cellDetails[i][j].g = 0;
    cellDetails[i][j].h = 0;
    cellDetails[i][j].row = i;
    cellDetails[i][j].col = j;

    // Create lists for open cells
    bool **openList = (bool **)malloc(numRows * sizeof(bool *));
    for (int i = 0; i < numRows; i++) {
        openList[i] = (bool *)calloc(numCols, sizeof(bool));
    }
    openList[i][j] = true;

    // Main loop
    while (true) {
        int minF = INT_MAX;
        int minIndex_i = -1, minIndex_j = -1;

        // Find the cell with the minimum f value from the open list
        for (int i = 0; i < numRows; i++) {
            for (int j = 0; j < numCols; j++) {
                if (openList[i][j] && cellDetails[i][j].f < minF) {
                    minF = cellDetails[i][j].f;
                    minIndex_i = i;
                    minIndex_j = j;
                }
            }
        }

        // No path exists
        if (minIndex_i == -1 || minIndex_j == -1) {
            printf("Destination not reachable\n");
            return;
        }

        // Remove the selected cell from the open list and add to the closed list
        openList[minIndex_i][minIndex_j] = false;
        closedList[minIndex_i][minIndex_j] = true;

        // Check all the adjacent cells (up, down, left, right)
        int newRow, newCol;
        int rowOffsets[] = {-1, 1, 0, 0};
        int colOffsets[] = {0, 0, -1, 1};
        for (int k = 0; k < 4; k++) {
            newRow = minIndex_i + rowOffsets[k];
            newCol = minIndex_j + colOffsets[k];

            // If the cell is valid, not blocked, and not in the closed list
            if (isValid(newRow, newCol, numRows, numCols) && isUnblocked(grid, newRow, newCol) && !closedList[newRow][newCol]) {
                int gNew, hNew, fNew;

                gNew = cellDetails[minIndex_i][minIndex_j].g + 1; // Cost of moving to the next cell is 1
                hNew = calculateHValue(newRow, newCol, dest);
                fNew = gNew + hNew;

                // If the cell is not in the open list, add it to the open list
                // and update its details
                if (cellDetails[newRow][newCol].f == INT_MAX || cellDetails[newRow][newCol].f > fNew) {
                    openList[newRow][newCol] = true;
                    cellDetails[newRow][newCol].f = fNew;
                    cellDetails[newRow][newCol].g = gNew;
                    cellDetails[newRow][newCol].h = hNew;
                    cellDetails[newRow][newCol].row = minIndex_i;
                    cellDetails[newRow][newCol].col = minIndex_j;
                }
            }
        }

        // If the destination is reached, trace and print the path
        if (isDestination(minIndex_i, minIndex_j, dest)) {
            printf("Destination found!\n");
            tracePath(grid, cellDetails, dest, numRows, numCols, start);
            return;
        }
    }
}

int main() {
    int numRows, numCols;
    char temp;
    // printf("Enter the number of rows and columns: ");
    FILE *fp1;
    fp1 = fopen("maze.txt", "r");
    fscanf(fp1, "%d,%d", &numRows, &numCols);
    printf("%d %d\n", numRows, numCols);

    // Dynamically allocate memory for the grid
    int **grid = (int **)malloc(numRows * sizeof(int *));
    for (int i = 0; i < numRows; i++) {
        grid[i] = (int *)malloc(numCols * sizeof(int));
    }

    // Input the grid
    // printf("Enter the grid (0 for unblocked, 1 for blocked):\n");
    struct Cell start;
    struct Cell dest;
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            fscanf(fp1, "%c", &temp);
			if(temp == ' '){
				grid[i][j] = 0;
			}
            else if(temp == 'x'){
				grid[i][j] = 1;
			}
            else if(temp == 's'){
				start.row = i;
                start.col = j;
                grid[i][j] = 0;
			}
            else if(temp == 'g'){
				dest.row = i;
                dest.col = j;
                grid[i][j] = 0;
			}
            else if(temp == '\n'){
				j--;
			}
        }
    }

    for (int k = 0; k < numRows; k++)
    {
        for (int l = 0; l < numCols; l++)
        {
            printf("%d ", grid[k][l]);
        }
        printf("\n");
    }
    
    // Find the path
    aStarSearch(grid, numRows, numCols, start, dest);

    // Free dynamically allocated memory
    for (int i = 0; i < numRows; i++) {
        free(grid[i]);
    }
    free(grid);

    return 0;
}
