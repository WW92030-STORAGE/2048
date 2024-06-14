#ifndef GENETIC_H
#define GENETIC_H

#include <vector>
#include <algorithm>
#include <iostream>
#include <climits>
#include <cfloat>
#include <cstdint>

#include "2048.h"

class AI2048 {
    public:
    double emptytile = 0.5;
	double rough = -2;
	double matsum = 4; // Weighted sum
    double rinv = 1;
    double flatboard = -10; // Penalty of a flat board (which forces a movement in the "wrong" direction)
    double trueflat = -100; // Penalty of a 3x4 flat board
    long long INF = 100000000000000; // 1e14
    int ply = 2;

    std::string toString() {
        std::string res = "EMPTY " + std::to_string(emptytile) + " ROUGH " + std::to_string(rough) + " MATSUM " + std::to_string(matsum);
        res = res + " RINV " + std::to_string(rinv) + " FLAT " + std::to_string(flatboard) + " TRUEFLAT " + std::to_string(trueflat);
        return res;
    }
    
    bool operator<(AI2048& o) {
        std::vector<double> v1 = {emptytile, rough, matsum, rinv, flatboard, trueflat};
        std::vector<double> v2 = {o.emptytile, o.rough, o.matsum, o.rinv, o.flatboard, o.trueflat};
        return v1 < v2;
    }
    
    AI2048() { }
    AI2048(double em, double ro, double ma, double ri, double fl, double tr) {
        emptytile = em;
        rough = ro;
        matsum = ma;
        rinv = ri;
        flatboard = fl;
        trueflat = tr;
    }

	AI2048(const AI2048& a) {
		emptytile = a.emptytile;
		rough = a.rough;
		matsum = a.matsum;
        rinv = a.rinv;
        flatboard = a.flatboard;
        trueflat = a.trueflat;
	}
    
    int dx[4] = {01, 00, -1, 00};
    int dy[4] = {00, 01, 00, -1};

	// Based on the snake sum from here - https://cs229.stanford.edu/proj2016/report/NieHouAn-AIPlays2048-report.pdf
	double matrixSum(Game2048 game, double scalefactor = 4) {
		double res = 0;
		double scal = 1.0 / (256.0 * 256.0);

		for (int y = 0; y < 4; y++) {
			if (y & 1) {
				for (int x = 0; x < 4; x++) {
					res += scal * game.board[x][y];
					scal *= scalefactor;
				}
			}
			else {
				for (int x = 3; x >= 0; x--) {
					res += scal * game.board[x][y];
					scal *= scalefactor;
				}
			}
		}

		return res;
	}

	double lineSum(Game2048 game, double scalefactor = 4) {
		double res = 0;
		double scl = 1.0 / 256.0;
		for (int i = 0; i < 6; i++) {
			for (int j = 0; j <= i; j++) {
				if (!game.inBounds(j, i - j)) continue;
				res += scl * game.board[j][i - j];
			}
			scl *= scalefactor;
		}
		return res;
	}
    
