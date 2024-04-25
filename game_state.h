#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include "random_maze.h"
using namespace std;

struct Item {
    string name;
    int quantity;
};
using Inventory = vector<Item>;

struct GameState {
    cell** maze;
    string player_name;
    int width, height;
    int player_x, player_y;
    int mode, difficulty;
    Inventory inventory;
    double elapsed;
};

void initializeNewGame(GameState& gameState);
void updateRank(vector<double>& rank, double time);
void saveGame(const GameState& gameState);
void loadGame(const GameState& gameState);

#endif
