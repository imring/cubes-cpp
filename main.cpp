#include "main.hpp"

sf::RenderWindow window;
sf::Font mainFont;

int size_window[2];
float screen_diff[2];

bool IsHostMenuOpened = false, IsConnectMenuOpened = false;

int main() {
    sf::UdpSocket socket;
    socket.bind(sf::Socket::AnyPort);
    socket.setBlocking(false);

    auto desktop = sf::VideoMode::getDesktopMode();
    window.create(desktop, "CUBES", sf::Style::Fullscreen);
    size_window[0] = desktop.width;
    size_window[1] = desktop.height;

    screen_diff[0] = float(size_window[0]) / 1366;
    screen_diff[1] = float(size_window[1]) / 768;

    window.setKeyRepeatEnabled(true);
    window.setFramerateLimit(30);

    if(!mainFont.loadFromFile("fonts/FONT.ttf")) return 0;

    Player *localPlayer = new Player();

    while(window.isOpen()) {

        std::vector<sf::Event> events;
        sf::Event tempevent;
        while(window.pollEvent(tempevent)) {
            if(tempevent.type == sf::Event::Closed) window.close();
            else events.push_back(tempevent);
        }

        window.setMouseCursorVisible(false);
        window.clear(sf::Color(20, 20, 20));

        if(IsHostMenuOpened) Menu::DrawCreateHost(events, localPlayer);
        else if(!Menu::DrawTitle(events)) Menu::DrawMainMenu(events);

        window.display();
    }

    return 0;
}