    // This method assumes our desired placement of tiles concentrates in the bottom right. In the wrapper function this is run on various rotated and reflected boards.
    double getOneDirectionScore(Game2048 game) {
        if (game.gameCleared()) return INF;
        if (game.gameOver()) return -1 * INF;
        int empties = 0;
		int roughness = 0;
        int rowinv = 0;

        // Flat board calculation

        bool ff1 = true;
        bool foundthings = false;
        for (int i = 0; i < 4; i++) {
            int sum = 0;
            for (int j = 0; j < 4; j++) sum += (game.board[i][j] > 0) ? 1 : 0;
            if (sum > 0 && sum < 4) {
                ff1 = false;
                break;
            }
            if (sum == 4) foundthings = true;
            if ((sum == 0) && foundthings) {
                ff1 = false;
                break;
            }
        }

        bool ff2 = true;
        foundthings = false;
        for (int i = 0; i < 4; i++) {
            int sum = 0;
            for (int j = 0; j < 4; j++) sum += (game.board[j][i] > 0) ? 1 : 0;
            if (sum > 0 && sum < 4) {
                ff2 = false;
                break;
            }
            if (sum == 4) foundthings = true;
            if ((sum == 0) && foundthings) {
                ff2 = false;
                break;
            }
        }

        bool foundflat = ff1 || ff2;

        // True flatboard calculation
        bool tf1 = true;
        for (int i = 0; i < 4; i++) {
            if (game.board[i][0] > 0) tf1 = false;
            if (game.board[i][1] == 0) tf1 = false;
            if (game.board[i][2] == 0) tf1 = false;
            if (game.board[i][3] == 0) tf1 = false;
        }

        bool tf2 = true;
        for (int i = 0; i < 4; i++) {
            if (game.board[0][i] > 0) tf1 = false;
            if (game.board[1][i] == 0) tf1 = false;
            if (game.board[2][i] == 0) tf1 = false;
            if (game.board[3][i] == 0) tf1 = false;
        }

        bool foundtrueflat = tf1 || tf2;
        
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (game.board[i][j] == 0) {
                    empties++;
                    continue;
                }
                for (int d = 0; d < 4; d++) {
                    int x = i + dx[d];
                    int y = j + dy[d];
                    if (!game.inBounds(x, y)) continue;
					else roughness += abs(game.board[i][j] - game.board[x][y]);
                }
            }
        }

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 3; j++) {
                if (game.board[i][j] > game.board[i][j + 1]) rowinv++;
            }
        }
        
		double retval = emptytile * empties + rough * roughness + matsum * matrixSum(game) + rinv * rowinv + flatboard * foundflat + trueflat * foundtrueflat;

		return retval;
    }

	// Return the maximum of the four directional scores
	double getScore(Game2048 game) {
		Game2048 game2(game);
		double res = -1 * DBL_MIN;
		for (int i = 0; i < 4; i++) {
			res = std::max(res, getOneDirectionScore(game2));
			game2.rotateBoard();
		}
		game2.reflectBoard();
		for (int i = 0; i < 4; i++) {
			res = std::max(res, getOneDirectionScore(game2));
			game2.rotateBoard();
		}
		return res;
	}
    
    // We view the "minimizing player/opponent" as the random placement of tiles on the board.
    
    int chosenmove = 0;
    int leafcount = 0;
    double abprune(Game2048 game, int remlayers, double alpha, double beta, bool isMaximizing) {
        if (remlayers <= 0) {
            leafcount++;
            return getScore(game);
        }
        // Ironically the easy part
        if (isMaximizing) {
            double res = -1 * DBL_MAX;
            std::vector<int> v;
            for (int i = 0; i < 4; i++) v.push_back(i);
            std::random_shuffle(v.begin(), v.end());
            
            for (auto p : v) {
                Game2048 game2(game);
                game2.move(p);
                if (game2 == game) continue; // Move has to have done something
                
                double value = abprune(game2, remlayers - 1, alpha, beta, false);
                if (value > res){
                    chosenmove = p;
                    res = value;
                }
                if (value == res && rand() % 2 == 0) {
                    chosenmove = p;
                    res = value;
                }
                
                alpha = std::max(res, alpha);
                if (beta <= alpha) break;
            }
			return res;
        }
        else {
            double res = DBL_MAX;
            std::vector<std::pair<int, std::pair<int, int>>> fuckery;
            for (int x = 0; x < 4; x++) {
                for (int y = 0; y < 4; y++) {
                    if (game.board[x][y] != 0) continue;
                    fuckery.push_back({2, {x, y}});
                    fuckery.push_back({4, {x, y}});
                }
            }
            std::random_shuffle(fuckery.begin(), fuckery.end());
            
            for (auto p : fuckery) {
                Game2048 game2(game);
                game2.board[p.second.first][p.second.second] = p.first;
                double value = abprune(game2, remlayers - 1, alpha, beta, true);
                
                if (value < res) res = value;
                if (value == res && rand() % 2 == 0) res = value;
                beta = std::min(beta, res);
                if (beta <= alpha) break;
            }
			return res;
        }
        return -1;
    }
    
    int pick(Game2048 game, bool verbose = false) {
        leafcount = 0;
        chosenmove = -1;
        
        abprune(game, ply , -1 * DBL_MAX, DBL_MAX, true); // Twice the number of player moves
        return chosenmove;
    }
};

