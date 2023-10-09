//
// Created by eduardo on 8/10/23.
//

#ifndef BRAKEZA3D_SHAREDLUACONTEXT_H
#define BRAKEZA3D_SHAREDLUACONTEXT_H


#include <vector>
#include <string>
#include <algorithm>

class SharedLUAContext {
    std::vector<std::pair<std::string, std::string>> pairs;
public:
    int prueba = 1;

    [[nodiscard]] std::string get(const std::string& firstElement) const {
        for (const auto& pair : pairs) {
            if (pair.first == firstElement) {
                return pair.second;
            }
        }
        return "";
    }

    void remove(const std::string& firstElement) {
        pairs.erase(std::remove_if(pairs.begin(), pairs.end(),
        [&](const std::pair<std::string, std::string>& pair) {
           return pair.first == firstElement;
        }), pairs.end());
    }

    bool set(const std::string& firstElement, const std::string& newSecondElement) {
        for (auto& pair : pairs) {
            if (pair.first == firstElement) {
                pair.second = newSecondElement;
                return true;
            }
        }

        pairs.emplace_back(firstElement, newSecondElement);
        return true;
    }

    void clearPairs() {
        pairs.clear();
    }

    int value = 1;

    void addPair(const std::string& key, const std::string& value) {
        pairs.emplace_back(key, value);
    }
};


#endif //BRAKEZA3D_SHAREDLUACONTEXT_H
