#include <algorithm>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <windows.h>

using std::cout;
using std::endl;
using std::ifstream;
using std::istringstream;
using std::string;
using std::vector;

enum class State {kEmpty, kObstacle, kClosed, kPath, kStart, kFinish};

const int delta[4][2]{{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

vector<State> Parseline(string s) {
    istringstream stream(s);
    vector<State> row;
    int num;
    char coma;

    while(stream >> num >> coma && coma == ',') {
        switch(num) {
            case 0:
                row.push_back(State::kEmpty);
                break;
            case 1:
                row.push_back(State::kObstacle);
                break;
        }
    }
    return row;
}

vector<vector<State>> ReadBoardFile(string path) {
    ifstream file;
    string line;
    vector<State> int_line;
    vector<vector<State>> board;

    file.open(path);
    if (file){
        while (getline(file, line)) {
            int_line = Parseline(line);
            board.push_back(int_line);
        }
    }
    return board;
}

bool Compare(const vector<int> a, const vector<int> b) {
    int f1 = a[2] + a[3]; // f1 = g1 + h1
    int f2 = b[2] + b[3]; // f2 = g2 + h2
    return f1 > f2; 
}

void CellSort(vector<vector<int>> *v) {
  sort(v->begin(), v->end(), Compare);
}

int Heuristic (int x1, int y1, int x2, int y2) {
    return abs(x2 - x1) + abs(y2 - y1);
}

bool CheckValidCell(int x, int y, vector<vector<State>> &grid) {
    bool esta_x = (x >= 0 && x < grid.size());
    bool esta_y = (y >= 0 && y < grid.size());
    if (esta_x && esta_x)
        return grid[x][y] == State::kEmpty; // true
    return false;
}

void AddToOpen(int x, int y, int g, int h, vector<vector<int>> &open_nodes, vector<vector<State>> &grid) {
    vector<int> coordinates = {x, y, g, h};

    open_nodes.push_back(coordinates);
    grid[x][y] = State::kClosed;
}

void ExpandNeighbors(vector<int> &current_node, vector<vector<int>> &open, vector<vector<State>> &grid, int goal[2]) {
    int x = current_node[0];
    int y = current_node[1];
    int g = current_node[2];

    for (int i = 0; i < 4; i++) {
        int x2 = x + delta[i][0];
        int y2 = y + delta[i][1];

        if (CheckValidCell(x2, y2, grid)) {
            // Increment g value and add neighbor to open list.
            int g2 = g + 1;
            int h2 = Heuristic(x2, y2, goal[0], goal[1]);
            AddToOpen(x2, y2, g2, h2, open, grid);
        }
    }
}

vector<vector<State>> Search(vector<vector<State>> grid, int init[2], int goal[2]) {
    vector<vector<int>> open {};
    int x = init[0], y = init[1];
    int g = 0;
    int h = Heuristic(x, y, goal[0], goal[1]);

    AddToOpen(x, y, g, h, open, grid);

    while(open.size() > 0) {            
        CellSort(&open);                
        auto current = open.back();  

        open.pop_back();
        x = current[0];
        y = current[1];                 
        grid[x][y] = State::kPath;

        if (x == goal[0] && y == goal[1]) {
            grid[init[0]][init[1]] = State::kStart;
            grid[goal[0]][goal[1]] = State::kFinish; 
            return grid;
        }
        ExpandNeighbors(current, open, grid, goal);
    }
    cout << "No path found: the goal landed on an obstacle." << endl;
    return vector<vector<State>> {};
}

string CellString(State state)
{
    switch (state) {
        case State::kObstacle:
            return " ⛰  ";
            break;
        case State::kPath:
            return "🚗 ";
            break;
        case State::kStart:
            return "🚦 ";
            break;
        case State::kFinish:
            return "🏁 ";
            break;
        default: 
            return " 0 ";
    }
}

void PrintBoard(const vector<vector<State>> tablerito) {
    for (int i = 0; i < tablerito.size(); i++) {
        for (int j = 0; j < tablerito[i].size(); j++) {
            cout << CellString(tablerito[i][j]);
        }
        cout << endl;
    }
}

int main() {
    SetConsoleOutputCP(CP_UTF8);

    int init[2] {0, 0};
    int goal[2];
    
    cout << "Enter first goal coordinate: " << endl;
    std::cin >> goal[0];
    while (goal[0] > 4) {
        cout << "The coordinate must be between 0 - 4." << endl;
        cout << "Enter first goal coordinate: " << endl;
        std::cin >> goal[0];
    }
    cout << "Enter second goal coordinate: " << endl;
    std::cin >> goal[1];
    while (goal [1] > 5) {
        cout << "The coordinate must be between 0 - 5." << endl;
        cout << "Enter second goal coordinate: " << endl;
        std::cin >> goal[1];
    }

    auto board = ReadBoardFile("files/1.board");
    auto solution = Search(board, init, goal);
    PrintBoard(solution);
}
