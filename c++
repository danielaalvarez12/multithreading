#include "raylib.h"
#include <thread>
#include <vector>
#include <mutex>
#include <cstdlib>
#include <ctime>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int GRID_ROWS = 30;
const int GRID_COLS = 40;
const int CELL_WIDTH = SCREEN_WIDTH / GRID_COLS;
const int CELL_HEIGHT = SCREEN_HEIGHT / GRID_ROWS;

struct Cell {
    Color color = BLACK;
};

std::vector<std::vector<Cell>> grid(GRID_ROWS, std::vector<Cell>(GRID_COLS));
std::mutex gridMutex;

// Thread function for Part 1 (drawing freely)
void drawRandomCells(Color threadColor) {
    while (true) {
        int row = rand() % GRID_ROWS;
        int col = rand() % GRID_COLS;

        std::lock_guard<std::mutex> lock(gridMutex);
        grid[row][col].color = threadColor;

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

// Thread function for Part 2 (interaction)
void drawWithInteraction(Color threadColor) {
    while (true) {
        int row = rand() % GRID_ROWS;
        int col = rand() % GRID_COLS;

        std::lock_guard<std::mutex> lock(gridMutex);
        
        // Check neighbors
        int sameColorNeighbors = 0;
        for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
                int r = row + dr;
                int c = col + dc;
                if (r >= 0 && r < GRID_ROWS && c >= 0 && c < GRID_COLS) {
                    if (grid[r][c].color.r == threadColor.r &&
                        grid[r][c].color.g == threadColor.g &&
                        grid[r][c].color.b == threadColor.b) {
                        sameColorNeighbors++;
                    }
                }
            }
        }

        // Interaction rule: if neighbors have same color, change this cell to white
        if (sameColorNeighbors > 2) {
            grid[row][col].color = WHITE;
        } else {
            grid[row][col].color = threadColor;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int main() {
    srand(time(0));
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Multithreading Grid Painting");

    // Part 1: four threads, two colors
    Color colors[4] = { RED, GREEN, BLUE, YELLOW };
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; i++) {
        threads.push_back(std::thread(drawRandomCells, colors[i]));
    }

    // Uncomment the following lines for Part 2 instead of Part 1
    /*
    Color interactColors[4] = { RED, GREEN, BLUE, YELLOW };
    for (int i = 0; i < 4; i++) {
        threads.push_back(std::thread(drawWithInteraction, interactColors[i]));
    }
    */

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        // Draw grid
        gridMutex.lock();
        for (int r = 0; r < GRID_ROWS; ++r) {
            for (int c = 0; c < GRID_COLS; ++c) {
                DrawRectangle(c * CELL_WIDTH, r * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT, grid[r][c].color);
            }
        }
        gridMutex.unlock();

        EndDrawing();
    }

    // Close threads (in a real assignment, you'd want a proper stop signal)
    for (auto& t : threads) t.detach();

    CloseWindow();
    return 0;
}
