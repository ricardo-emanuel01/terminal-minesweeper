# include <time.h>
# include <stdio.h>
# include <stdlib.h>


typedef enum {
    SUCCESS,
    ALREADY_VISIBLE,
    FAIL,
    OUT_OF_BOUNDS,
} OpenStatus;

typedef enum {
    ALIVE,
    LOSE,
    WIN,
    QUIT,
} GameState;

typedef struct Block {
    unsigned short int n_mines; // Number of adjacent mines
    unsigned short int is_mine; // 1 if it's a mine and 0 otherwise
    unsigned short int visible; // 1 if it's visible for the player and 0 otherwise
} Block, *BlockPtr;

typedef struct Grid {
    BlockPtr blocks;
    int n_rows;
    int n_opened;
    int n_mines; // Number of adjacent mines
} Grid, *GridPtr;

void incrementAdjacent(BlockPtr grid, int n_rows, int spot) {
    int possible_movements[] = {-1, 0, 1};
    int mine_spot[] = {spot/n_rows, spot%n_rows};

    for (int i = 0; i < 3; ++i) {
        int current_row = mine_spot[0] + possible_movements[i];
        for (int j = 0; j < 3; ++j) {
            int current_col = mine_spot[1] + possible_movements[j];
            if ((current_row >= 0 && current_row < n_rows) &&
                (current_col >= 0 && current_col < n_rows) &&
                (current_row != mine_spot[0] || current_col != mine_spot[1]) &&
                (grid[n_rows*current_row + current_col].is_mine == 0))
            {
                grid[n_rows*current_row + current_col].n_mines++;
            }
        }
    }
}

void generateBlocks(GridPtr grid) {
    int grid_size = grid->n_rows * grid->n_rows;
    grid->blocks = (BlockPtr)calloc(grid_size, sizeof(Block));
    int n_mines = 0;

    while (n_mines != grid->n_mines) {
        int mine_spot = rand() % grid_size;
        if (grid->blocks[mine_spot].is_mine == 0) {
            grid->blocks[mine_spot].is_mine = 1;
            n_mines++;
            incrementAdjacent(grid->blocks, grid->n_rows, mine_spot);
        }
    }
}

GridPtr initGrid(int n_rows, int n_mines) {
    GridPtr grid = (GridPtr)malloc(sizeof(Grid));
    grid->n_opened = 0;
    grid->n_rows = n_rows;
    grid->n_mines = n_mines;

    generateBlocks(grid);

    return grid;
}

int openBlocks(GridPtr grid, int row, int col) {
    if (row < 0 || col < 0 ||
        row >= grid->n_rows || col >= grid->n_rows)
    {
        return OUT_OF_BOUNDS;
    }

    int current_idx = grid->n_rows * row + col;
    if (grid->blocks[current_idx].visible == 1) {
        return ALREADY_VISIBLE;
    }

    if (grid->blocks[current_idx].is_mine == 1) {
        return FAIL;
    }

    grid->blocks[current_idx].visible = 1;
    if (grid->blocks[current_idx].n_mines == 0) {
        int possible_movements[] = {-1, 0, 1};
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                openBlocks(grid, row+possible_movements[i], col+possible_movements[j]);
            }
        }
    }

    grid->n_opened++;
    return SUCCESS;
}

void printGrid(GridPtr grid) {
    for (int i = 0; i < grid->n_rows; ++i) {
        for (int j = 0; j < grid->n_rows; ++j) {
            int current_idx = grid->n_rows*i + j;
            if (grid->blocks[current_idx].visible == 1) {
                if (grid->blocks[current_idx].is_mine) {
                    printf("B ");
                } else 
                    printf("%d ", grid->blocks[grid->n_rows*i + j].n_mines);
            } else {
                printf("X ");
            }
        }
        puts("");
    }
}

void turnAllVisible(GridPtr grid) {
    for (int i = 0; i < grid->n_rows*grid->n_rows; ++i) {
        grid->blocks[i].visible = 1;
    }
}

void freeGrid(GridPtr *grid) {
    free((*grid)->blocks);
    free(*grid);
}

int main() {
    srand(time(NULL));
    int n_rows = 8, n_mines = 10;
    int row, col, game_state = 0;
    
    GridPtr grid = initGrid(n_rows, n_mines);
    printGrid(grid);

    while (game_state == ALIVE) {
        puts("Type a row and a colums <x, y> or 10 to quit.");
        scanf("%d, %d", &row, &col);
        if (row == 10) {
            game_state = QUIT;
            break;
        }

        int open_result = openBlocks(grid, row, col);
        if (open_result == FAIL) {
            game_state = LOSE;
            break;
        } else if (open_result == OUT_OF_BOUNDS || open_result == ALREADY_VISIBLE) {
            puts("Type a possible coordinate.");
        } else {
            if (grid->n_opened+n_mines == n_rows*n_rows) {
                game_state = WIN;
                break;
            }
        }

        printGrid(grid);
    }

    if (game_state == WIN) {
        puts("Congrats! You won.");
    } else if (game_state == LOSE) {
        puts("Try again!");
    }

    turnAllVisible(grid);
    printGrid(grid);
    freeGrid(&grid);

    return 0;
}
