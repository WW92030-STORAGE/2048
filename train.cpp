#include "2048.h"
#include "genetic.h"

int main()
{
    srand(time(0));
    
    std::vector<AI2048> v;
    for (int i = 0; i < 16; i++) v.push_back(Genetic::randomAI());
    
    for (int i = 0; i < 64; i++) {
        std::cout << "GEN " << (i + 1) << "\n"; 
        std::vector<AI2048> top = Genetic::tournament(v, v.size() / 2);
        for (auto i : top) std::cout << i.toString() << "\n";

		v.clear();
		for (int i = 0; i < 4; i++) {
			std::random_shuffle(top.begin(), top.end());
			for (int j = 0; j < top.size(); j += 2) v.push_back(Genetic::mutate(Genetic::cross(top[j], top[j + 1])));
		}
    }

	// Reduction

    std::vector<AI2048> res;

    while (true) {
        res = Genetic::tournament(v, v.size() / 2);
        for (auto i : res) std::cout << i.toString() << "\n";

        if (res.size() <= 1) break;

		v.clear();
		for (int i = 0; i < 2; i++) {
			std::random_shuffle(res.begin(), res.end());
			for (int j = 0; j < res.size(); j += 2) v.push_back(Genetic::mutate(Genetic::cross(res[j], res[j + 1])));
		}
    }

    for (int i = 0; i < 64; i++) std::cout << Genetic::test(res[0], false, 1) << " ";
    std::cout << "\n" << res[0].toString() << "\n";

    return 0;
}
