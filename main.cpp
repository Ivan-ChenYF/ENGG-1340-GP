#include "random_maze.h"
#include "game_state.h"
#include "player_control.h"
#include <iostream>
#include <ncurses.h>
#include <string>
using namespace std;
void classic_mode(cell** &maze,int width, int height, int player_x,int player_y,int start_time,double elapsed,int end_x,int end_y){
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            maze[i][j].revealed = true;
        }
    }
    clear();
    printMaze(maze, width, height, player_x, player_y,end_x,end_y);
    mvprintw(2*height+1, 0, "Time elapsed: %.0f seconds", elapsed);// Display elapsed time on screen
    refresh();
}
void fog_mode(cell** &maze,int width, int height, int player_x,int player_y,int start_time,double elapsed,int end_x,int end_y){
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
    printMaze(maze, width, height, player_x, player_y,end_x,end_y);
    mvprintw(2*height+1, 0, "Time elapsed: %.0f seconds", elapsed);// Display elapsed time on screen
    refresh();
    for (int i = minX; i <= maxX; i++) {
        for (int j = minY; j <= maxY; j++) {
            maze[i][j].revealed = false;
        }
    }
}

void difficult_level(int mode, int &width,int &height,int &start_x, int &start_y){
    if (mode=="easy"){
        width=height=10;
    }
    else if (mode=="medium"){
        width=height=15;
    }
    else if (mode=="difficult"){
        width=height=20
    }
    else{
        cout<<"Width: ";
        cin>>width;
        cout<<"Height: ";
        cin>>height;
        cout<<"start_x: ";
        cin>>start_x;
        cout<<"start_y: ";
        cin>>start_y;
    }
    srand(time(nullptr));
    start_x=rand()%width;
    start_y=rand()%height;
}
int main(){
    GameState game; // Initialize gameState
    vector<int> time_rank; // Initialize ranking
    
    setlocale(LC_ALL, "");
    
    // LOADING PAGE
    initscr();
    PrintFromFile("ASCII - Enigma_Maze.txt");
    getch();
    clear();
    endwin();

    // INPUT USERNAME
    initscr();
    PrintFromFile("ASCII - Create_User.txt");
    char ch;
    while ((ch = getch()) != '\n') {
        if (ch == KEY_BACKSPACE || ch == 127) {
            if (!game.player_name.empty()) {
                game.player_name.pop_back();
                move(getcury(stdscr), getcurx(stdscr) - 1);
                mvdelch(getcury(stdscr), getcurx(stdscr));
                refresh();
            }
        } else {
            game.player_name.push_back(ch);
            refresh();
        }
    }
    clear();
    endwin();

    cout << "Your user name is " << game.player_name << endl;

    int mode, difficulty;
    mode = choiceUI("mode");
    difficulty = choiceUI("difficulty");
    cout << "Your mode is " << mode << endl;
    
    // Initialize maze parameters
    int width;
    int height;
    int start_x,start_y;
    int end_x,end_y;
    difficult_level(difficulty,width,height,start_x,start_y);
    time_t start_time = time(nullptr);
    time_t current_time;
    cell** maze = new cell*[width]; //使用动态数组创建maze
    for (int i = 0; i < width; ++i) {
        maze[i] = new cell [height];
    }
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            init(&maze[i][j],i,j);
        }
    }
    initscr();          // Start ncurses mode
    cbreak();           // Disable line buffering
    noecho();           // Don't echo user input
    keypad(stdscr, TRUE); // Enable function keys
    curs_set(0);        // Hide the cursor
    timeout(100);     //100毫秒没有接收到新的输入返回ERR
    
    
    generateMaze(maze, width,height,start_x,start_y,end_x,end_y);
    printMaze(maze, width,height,start_x,start_y,end_x,end_y);

    


    int player_x = start_x;
    int player_y = start_y;


    while ((ch = getch()) != 'q') {
        

        current_time = time(nullptr);
        double elapsed = difftime(current_time, start_time);

        if (elapsed>50||(player_x==end_x&&player_y==end_y)){
            break;
        }
        if (mode==0){
            classic_mode(maze,width, height, player_x,player_y, start_time, elapsed, end_x, end_y);
        }
        else{
            fog_mode(maze,width, height, player_x,player_y, start_time, elapsed, end_x, end_y);
        }
        if (ch == ERR) continue;
        
        player_movement(maze,width,height,player_x,player_y,ch);
        
    }
    saveGame(game);
    updateRank(time_rank, elapsed);
    endwin();
    
    // Clean up
    for (int i = 0; i < width; i++) {
        delete[] maze[i];
    }
    delete[] maze;
    return 0;
}


