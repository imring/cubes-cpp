#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/String.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/Network.hpp>

#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <map>

#include "player.hpp"
#include "menu.hpp"
#include "color.hpp"

namespace rd {
    namespace {
        static std::random_device rd;
        static std::mt19937 generator(rd());
    };

    template<typename T>
    auto floating_point(T min, T max) {
        static_assert(std::is_floating_point<T>::value, "");
        std::uniform_real_distribution<T> distribution(min, max);
        return distribution(generator);
    }

    template<typename T>
    auto integral(T min, T max) {
        static_assert(std::is_integral<T>::value, "");
        std::uniform_int_distribution<T> distribution(min, max);
        return distribution(generator);
    }
};