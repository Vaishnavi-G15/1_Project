#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

/* ================================================================= */
/*           METHODS AND STRUCTS USED IN THE PROGRAM                 */
/* ================================================================= */
typedef struct {
    int x, y;
} Coordinate;

Coordinate new_coordinate(int x, int y) {
    Coordinate ret = {x, y};
    return ret;
}

/* returns a coordinate with -1 as x and -1 as y if the entrance is not found */
Coordinate get_entrance(char** m, int lines, int columns) {
    int i;
    Coordinate entrance = {-1, -1};

    for (i = 0; i < lines; i++) {
        if (m[i][0] == 'I') {
            entrance = new_coordinate(i, 0);
            return entrance;
        }
        if (m[i][columns - 1] == 'I') {
            entrance = new_coordinate(i, columns - 1);
            return entrance;
        }
    }
    for (i = 0; i < columns; i++) {
        if (m[0][i] == 'I') {
            entrance = new_coordinate(0, i);
            return entrance;
        }
        if (m[lines - 1][i] == 'I') {
            entrance = new_coordinate(lines - 1, i);
        }
    }

    return entrance; /* did not find an entrance */
}

char equals_coordinates(Coordinate coord1, Coordinate coord2) {
    return (coord1.x == coord2.x && coord1.y == coord2.y) ? 1 : 0;
}

void print_maze(char** maze, int lines, int columns) {
    int i, j;
    for (i = 0; i < lines; i++) {
        for (j = 0; j < columns; j++) {
            printf("%c", maze[i][j]);
        }
        printf("\n");
    }
}

/* Changes the array given as parameter to the possible ways the maze can continue */
/* The array must have 4 spaces */
void possibilities(Coordinate current, char** maze, Coordinate* poss_ways,
                   int lines, int columns) {
    /* Down */
    poss_ways[0] = (current.x < lines - 1 && (maze[current.x + 1][current.y] == ' ' || maze[current.x + 1][current.y] == 'O'))
                        ? new_coordinate(current.x + 1, current.y)
                        : new_coordinate(-1, -1);

    /* Up */
    poss_ways[1] = (current.x > 0 && (maze[current.x - 1][current.y] == ' ' || maze[current.x - 1][current.y] == 'O'))
                        ? new_coordinate(current.x - 1, current.y)
                        : new_coordinate(-1, -1);

    /* Left */
    poss_ways[2] = (current.y > 0 && (maze[current.x][current.y - 1] == ' ' || maze[current.x][current.y - 1] == 'O'))
                        ? new_coordinate(current.x, current.y - 1)
                        : new_coordinate(-1, -1);

    /* Right */
    poss_ways[3] = (current.y < columns - 1 && (maze[current.x][current.y + 1] == ' ' || maze[current.x][current.y + 1] == 'O'))
                        ? new_coordinate(current.x, current.y + 1)
                        : new_coordinate(-1, -1);
}

char stop_program() {
    char answer;
    printf("\nWould you like to stop the program? (Y/N)\n");
    scanf(" %c", &answer);
    return toupper(answer);
}

/* ================================================================= */
/*                   MAIN AND SOLVER FUNCTIONS                       */
/* ================================================================= */

/* Recursively solve the maze */
Coordinate solve(Coordinate current, char** maze, int lines, int columns) {
    if (current.x == -1) return current;
    if (maze[current.x][current.y] == 'O') return new_coordinate(-2, -2);
    maze[current.x][current.y] = '*';

    Coordinate poss[4], answer;
    possibilities(current, maze, poss, lines, columns);

    for (int i = 0; i < 4; i++) {
        answer = solve(poss[i], maze, lines, columns);
        if (answer.x == -2) return answer;
    }

    maze[current.x][current.y] = ' '; // backtrack
    return current;
}

int main(void) {
    char save, pathToFile[256];
    char** maze;
    int nLines, nColumns, i, j;
    FILE* fil;

    printf("=======================================\n");
    printf(" Maze Solver - by github.com/edusporto \n");
    printf("=======================================\n");

    do {
        /* READING FILE CONTAINING THE MAZE */
        printf("\nPlease write the path to a file containing a maze:\n");
        scanf("%255s", pathToFile);

        if ((fil = fopen(pathToFile, "r")) == NULL) {
            printf("\nCould not open file.\n");
            continue;
        }

        /* READING DIMENSIONS */
        fscanf(fil, "%d", &nLines);
        fscanf(fil, "%d", &nColumns);

        /* ALLOCATING THE MATRIX */
        maze = (char**)malloc(sizeof(char*) * nLines);
        for (i = 0; i < nLines; i++)
            maze[i] = (char*)malloc(sizeof(char) * nColumns);

        /* FILLING THE MATRIX */
        fgetc(fil);
        for (i = 0; i < nLines; i++) {
            for (j = 0; j < nColumns; j++) {
                maze[i][j] = fgetc(fil);
            }
            fgetc(fil);
        }
        fclose(fil);

        /* GET ENTRANCE AND SOLVE */
        Coordinate entrance_to_maze = get_entrance(maze, nLines, nColumns);
        Coordinate exit_to_maze = solve(entrance_to_maze, maze, nLines, nColumns);

        if (equals_coordinates(entrance_to_maze, exit_to_maze))
            printf("\nThere is no answer to this maze.\n");
        else {
            maze[entrance_to_maze.x][entrance_to_maze.y] = 'I';
            printf("\nThere is an answer to this maze:\n");
            print_maze(maze, nLines, nColumns);

            printf("\nWould you like to save the solved maze to a file? (Y/N)\n");
            scanf(" %c", &save);
            if (toupper(save) == 'Y') {
                printf("\nPlease write the path to save the solved maze:\n");
                scanf("%255s", pathToFile);

                if ((fil = fopen(pathToFile, "w")) == NULL) {
                    printf("\nCould not open file.\n");
                } else {
                    fprintf(fil, "%d\n%d\n", nLines, nColumns);
                    for (i = 0; i < nLines; i++) {
                        for (j = 0; j < nColumns; j++)
                            fprintf(fil, "%c", maze[i][j]);
                        fprintf(fil, "\n");
                    }
                    fclose(fil);
                }
            }
        }

        /* DEALLOCATE THE MAZE */
        for (i = 0; i < nLines; i++)
            free(maze[i]);
        free(maze);

    } while (stop_program() != 'Y');

    return 0;
}
