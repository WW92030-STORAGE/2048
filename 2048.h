#ifndef H_2048
#define H_2048

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

class Game2048 {
    public:
    std::vector<std::vector<int>> board;
    int score = 0;
    
    Game2048() {
        board = std::vector<std::vector<int>>(4, std::vector<int>(4, 0));
    }
    
    Game2048(Game2048& other) {
        board = std::vector<std::vector<int>>(4, std::vector<int>(4, 0));
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) board[i][j] = other.board[i][j];
        }
        score = other.score;
    }
    
    bool operator<(Game2048& other) {
        if (score != other.score) return score < other.score;
        return board < other.board;
    }
    
    bool operator==(Game2048& other) {
        return board == other.board && score == other.score;
    }
    
    bool operator!=(Game2048& other) {
        return board != other.board || score != other.score;
    }
    
    bool inBounds(int x, int y) {
        if (x < 0 || y < 0) return false;
        if (x > 3 || y > 3) return false;
        return true;
    }
    
    bool inBounds(std::pair<int, int> p) {
        return inBounds(p.first, p.second);
    }
    
    bool placeRandom() {
        int value = (rand() % 2 == 0) ? 2 : 4; // Value of the tile
        
        std::vector<std::pair<int, int>> v;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) v.push_back({i, j});
        }
        
        std::random_shuffle(v.begin(), v.end());
        
        for (auto i : v) {
            if (board[i.first][i.second] == 0) {
                board[i.first][i.second] = value;
                return true;
            }
        }
        return false;
    }
    
    int dx[4] = {01, 00, -1, 00};
    int dy[4] = {00, 01, 00, -1};
    
    // Executes a move regardless of its effect. Does not spawn any new tiles.
    bool move(int d) {
        std::vector<int> v;
        if (d == 0) { // Increase in x
            for (int y = 0; y < 4; y++) {
                for (int x = 0; x < 4; x++) {
                    if (board[x][y] != 0) v.push_back(board[x][y]);
                    board[x][y] = 0;
                }
                int x = 3;
                while (v.size() > 0) {
                    int object = v[v.size() - 1];
                    if (v.size() > 1 && object == v[v.size() - 2]) {
                        v.pop_back();
                        v.pop_back();
                        v.push_back(object * 2);
                        score += object * 2;
                    }
                    board[x--][y] = v[v.size() - 1];
                    v.pop_back();
                }
            }
        }
        if (d == 2) { // Decrease along x
            for (int y = 0; y < 4; y++) {
                for (int x = 3; x >= 0; x--) {
                    if (board[x][y] != 0) v.push_back(board[x][y]);
                    board[x][y] = 0;
                }
                int x = 0;
                while (v.size() > 0) {
                    int object = v[v.size() - 1];
                    if (v.size() > 1 && object == v[v.size() - 2]) {
                        v.pop_back();
                        v.pop_back();
                        v.push_back(object * 2);
                        score += object * 2;
                    }
                    board[x++][y] = v[v.size() - 1];
                    v.pop_back();
                }
            }
        }
        if (d == 1) { // Increase along y
            for (int x = 0; x < 4; x++) {
                for (int y = 0; y < 4; y++) {
                    if (board[x][y] != 0) v.push_back(board[x][y]);
                    board[x][y] = 0;
                }
                int y = 3;
                while (v.size() > 0) {
                    int object = v[v.size() - 1];
                    if (v.size() > 1 && object == v[v.size() - 2]) {
                        v.pop_back();
                        v.pop_back();
                        v.push_back(object * 2);
                        score += object * 2;
                    }
                    board[x][y--] = v[v.size() - 1];
                    v.pop_back();
                }
            }
        }
        
        if (d == 3) { // Decrease along y
            for (int x = 0; x < 4; x++) {
                for (int y = 3; y >= 0; y--) {
                    if (board[x][y] != 0) v.push_back(board[x][y]);
                    board[x][y] = 0;
                }
                int y = 0;
                while (v.size() > 0) {
                    int object = v[v.size() - 1];
                    if (v.size() > 1 && object == v[v.size() - 2]) {
                        v.pop_back();
                        v.pop_back();
                        v.push_back(object * 2);
                        score += object * 2;
                    }
                    board[x][y++] = v[v.size() - 1];
                    v.pop_back();
                }
            }
        }
        
        return (d == 0) || (d == 1) || (d == 2) || (d == 3);
    }
    
    
    
    std::string toString() {
        std::string res = "";
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                std::string val = std::to_string(board[i][j]);
                while (val.length() < 4) val = val + " ";
                res = res + val + " ";
            }
            res = res + "\n";
        }
        return res;
    }
    void disp() { std::cout << toString() << "\n"; }
    
    bool gameOver() {
        Game2048 original(*this);
        for (int i = 0; i < 4; i++) {
            Game2048 test(*this);
            test.move(i);
            if (test != original) return false;
        }
        return true;
    }
    
    bool gameCleared() {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (board[i][j] >= 2048) return true;
            }
        }
        return false;
    }


	Game2048 rotateBoard() {
		Game2048 game2(*this);
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				int temp = board[i][j];
				board[i][j] = board[j][3 - i];
				board[j][3 - i] = board[3 - i][3 - j];
				board[3 - i][3 - j] = board[3 - j][i];
				board[3 - j][i] = temp;
			}
		}
		return game2;
	}

	Game2048 reflectBoard() {
		Game2048 game2(*this);
		for (int i = 0; i < 4; i++) {
			int temp = board[i][0];
			board[i][0] = board[i][3];
			board[i][3] = temp;
			temp = board[i][1];
			board[i][1] = board[i][2];
			board[i][2] = temp;
		}
		return game2;
	}
};

#endif