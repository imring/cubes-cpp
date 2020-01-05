#include "main.hpp"

std::map<int, sf::Color[2]> colors;

void Color::CreateColor(int id) {
    SetColor(id, stdColors[rd::integral(0UL, stdColors.size() - 1)]);
    SetNextColor(id, stdColors[rd::integral(0UL, stdColors.size() - 1)]);
}

sf::Color Color::GetColor(int id) {
    if(colors.find(id) == colors.end()) CreateColor(id);
    return colors[id][0];
}

sf::Color Color::GetNextColor(int id) {
    if(colors.find(id) == colors.end()) CreateColor(id);
    return colors[id][1];
}

void Color::SetColor(int id, sf::Color color) {
    colors[id][0] = color;
}

void Color::SetNextColor(int id, sf::Color color) {
    colors[id][1] = color;
}

void Color::UpdateColor(int id) {
    sf::Color color = GetColor(id), next_color = GetNextColor(id);
    if( abs(color.r - next_color.r) < 5 && abs(color.g - next_color.g) < 5 && abs(color.b - next_color.b) < 5 )
        SetNextColor(id, stdColors[rd::integral(0UL, stdColors.size() - 1)]);
    
    color.r = next_color.r < color.r ? std::max(color.r - 4, 0) : std::min(color.r + 4, 255);
    color.g = next_color.g < color.g ? std::max(color.g - 4, 0) : std::min(color.g + 4, 255);
    color.b = next_color.b < color.b ? std::max(color.b - 4, 0) : std::min(color.b + 4, 255);
    SetColor(id, color);
}