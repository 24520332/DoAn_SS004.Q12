#include "Bag.h"
#include <algorithm>
#include <cstdlib>

int Bag::next() {
    if (bag.empty()) {
        bag = {0,1,2,3,4,5,6};

        for (int i = bag.size() - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            std::swap(bag[i], bag[j]);
        }
    }

    int t = bag.back();
    bag.pop_back();
    return t;
}
