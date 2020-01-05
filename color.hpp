const std::vector<sf::Color> stdColors = {
    {255, 255, 0},
    {51, 51, 255},
    {255, 0, 0},
    {0, 255, 0},
    {255, 0, 127},
    {255, 255, 255},
    {255, 128, 0},
    {0, 255, 255},
    {144, 238, 144}
};

namespace Color {
    void CreateColor(int id);
    sf::Color GetColor(int id);
    sf::Color GetNextColor(int id);
    void SetColor(int id, sf::Color color);
    void SetNextColor(int id, sf::Color color);
    void UpdateColor(int id);
};