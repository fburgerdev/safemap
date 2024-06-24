#include "secure-map.hpp"
#include <chrono>
#include <math.h>

using namespace Memory;
void dosth(int key, Array<double, 50>& array) {
    // cout << key << ' ';
    for (int j = 0; j < 50; ++j) {
        for (int k = 0; k < 100; ++k) {
            array[j] += std::sqrt((double)j + k );
        }
    }
}
int main() {
    int count = 10'000;
    // {
    //     Map<int, Array<double, 50>> map;
    //     for (int i = 0; i < count; ++i) {
    //         map.try_emplace(i);
    //     }
    //     auto start = std::chrono::high_resolution_clock::now();
    //     for (auto& [key, value] : map) {
    //         dosth(key, value);
    //     }
    //     auto finish = std::chrono::high_resolution_clock::now();
    //     auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish-start);
    //     std::cout << ms.count() << "ms\n";
    // }
    {
        Map<int, Array<double, 50>> map;
        for (int i = 0; i < count; ++i) {
            map.try_emplace(i);
        }
        auto start = std::chrono::high_resolution_clock::now();
        std::for_each(map.begin(), map.end(), ([](auto& pair){
            dosth(pair.first, pair.second);
        }));
        auto finish = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish-start);
        std::cout << ms.count() << "ms\n";
    }
    {
        SecureMap<int, Array<double, 50>> map;
        for (int i = 0; i < count; ++i) {
            map.emplace(i);
        }
        auto start = std::chrono::high_resolution_clock::now();
        // auto mid = std::next(map.begin(), count/2);
        map.foreach(dosth);
        auto finish = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish-start);
        std::cout << ms.count() << "ms\n";
    }
    return EXIT_SUCCESS;
}