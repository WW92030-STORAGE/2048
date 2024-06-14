#include <iostream>

#include "genetic.h"
#include "2048.h"

using namespace std;


int main() {
	srand(time(0));
	AI2048 ai(2739.060733, -2793.153255, 10.774997, 5.861263, -14.881667, -5.521547);

	for (int i = 0; i < 128; i++) std::cout << Genetic::test(ai, false, 1) << " ";
    std::cout << "\n" << ai.toString() << "\n";
	
	ai.ply = 4;
	for (int i = 0; i < 128; i++) std::cout << Genetic::test(ai, false, 1) << " ";
    std::cout << "\n" << ai.toString() << "\n";
	return 0;
}
