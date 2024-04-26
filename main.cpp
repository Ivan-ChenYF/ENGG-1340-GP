#include "random_maze.h"
#include "game_state.h"
#include "player_control.h"
#include "user_interface.h"
#include <iostream>
#include <ncurses.h>
#include <string>
#include <unistd.h>
using namespace std;

void classic_mode(cell** &maze,int width, int height, int player_x,int player_y,int start_time,double elapsed,int end_x,int end_y,int timelimit,int bomb){
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            maze[i][j].revealed = true;
        }
    }
    clear();
    printMaze(maze, width, height, player_x, player_y,end_x,end_y,bomb,timelimit-elapsed);
    refresh();
}
void fog_mode(cell** &maze,int width, int height, int player_x,int player_y,int start_time,double elapsed,int end_x,int end_y, int timelimit,int bomb){
    int centerIndex = 2; // 5x5区域的中心索引
    int displaySize = 5; // 显示区域尺寸
    int minX = max(0, player_x - centerIndex);
    int maxX = min(width - 1, player_x + centerIndex);
    int minY = max(0, player_y - centerIndex);
    int maxY = min(height - 1, player_y + centerIndex);

    // 确保显示区域为5x5，如果接近边界，则调整
    if (maxX - minX < displaySize - 1) {
        if (minX == 0) maxX = displaySize - 1; // 靠近左边界
        if (maxX == width - 1) minX = width - displaySize; // 靠近右边界
    }
    if (maxY - minY < displaySize - 1) {
        if (minY == 0) maxY = displaySize - 1; // 靠近上边界
        if (maxY == height - 1) minY = height - displaySize; // 靠近下边界
    }

    // 设置显示区域
    for (int i = minX; i <= maxX; i++) {
        for (int j = minY; j <= maxY; j++) {
            maze[i][j].revealed = true;
        }
    }


    clear();
    printMaze(maze, width, height, player_x, player_y,end_x,end_y,bomb,timelimit-elapsed);
    refresh();
    for (int i = minX; i <= maxX; i++) {
        for (int j = minY; j <= maxY; j++) {
            maze[i][j].revealed = false;
        }
    }
}

