#include "Bag.h"
#include <algorithm>
#include <cstdlib>

int Bag::next() {
    if (bag.empty()) {
        bag = {0,1,2,3,4,5,6};

        for (size_t i = bag.size() - 1; i > 0; i--) {
            size_t j = static_cast<size_t>(rand()) % (i + 1);
            std::swap(bag[i], bag[j]);
        }
    }

    int t = bag.back();
    bag.pop_back();
    return t;
}
