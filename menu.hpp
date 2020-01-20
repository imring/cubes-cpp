namespace Menu {
    enum MainMenu {
        NewHost = 0,
        Connect,
        Exit
    };

    enum HostMenu {
        CreateHost = 1,
        Back
    };

    bool DrawTitle(const std::vector<sf::Event> &events);
    void DrawMainMenu(const std::vector<sf::Event> &events);
    int DrawOptions(const std::vector<sf::Event> &events, std::vector<std::string> options, unsigned int &currentOption);
    void DrawCreateHost(const std::vector<sf::Event> &events, Player *localPlayer);
};