void difficult_level(int mode, int &width,int &height,int &start_x, int &start_y,int &timelimit,int &bomb, string username){
    if (mode==0){
        width=height=10;
        timelimit=60;
        bomb=1;
    }
    else if (mode==1){
        width=height=15;
        timelimit=100;
        bomb=2;
    }
    else if (mode==2){
        width=height=20;
        timelimit=140;
        bomb=3;
    }
    else if (mode==3){
        customize(width, height, timelimit, bomb, username);
    }

    srand(time(nullptr));
    start_x=rand()%width;
    start_y=rand()%height;
}
int main(){
    int bomb=0;
    int timelimit;
    int ch;
    // Initialize maze parameters
    int width;
    int height;
    int start_x,start_y;
    int end_x,end_y;
    int difficulty;
    int mode;
    cell** maze=nullptr;
    double elapsed;
    double time_used;
    int player_x,player_y;
    GameState game; // Initialize gameState
    vector<double> time_rank; // Initialize ranking
    
    // LOADING PAGE
    setlocale(LC_ALL, "");
    initscr();
    PrintFromFile("ASCII - Enigma_Maze.txt");
    getch();
    clear();
    refresh();
    
    // INPUT USERNAME
    
    PrintFromFile("ASCII - Create_User.txt");
    while ((ch = getch()) != '\n') {
        if (ch == KEY_BACKSPACE || ch == 127) {
            if (!game.player_name.empty()) {
                game.player_name.pop_back();
                refresh();
            }
        } else {
            game.player_name.push_back(ch);
            refresh();
        }
        clear();
        PrintFromFile("ASCII - Create_User.txt");
        printw(game.player_name.c_str());
    }
    clear();
    refresh();
    
    cout << "Your user name is " << game.player_name << endl;
    bool gamestate=true;
    while (gamestate){
        mode = choiceUI(MODE, game.player_name);
        if (mode==0||mode==1){
            difficulty = choiceUI(DIFFICULTY, game.player_name);
            difficult_level(difficulty,width,height,start_x,start_y,timelimit,bomb, game.player_name);
            maze = new cell*[width]; //使用动态数组创建maze
            for (int i = 0; i < width; ++i) {
                maze[i] = new cell [height];
            }
            for (int i = 0; i < width; i++) {
                for (int j = 0; j < height; j++) {
                    init(&maze[i][j],i,j);
                }
            }
            generateMaze(maze, width,height,start_x,start_y,end_x,end_y);
            time_used=0;
        }
        else if (mode ==2){
            loadGame(game);  // 确保 game 是一个 GameState 类型的实例

            // After loading, we might need to setup the maze structure
            maze = new cell*[game.width];  // 动态分配空间
            for (int i = 0; i < game.width; ++i) {
                maze[i] = new cell[game.height];
            }

            // Initialize the maze cells with the loaded data
            for (int i = 0; i < game.width; ++i) {
                for (int j = 0; j < game.height; ++j) {
                    init(&maze[i][j], i, j);  // Assuming init() can set up each cell
                    // Alternatively, directly copy the data if available
                    maze[i][j].top_w = game.maze[i][j].top_w;  // Assuming deep copy is possible or necessary
                    maze[i][j].right_w = game.maze[i][j].right_w;
                    maze[i][j].left_w = game.maze[i][j].left_w;
                    maze[i][j].down_w = game.maze[i][j].down_w;
                    maze[i][j].revealed = game.maze[i][j].revealed;
                }
            }
            mode=game.mode;
            start_x=game.player_x;
            start_y=game.player_y;
            time_used=game.elapsed;
        }
    
        else if (mode == 3){
            PrintFromFile("ASCII - End.txt");
            usleep(2000000);
            endwin();
            break;
        }



        
        
        time_t start_time = time(nullptr);
        time_t current_time;

        

        
        
        initscr();          // Start ncurses mode
        cbreak();           // Disable line buffering
        noecho();           // Don't echo user input
        keypad(stdscr, TRUE); // Enable function keys
        curs_set(0);        // Hide the cursor
        timeout(100);     //100毫秒没有接收到新的输入返回ERR
        
        
        
        
        
        player_x = start_x;
        player_y = start_y;
        
        

        
        while ((ch = getch()) != 'q') {
            
            
            current_time = time(nullptr);
            elapsed= difftime(current_time, start_time)+time_used;
            
            if (elapsed>timelimit){
                init_pair(1, COLOR_RED, COLOR_WHITE); // 定义颜色对，红色前景，白色背景
                attron(COLOR_PAIR(1));  // 使用红色前景和白色背景
                usleep(100000);
                clear();
                PrintFromFile("ASCII - Try_Again.txt");
                usleep(2000000);
                attroff(COLOR_PAIR(1));
                break;
            }
            if (player_x==end_x&&player_y==end_y){
                init_pair(1, COLOR_RED, COLOR_WHITE); // 定义颜色对，红色前景，白色背景
                attron(COLOR_PAIR(1));  // 使用红色前景和白色背景
                usleep(100000);
                clear();
                PrintFromFile("ASCII - Well_Done.txt");
                usleep(2000000);
                attroff(COLOR_PAIR(1));
                
                break;
            }
            if (mode==0){
                classic_mode(maze,width, height, player_x,player_y, start_time, elapsed, end_x, end_y,timelimit,bomb);
            }
            else{
                fog_mode(maze,width, height, player_x,player_y, start_time, elapsed, end_x, end_y, timelimit,bomb);
            }
            if (ch == ERR) continue;
            
            player_movement(maze,width,height,player_x,player_y,ch,bomb);
            
        }
        // Get gameState
        game.mode = mode;
        game.difficulty = difficulty;
        game.maze = maze;
        game.width = width;
        game.height = height;
        game.player_x = player_x;
        game.player_y = player_y;
        game.end_x = end_x;
        game.end_y = end_y;
        game.bomb=bomb;
        game.elapsed=elapsed;
        saveGame(game);

        endwin();
    }
    // Clean up

    return 0;
}
