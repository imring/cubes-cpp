#include "main.hpp"

extern int size_window[2];
extern float screen_diff[2];
extern sf::Font mainFont;
extern sf::RenderWindow window;
extern bool IsHostMenuOpened, IsConnectMenuOpened;

static std::string symbolsAllowedForName = "1234567890QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm *-+`.";

#ifdef _MSC_VER
#pragma warning(disable : 4996)
#elif __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

bool Menu::DrawTitle(const std::vector<sf::Event> &events) {
    static bool run = true;
    static unsigned char alpha = 255;
    static int frameCount = 0;

    if(!run) return false;

    for(auto &event : events)
        if(event.type == sf::Event::KeyPressed || event.type == sf::Event::MouseButtonPressed)
            run = false;

    static sf::Text title;
    title.setString("CUBES");
    title.setFont(mainFont);
    title.setCharacterSize(screen_diff[0] * 128.f);
    auto textRect = title.getLocalBounds();
    title.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    title.setPosition(sf::Vector2f(size_window[0] / 2, size_window[1] / 2));

    sf::Color titleColor = Color::GetColor(0);
    if(frameCount >= 200) alpha -= 5;
    titleColor.a = alpha;
    title.setColor(titleColor);
    Color::UpdateColor(0);

    window.draw(title);

    frameCount++;
    if(alpha <= 0) run = false;

    return true;
}

void Menu::DrawMainMenu(const std::vector<sf::Event> &events) {
    static std::vector<std::string> options = {"CREATE HOST", "CONNECT", "EXIT"};
    static unsigned int current_menu = 0;
    
    switch(DrawOptions(events, options, current_menu)) {
    case MainMenu::NewHost:
        IsHostMenuOpened = true;
        break;
    case MainMenu::Connect:
        IsConnectMenuOpened = true;
        break;
    case MainMenu::Exit:
        window.close();
        break;
    }
}

int Menu::DrawOptions(const std::vector<sf::Event> &events, std::vector<std::string> options, unsigned int &currentOption) {
    sf::Text option;
    option.setFont(mainFont);
    option.setCharacterSize(screen_diff[1] * 35.f);
    sf::Color menuColor = Color::GetColor(1);
    Color::UpdateColor(1);

    for(size_t i = 0; i < options.size(); ++i) {
        option.setPosition(sf::Vector2f(50, size_window[1] - screen_diff[1] * ( 20 + ( options.size() - i ) * 50 ) ));
        option.setString(sf::String(options[i]));
        option.setColor(i == currentOption ? menuColor : sf::Color(120, 120, 120));
        window.draw(option);
    }

    for(auto &event : events) {
        if(event.type == sf::Event::KeyPressed) {
            switch(event.key.code) {
            case sf::Keyboard::Up:
                currentOption--;
                currentOption = currentOption < 0 ? options.size() - 1 : currentOption;
                break;
            case sf::Keyboard::Down:
                currentOption = ( currentOption + 1 ) % options.size();
                break;
            case sf::Keyboard::Enter:
                return currentOption;
            
            default: break;
            }
        }
    }
    return -1;
}

void Menu::DrawCreateHost(const std::vector<sf::Event> &events, Player *localPlayer) {
    static std::vector<std::string> options = {"NAME: ", "CREATE HOST", "BACK"};
    static unsigned int current_menu = 0;

    auto tempoptions = options;
    tempoptions[0] += localPlayer->name;

    for(auto &event : events) {
        switch(event.type) {
        case sf::Event::KeyPressed:
            if(event.key.code == sf::Keyboard::Escape)
                IsHostMenuOpened = false;
            else if(event.key.code == sf::Keyboard::BackSpace && current_menu == 0 && !localPlayer->name.empty())
                localPlayer->name.erase(localPlayer->name.size() - 1);
            break;
        case sf::Event::TextEntered:
            if(current_menu == 0 && symbolsAllowedForName.find(event.text.unicode) != std::string::npos && event.text.unicode < 0x80 && localPlayer->name.size() <= 12)
                localPlayer->name += event.text.unicode;
            break;

        default: break;
        }
    }

    switch(DrawOptions(events, tempoptions, current_menu)) {
    case HostMenu::Back:
        IsHostMenuOpened = false;
    }
}