bool operator<(const AI2048& t, const AI2048& o) {
    std::vector<double> v1 = {t.emptytile, t.rough, t.matsum, t.rinv};
    std::vector<double> v2 = {o.emptytile, o.rough, o.matsum, o.rinv};
    return v1 < v2;
}

namespace Genetic {

long long test(AI2048 ai, bool verbose = false, int numGames = 3) {
	long long totalscore = 0;
	for (int i = 0; i < numGames; i++) {
    Game2048 game;
    game.placeRandom();
    while (!game.gameOver()) {
        int res = ai.pick(game);
        game.move(res);
        game.placeRandom();
        if (verbose) game.disp();
    }
	totalscore += game.score;
	if (game.gameCleared()) {
        totalscore += ai.INF;
        // std::cout << "CLEARED";
	}
    }
	return totalscore;
}

std::vector<AI2048> tournament(std::vector<AI2048> v, int k, bool verbose = true) {
    std::vector<std::pair<long long, AI2048>> rank;
    for (auto i : v) {
        rank.push_back({-1 * test(i), AI2048(i)});
        if (verbose) std::cout << "X";
    }
    std::cout << "\n";
    
    std::sort(rank.begin(), rank.end());
    std::vector<AI2048> res;
    for (int i = 0; i < v.size() && i < k; i++) res.push_back(rank[i].second);
    return res;
}

double randf() {
    return (double)(rand()) / (double)(RAND_MAX);
}

double randrad() {
     return 2 * (0.5 - randf());
}

AI2048 randomAI(double radius = 4096) {
    return AI2048(radius * randrad(), radius * randrad(), radius * randrad(), radius * randrad(), radius * randrad(), radius * randrad());
}

AI2048 mutate(AI2048 ai, double radius = 16) {
    AI2048 res(ai);
    int blah = rand() % 32;
    if (blah == 0) res.emptytile = radius * randrad();
	if (blah == 1) res.rough = radius * randrad();
	if (blah == 2) res.matsum = radius * randrad();
    if (blah == 3) res.rinv = radius * randrad();
    if (blah == 4) res.flatboard = radius * randrad();
    if (blah == 5) res.trueflat = radius * randrad();
    return res;
}

AI2048 cross(AI2048 a1, AI2048 a2) {
    AI2048 res(a1);
    if (rand() % 2 == 0) res.emptytile = a2.emptytile;
	if (rand() % 2 == 0) res.rough = a2.rough;
	if (rand() % 2 == 0) res.matsum = a2.matsum;
    if (rand() % 2 == 0) res.rinv = a2.rinv;
    if (rand() % 2 == 0) res.flatboard = a2.flatboard;
    if (rand() % 2 == 0) res.trueflat = a2.trueflat;
    return res;
}

}

#endif

/*

2ply 16x64 3/r

EMPTY 1.772027 ROUGH -1136.909696 MATSUM 14.485305
EMPTY 7.224121 ROUGH -3988.781414 MATSUM 15.735084 RINV -11.251073
EMPTY 11.970711 ROUGH -2496.006038 MATSUM 11.952956 RINV 8.097248 FLAT -9.355223
EMPTY 2739.060733 ROUGH -2793.153255 MATSUM 10.774997 RINV 5.861263 FLAT -14.881667 TRUEFLAT -5.521547

*/
