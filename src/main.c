# include <time.h>
# include <stdio.h>
# include <stdlib.h>


typedef struct Block {
    unsigned short int n_bombs; // Number of adjacent bombs
    unsigned short int is_bomb; // 1 if it's a bomb and 0 otherwise
    unsigned short int visible; // 1 if it's visible for the player and 0 otherwise
} Block, *BlockPtr;

typedef struct Grid {
    BlockPtr blocks;
    int n_rows;
    int n_opened;
} Grid, *GridPtr;

void incrementAdjacent(BlockPtr grid, int n_rows, int spot) {
    int possible_movements[] = {-1, 0, 1};
    int bomb_spot[] = {spot/n_rows, spot%n_rows};

    for (int i = 0; i < 3; ++i) {
        int current_row = bomb_spot[0] + possible_movements[i];
        for (int j = 0; j < 3; ++j) {
            int current_col = bomb_spot[1] + possible_movements[j];
            if ((current_row >= 0 && current_row < n_rows) &&
                (current_col >= 0 && current_col < n_rows) &&
                (current_row != bomb_spot[0] || current_col != bomb_spot[1]) &&
                (grid[n_rows*current_row + current_col].is_bomb == 0))
            {
                grid[n_rows*current_row + current_col].n_bombs++;
            }
        }
    }
}

void generateBlocks(GridPtr grid) {
    int grid_size = grid->n_rows * grid->n_rows;
    grid->blocks = (BlockPtr)calloc(grid_size, sizeof(Block));
    int n_bombs = 0;

    while (n_bombs != 10) {
        int bomb_spot = rand() % grid_size;
        if (grid->blocks[bomb_spot].is_bomb == 0) {
            grid->blocks[bomb_spot].is_bomb = 1;
            n_bombs++;
            incrementAdjacent(grid->blocks, grid->n_rows, bomb_spot);
        }
    }
}

GridPtr initGrid(int n_rows, int n_mines) {
    GridPtr grid = (GridPtr)malloc(sizeof(Grid));
    grid->n_opened = 0;
    grid->n_rows = n_rows;

    generateBlocks(grid);

    return grid;
}

void printGrid(GridPtr grid) {
    for (int i = 0; i < grid->n_rows; ++i) {
        for (int j = 0; j < grid->n_rows; ++j) {
            int current_idx = grid->n_rows*i + j;
            if (grid->blocks[current_idx].is_bomb) {
                printf("X ");
            } else 
                printf("%d ", grid->blocks[grid->n_rows*i + j].n_bombs);
        }
        puts("");
    }
}

void freeGrid(GridPtr *grid) {
    free((*grid)->blocks);
    free(*grid);
}

int main() {
    srand(time(NULL));
    int n_rows = 8, n_mines = 10;
    
    GridPtr grid = initGrid(n_rows, n_mines);
    printGrid(grid);
    freeGrid(&grid);

    return 0;
}
