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
    Inventory inventory;
};

vector<int> time_rank;

void initializeNewGame(GameState& gameState);
void saveGame(const GameState& gameState, const string& filename);
void loadGame(const GameState& gameState, const string& filename);

#endif
