#include <bits/stdc++.h>

#include "genetic.h"
#include "2048.h"

using namespace std;


int main() {
	srand(time(0));
	AI2048 ai(1.772027, -1136.909696, 14.485305);

	for (int i = 0; i < 128; i++) std::cout << Genetic::test(ai, false, 1) << " ";
    std::cout << "\n" << ai.toString() << "\n";
	
	ai.ply = 4;
	for (int i = 0; i < 128; i++) std::cout << Genetic::test(ai, false, 1) << " ";
    std::cout << "\n" << ai.toString() << "\n";
	return 0;
}

/*

4ply 32x64

2ply 16x64

SUM 6.093448 MAX 12.665914 PAIRS 7.813227 EMPTIES -8.727317 INVS -0.734886 EDGE -6.387402 REV -15.349590 ROUGH -14.033143 MATSUM 0.687033 LINESUM -15.094699

2ply 32x16

SUM 10.459792 MAX -10.709799 PAIRS -15.530259 EMPTIES 8.748802 INVS 11.507675 EDGE -9.208777 REV 12.066286 ROUGH -13.845637 MATSUM 11.551622 LINESUM 11.344584

*/