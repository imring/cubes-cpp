#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SFML/Network.hpp>
#include <stdio.h>
#include <stdlib.h>
#include "scripts/cube.h"
#include "scripts/bullet.h"
#include <cmath>
#include "scripts/get&send.h"
#include <string>
#include "scripts/menu.h"
#include "scripts/font.h"
#include <algorithm>
#include "scripts/npc.h"
#include "scripts/strings.h"
#include "scripts/sizeProperties.h"
#include "scripts/messages.h"


;using namespace std;
using sf::UdpSocket;
using sf::Socket;
using sf::IpAddress;

SDL_Window *window = NULL;
SDL_Renderer *ren = NULL;

TTF_Font *nameFont = NULL;
Font *menuOptionFont = NULL;
Font *messageFont = NULL;
Font *titleFont = NULL;

int SCREEN_SIZE[2];
float SCREEN_DIFF[2] = {0, 0};

int random(int min, int max) {
    return min + rand()%max;
}
int random(int max) {
    return rand()%max;
}

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Can't init SDL2");
        return false;
    }

    if (TTF_Init() == -1) {
        printf("Can't init TTF\n");
        return false;
    }

    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    int Width = DM.w;
    int Height = DM.h;

    window = SDL_CreateWindow("CUBES", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Width, Height, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);
    // window = SDL_CreateWindow("CUBES", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);
    if (window == NULL) {
        printf("Can't create window");
        return false;
    }

    ren = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (ren == NULL) {
        printf("Can't create renderer");
        return false;
    }
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
    SDL_GetRendererOutputSize(ren, &SCREEN_SIZE[0], &SCREEN_SIZE[1]);
    SCREEN_DIFF[0] = (float)SCREEN_SIZE[0] / 1366;
    SCREEN_DIFF[1] = (float)SCREEN_SIZE[1] / 768;

    nameFont = TTF_OpenFont("fonts/FONT.otf", round((float)SCREEN_SIZE[0] / 1366 * 12));
    menuOptionFont = new Font("fonts/FONT.ttf", round((float)SCREEN_SIZE[0] / 1366 * 35));
    messageFont = new Font("fonts/FONT.ttf", round((float)SCREEN_SIZE[0] / 1366 * 24));
    titleFont = new Font("fonts/FONT.ttf", round((float)SCREEN_SIZE[0] / 1366 * 128));

    return true;
}

void quit() {
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(window);

    nameFont = NULL;
    menuOptionFont = NULL;
    messageFont = NULL;
    titleFont = NULL;

    ren = NULL;
    window = NULL;

    SDL_Quit();
}

int HOST(string playerName, UdpSocket *socket, bool *playAgain) {
    IpAddress ADDRESS = IpAddress::getLocalAddress();
    unsigned short PORT = socket->getLocalPort();
    
    SDL_Event e;

    bool run = true;
    int xbutton, ybutton;
    bool rpr = false, lpr = false, upr = false, dpr = false, lmpr = false;

    int fTime = 1000 / 60, fDelta = 0;
    long int fStart, fEnd;
    int mPos[2] = {0, 0};

    vector<Bullet> bullets;
    vector<Player> players;
    vector<NPC> npcs;
    vector<int> ports;

    players.push_back(Player(random(1366 - playerSize), random(768 - playerSize), PORT, ADDRESS, playerName));

    char pData[4096];
    size_t received;
    IpAddress pAddr;
    unsigned short pPort;
    int currentOption = 0;

    Messages messages;

    /*
    CONNECTING PLAYERS
    */

    while (run) {
        fStart = SDL_GetTicks();
        ports.clear();
        ports.push_back(0);
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                *playAgain = false;
                return -1;
            }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_RETURN) {
                    if (checkReadyPlayers(players) && players.size() > 1){
                        run = false;
                    }
                }
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    *playAgain = false;
                    return 0;
                }
                if (e.key.keysym.sym == SDLK_r) {
                    players[0].togleReady();
                }

                if (e.key.keysym.sym == SDLK_n) {
                    if (players.size() < 10) {
                        IpAddress tempAddr = "0.0.0.0";
                        players.push_back(Player(random(1366 - playerSize), random(768 - playerSize), 0, tempAddr, aviableNamesForBots[rand()%aviableNamesForBots.size()]));
                        npcs.push_back(players.size() - 1);
                        players[players.size() - 1].togleReady();
                    }
                    
                }
            }


         }
    

        int additionalIter = 0;
        int pNum = 0;
        while (pNum < players.size() + 1 + additionalIter) {
            if (socket->receive(pData, 4096, received, pAddr, pPort) != Socket::NotReady) {
                if (pData[0] == 'n') {
                    if (getPlayerIndex(players, pPort) == -1) {
                        string name;
                        for (int i = 1; i < strlen(pData); i++) {
                            name = name + pData[i];
                        }
                        if (players.size() < 10) {
                            players.push_back(Player(random(1366 - playerSize), random(768 - playerSize), pPort, pAddr, name));
                            socket->send("HELLO", 4096, pAddr, pPort);
                            additionalIter++;
                        }
                        else {
                            socket->send("GAME STARTED", 4096, pAddr, pPort);
                            additionalIter++;
                        }
                    }
                    
                    
                }
                if (strcmp(pData, "GOOD BYE") == 0) {
                    int playerNum = getPlayerIndex(players, pPort);
                    if (playerNum < players.size() && playerNum >= 0) {
                        for (auto &bot: npcs) {if (bot.playerIndex > playerNum) {bot.playerIndex--;}}
                        players.erase(players.begin() + playerNum);
                    }
                }
                if (strcmp(pData, "I AM HERE") == 0) {
                    ports.push_back((int)pPort);
                    int playerNum = getPlayerIndex(players, pPort);
                    if (playerNum < players.size() && playerNum >= 0) {
                        players[playerNum].triedToConn = 0;
                    }
                    
                }
                if (strcmp(pData, "R PRESSED") == 0) {
                    ports.push_back((int)pPort);
                    int playerNum = getPlayerIndex(players, pPort);
                    if (playerNum < players.size() && playerNum >= 0) {
                        players[playerNum].triedToConn = 0;
                        players[playerNum].togleReady();
                    }
                }
            }
            pNum ++;
        }

        for (int p = 1; p < players.size(); p++) {
            int pN = findPort(ports, players[p].port);
            if (pN == -1) {
                players[p].triedToConn++;
                if (players[p].triedToConn >= 300) {
                    if (p < players.size() && p >= 0) {
                        for (auto &bot: npcs) {if (bot.playerIndex > p) {bot.playerIndex--;}}
                        players.erase(players.begin() + p);
                    }
                }
            }
            else {
                players[p].triedToConn = 0;
            }
        }

        json sjData = {
            {"players", {}}
        };
        string sData;
        for (auto &player:players) {
            json jData = {
                {"color1", player.color[0]},
                {"color2", player.color[1]},
                {"color3", player.color[2]},
                {"name", player.name},
                {"ready", player.rPressed}
            };
            sjData["players"].push_back(jData);
        }
        sData = sjData.dump();
        for (int i = 1; i < players.size(); i++) {
            socket->send(sData.c_str(), 4096, players[i].address, players[i].port);
        }

        SDL_SetRenderDrawColor(ren, 20, 20, 20, 255);
        SDL_RenderClear(ren);

        menu::draw_player_in_menu(ren, players, nameFont, SCREEN_SIZE, SCREEN_DIFF);

        menu::draw_ip_and_port(ren, ADDRESS.toString(), to_string(PORT), menuOptionFont, SCREEN_SIZE, SCREEN_DIFF);

        messageFont->render(ren, "Press N to add a bot", 50, SCREEN_SIZE[1] - SCREEN_DIFF[1] * 100, NULL);
        messageFont->render(ren, "Press R when you're ready", 50, SCREEN_SIZE[1] - SCREEN_DIFF[1] * 70, NULL);
        messageFont->render(ren, "Press ENTER to start", 50, SCREEN_SIZE[1] - SCREEN_DIFF[1] * 40, NULL);

        SDL_RenderPresent(ren);
        fEnd = SDL_GetTicks();
        fDelta = (fEnd - fStart);
        if (fDelta < fTime) {
            SDL_Delay(fTime - fDelta);
        }
    }

    for (int pl = 1; pl < players.size(); pl++) {
        socket->send("START GAME", 4096, players[pl].address, players[pl].port);
    }
    run = true;

    for (auto &p : players) {
        p.triedToConn = 0;
    }

    bool canPressEnter = false;

    string winnerName;
    bool winnerChecked = false;

    string lSurvivourString = allDiedStrings[rand()%allDiedStrings.size()];
    string oneSurvivedString = oneSurvived[rand()%oneSurvived.size()];
    int xButton, yButton, mPos1, mPos2;
    bool pausePressed = false;
    int pauseCurrentOption = 0;
    SDL_ShowCursor(SDL_ENABLE);

    /*
    GAME STARTED
    */

    while (run) {
        fStart = SDL_GetTicks();
        ports.clear();
        ports.push_back(0);
        for (auto &p : players) {
        }

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                *playAgain = false;
                return -1;
            }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_w && !pausePressed) {
                    upr = true;
                }
                if (e.key.keysym.sym == SDLK_s && !pausePressed) {
                    dpr = true;
                }
                if (e.key.keysym.sym == SDLK_d && !pausePressed) {
                    rpr = true;
                } 
                if (e.key.keysym.sym == SDLK_a && !pausePressed) {
                    lpr = true;
                }
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    if (pausePressed) {
                        pausePressed = false;
                    }
                    else {
                        pausePressed = true;
                        lmpr = false;
                        upr = false;
                        dpr = false;
                        rpr = false;
                        lpr = false;
                    }
                }
                if (e.key.keysym.sym == SDLK_RETURN && canPressEnter && !pausePressed) {
                    *playAgain = true;
                    for (int p = 1; p < players.size(); p++) {
                        socket->send("GAME FINISHED", 4096, players[p].address, players[p].port);
                    }
                    return 0;
                }

                //PAUSE CONTROL
                if (e.key.keysym.sym == SDLK_DOWN  && pausePressed) {
                    pauseCurrentOption++;
                    if (pauseCurrentOption == 2) {
                        pauseCurrentOption = 0;
                    }
                }
                if (e.key.keysym.sym == SDLK_UP && pausePressed) {
                    pauseCurrentOption--;
                    if (pauseCurrentOption == -1) {
                        pauseCurrentOption = 1;
                    }
                }
                if (e.key.keysym.sym == SDLK_RETURN && pausePressed) {
                    if (pauseCurrentOption == 0) {
                        pausePressed = false;
                    }
                    else {
                        *playAgain = false;
                        return 0;
                    }
                }
            }
            if (e.type == SDL_KEYUP && !pausePressed) {
                if (e.key.keysym.sym == SDLK_w) {
                    upr = false;
                }
                if (e.key.keysym.sym == SDLK_s) {
                    dpr = false;
                }
                if (e.key.keysym.sym == SDLK_d) {
                    rpr = false;
                } 
                if (e.key.keysym.sym == SDLK_a) {
                    lpr = false;
                }
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT && !pausePressed) {
                    lmpr = true;
                }
            }
            if (e.type == SDL_MOUSEBUTTONUP) {
                if (e.button.button == SDL_BUTTON_LEFT && !pausePressed) {
                    lmpr = false;
                }
            }

            if (lmpr) {
                SDL_GetMouseState(&mPos[0], &mPos[1]);
            }

            if ((lpr && rpr) || (!lpr & !rpr)) {
                xbutton = 0;
            }
            else if (lpr) {
                xbutton = 1;
            }
            else {
                xbutton = 2;
            }

            if ((upr && dpr) || (!upr && !dpr)) {
                ybutton = 0;
            }
            else if (upr) {
                ybutton = 1;
            }
            else {
                ybutton = 2;
            }
        }

        int pIndex = 1;
        char rData[4096];
        size_t received;
        IpAddress rAddr;
        unsigned short rPort;
        if (!players[0].is_dead()){
            players[0].update(xbutton, ybutton, &bullets, players, &messages);
            if (lmpr) {
                players[0].shoot(&bullets, mPos, SCREEN_DIFF, true);
            }
        }
        int additionalIter = 0;
        while (pIndex < players.size()+1+additionalIter) {
            if (socket->receive(rData, 4096, received, rAddr, rPort) == Socket::NotReady) {}
            else {
                if (rData[0] == 'n') {
                    if (getPlayerIndex(players, rPort) == -1) {
                        string name;
                        for (int i = 1; i < strlen(rData); i++) {
                            name = name + rData[i];
                        }
                        socket->send("GAME STARTED", 4096, rAddr, rPort);
                        additionalIter++;
                    } 
                }
                else if (strcmp(rData, "I AM HERE") == 0 || strcmp(rData, "R PRESSED") == 0) {additionalIter++;}
                else if (strcmp(rData, "GOOD BYE") == 0) {
                    int playerNum = getPlayerIndex(players, rPort);
                    if (playerNum < players.size() && playerNum >= 0) {
                        for (auto &bot: npcs) {if (bot.playerIndex > playerNum) {bot.playerIndex--;}}
                        players.erase(players.begin()+playerNum);
                    }
                }
                else if (strcmp(rData, "ENTER PRESSED") == 0) {
                    int playerNum = getPlayerIndex(players, rPort);
                    if (playerNum != -1) {
                        players[playerNum].enterPressed = true;
                    }
                }
                else {
                    json jData = json::parse(rData);
                    int playerNum = getPlayerIndex(players, rPort);
                    if (!players[playerNum].is_dead()) {
                        players[playerNum].update(jData["xbutton"], jData["ybutton"], &bullets, players, &messages);
                        if (jData["lmpr"]) {
                            int tempMPos[2] = {jData["mp1"], jData["mp2"]};
                            float scrDiff[2] = {jData["scrDiff1"], jData["scrDiff2"]};
                            players[playerNum].shoot(&bullets, tempMPos, scrDiff, true);
                        }
                    }
                    ports.push_back(rPort);
                }
            }
            pIndex++;
        }

        for (auto npc: npcs) {
            if (!players[npc.playerIndex].dead) {
                npc.update(players, bullets, &xButton, &yButton, &mPos1, &mPos2);
                players[npc.playerIndex].update(xButton, yButton, &bullets, players, &messages);
                int tMPos[2] = {mPos1, mPos2};
                players[npc.playerIndex].shoot(&bullets, tMPos, SCREEN_DIFF, false);
            }
        }

        for (int p = 1; p < players.size(); p++) {
            int pN = findPort(ports, players[p].port);
            if (pN == -1 && players[p].enterPressed == false) {
                players[p].triedToConn++;
                players[p].update(0, 0, &bullets, players, &messages);
                if (players[p].triedToConn >= 300) {
                    if (p < players.size() && p >= 0) {
                        for (auto &bot: npcs) {if (bot.playerIndex > p) {bot.playerIndex--;}}
                        players.erase(players.begin() + p);
                    }
                    
                }
            }
            else {
                players[p].triedToConn = 0;
            }
        }

        int bIndex = 0;
        while (bIndex < bullets.size()) {
            if (!bullets[bIndex].update()){
                bullets.erase(bullets.begin()+bIndex);
                bIndex--;
            };
            bIndex++;
        }

        string sData;
        json sjData = {
            {"players", {}},
            {"bullets", {}},
            {"messages", messages.messages}
        };
        for (auto &player : players) {
            json pData = {
                {"x", player.rect.x},
                {"y", player.rect.y},
                {"w", player.rect.w},
                {"h", player.rect.h},
                {"color1", player.color[0]},
                {"color2", player.color[1]},
                {"color3", player.color[2]},
                {"name", player.name},
                {"dead", player.dead},
                {"port", player.port},
                {"lifes", player.lifes}
            };
            sjData["players"].push_back(pData);
        }
        for (auto &bullet : bullets) {
            json bData = {
                {"x", bullet.rect.x},
                {"y", bullet.rect.y},
                {"w", bullet.rect.w},
                {"color1", bullet.color[0]},
                {"color2", bullet.color[1]},
                {"color3", bullet.color[2]}
            };
            sjData["bullets"].push_back(bData);
        }
        if (winnerChecked) {
            sjData["winnerChecked"] = true;
            sjData["winnerName"] = winnerName;
        }
        else {
            sjData["winnerChecked"] = false;
        }
        if (canPressEnter) {
            sjData["canPressEnter"] = true;
        }
        else {
            sjData["canPressEnter"] = false;
        }
        sData = sjData.dump();
        for (int player = 1; player < players.size(); player++) {
            if (socket->send(sData.c_str(), 4096, players[player].address, players[player].port) != Socket::Done && players[player].port != 0) {
                cout << "Can't send data to: " << players[player].address << endl;
            }
        }

        SDL_SetRenderDrawColor(ren, 20, 20, 20, 255);
        SDL_RenderClear(ren);

        for (auto &bullet : bullets) {
            bullet.draw(ren, SCREEN_DIFF);
        }
        for (auto &player : players) {
            player.draw(ren, nameFont, SCREEN_DIFF);
        }

        if (checkWinner(players) && !winnerChecked) {
            canPressEnter = true;
            winnerName = getWinner(players);
            int winnerIndex = getWinnerIndex(players);
            players[winnerIndex].winner = true;
            winnerChecked = true;
            
        }
        if (getBotsLeft(players) && !canPressEnter) {
            canPressEnter = true;
        }

        if (players.size() == 0 && winnerChecked == false) {
            winnerChecked = true;
            winnerName = "Poltergeist";
        }
        if (winnerChecked) {
            if (getSurvived(players) >= 1) {
                messageFont->render(ren, winnerName + oneSurvivedString, 20, SCREEN_SIZE[1] - SCREEN_DIFF[1]*70, NULL);
            }
            if (getSurvived(players) == 0) {
                messageFont->render(ren, winnerName + lSurvivourString, 20, SCREEN_SIZE[1] - SCREEN_DIFF[1]*70, NULL);
            }
        }

        if (canPressEnter) {
            messageFont->render(ren, "Press ENTER to play again", 20, SCREEN_SIZE[1] - SCREEN_DIFF[1]*40, NULL);
        }
        messages.draw(ren, messageFont, SCREEN_SIZE, SCREEN_DIFF);

        menuOptionFont->render(ren, "LIFES: " + to_string(players[0].lifes), SCREEN_SIZE[0] - 200 * SCREEN_DIFF[0], SCREEN_SIZE[1] - SCREEN_DIFF[1] * 60, NULL, true, SCREEN_DIFF[0], SCREEN_DIFF[1]);
        if (pausePressed) {
            menu::drawPause(ren, pauseCurrentOption, menuOptionFont, SCREEN_SIZE, SCREEN_DIFF);
        }

        SDL_RenderPresent(ren);

        fEnd = SDL_GetTicks();
        fDelta = (fEnd - fStart);
        if (fDelta < fTime) {
            SDL_Delay(fTime - fDelta);
        }
    }

    return 0;
}

/*
CLIENT
*/

int CLIENT(string addr, string port, string name, UdpSocket *socket, string *playAgainAddress, string *playAgainPort, bool *playAgain) {
    IpAddress ADDRESS = addr;
    unsigned int PORT = stoi(port);
    name = "n" + name;
    socket->send(name.c_str(), 4096, ADDRESS, PORT);
    char rData[4096];
    size_t received;
    IpAddress rAddr;
    unsigned short rPort;

    int triedToConn = 0;
    SDL_Event e;
    int fTime = 1000 / 60, fDelta = 0;
    long int fStart, fEnd;
    bool run = true;

    unsigned short selfPort = socket->getLocalPort();

    /*
    CONNECTIN TO SERVER
    */

    while (run) {
        fStart = SDL_GetTicks();
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                *playAgain = false;
                *playAgainAddress = "^";
                *playAgainPort = "^";
                return -1;
            }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    *playAgain = false;
                    *playAgainAddress = "^";
                    *playAgainPort = "^";
                    return 0;
                }
            }
        }

        if (socket->receive(rData, 4096, received, rAddr, rPort) != Socket::NotReady) {
            if (strcmp(rData, "GAME STARTED") == 0) {
                *playAgain = false;
                *playAgainAddress = "^";
                *playAgainPort = "^";
                return 0;
            }
            if (strcmp(rData, "HELLO") == 0) {
                run = false;
            }
        }
        else {
            triedToConn++;
            socket->send(name.c_str(), 4096, ADDRESS, PORT);
        }

        SDL_SetRenderDrawColor(ren, 20, 20, 20, 255);
        SDL_RenderClear(ren);
        menuOptionFont->render(ren, "Connecting to server...", 50, SCREEN_SIZE[1] - 60 * SCREEN_DIFF[1], NULL);

        SDL_RenderPresent(ren);

        fEnd = SDL_GetTicks();
        fDelta = (fEnd - fStart);
        if (fDelta < fTime) {
            SDL_Delay(fTime - fDelta);
        }

    }

    int xbutton, ybutton;
    bool rpr = false, lpr = false, upr = false, dpr = false, lmpr = false;
    int mPos[2] = {0, 0};
    run = true;

    vector<Player> players;
    int currentOption = 0;
    vector<string> options = {"BACK"};
    vector<string> optionsAddOns = {""};
    bool rPressed = false;

    /*
    IN MENU
    */

    while (run) {
        rPressed = false;
        fStart = SDL_GetTicks();
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                socket->send("GOOD BYE", 4096, ADDRESS, PORT);
                *playAgain = false;
                *playAgainAddress = "^";
                *playAgainPort = "^";
                return -1;
            }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    socket->send("GOOD BYE", 4096, ADDRESS, PORT);
                    *playAgain = false;
                    *playAgainAddress = "^";
                    *playAgainPort = "^";
                    return 0;
                }
                if (e.key.keysym.sym == SDLK_r) {
                    rPressed = true;
                }
            }
        }

        SDL_SetRenderDrawColor(ren, 20, 20, 20, 255);
        SDL_RenderClear(ren);
        if (!rPressed) {
            socket->send("I AM HERE", 4096, ADDRESS, PORT);
        }
        else {
            socket->send("R PRESSED", 4096, ADDRESS, PORT);
        }
        
        if (socket->receive(rData, 4096, received, rAddr, rPort) != Socket::NotReady) {
            if (strcmp(rData, "START GAME") == 0) {
                run = false;
            }
            else {
                players.clear();
                json rjData = json::parse(rData);
                for (auto &pl : rjData["players"]) {
                    players.push_back(Player(0, 0, 0, "0.0.0.0", pl["name"]));
                    players[players.size() - 1].color[0] = pl["color1"];
                    players[players.size() - 1].color[1] = pl["color2"];
                    players[players.size() - 1].color[2] = pl["color2"];
                    players[players.size() - 1].rPressed = pl["ready"];
                }
            }
        }
        else {
            triedToConn++;
        }
        if (triedToConn >= 180) {
            menuOptionFont->render(ren, "Connecting to server...", 10, 10, NULL);
        }
        if (triedToConn >= 600) {
            *playAgain = false;
            *playAgainAddress = "^";
            *playAgainPort = "^";
            return 0;
        }

        menu::draw_player_in_menu_client(ren, players, nameFont, SCREEN_DIFF);
        messageFont->render(ren, "Press \"R\" when you're ready", 50, SCREEN_SIZE[1] - SCREEN_DIFF[1] * 50, NULL);
        SDL_RenderPresent(ren);

        fEnd = SDL_GetTicks();
        fDelta = (fEnd - fStart);
        if (fDelta < fTime) {
            SDL_Delay(fTime - fDelta);
        }

    }

    run = true;
    triedToConn = 0;
    vector<Bullet> bullets;
    bool canPressEnter = false;
    bool dataParsed;
    json jData;

    /*
    GAME STARTED
    */
    string lSurvivourString = allDiedStrings[rand()%allDiedStrings.size()];
    string oneSurvivedString = oneSurvived[rand()%oneSurvived.size()];

    bool pausePressed = false;
    int pauseCurrentOption = 0;

    Messages messages;
    SDL_ShowCursor(SDL_ENABLE);

    while (run) {
        fStart = SDL_GetTicks();
        dataParsed = false;

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                socket->send("GOOD BYE", 4096, ADDRESS, PORT);
                *playAgain = false;
                *playAgainAddress = "^";
                *playAgainPort = "^";
                return -1;
            }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_w && !pausePressed) {
                    upr = true;
                }
                if (e.key.keysym.sym == SDLK_s && !pausePressed) {
                    dpr = true;
                }
                if (e.key.keysym.sym == SDLK_d && !pausePressed) {
                    rpr = true;
                } 
                if (e.key.keysym.sym == SDLK_a && !pausePressed) {
                    lpr = true;
                }
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    if (pausePressed) {
                        pausePressed = false;
                    }
                    else {
                        pausePressed = true;
                        lmpr = false;
                        upr = false;
                        dpr = false;
                        rpr = false;
                        lpr = false;
                    }
                }
                if (e.key.keysym.sym == SDLK_RETURN && canPressEnter && !pausePressed) {
                    *playAgainAddress = addr;
                    *playAgainPort = port;
                    *playAgain = true;
                    socket->send("ENTER PRESSED", 4096, ADDRESS, PORT);
                    return 0;
                }

                //PAUSE CONTROL
                if (e.key.keysym.sym == SDLK_RETURN && pausePressed) {
                    if (pauseCurrentOption == 0) {
                        pausePressed = false;
                    }
                    if (pauseCurrentOption == 1) {
                        socket->send("GOOD BYE", 4096, ADDRESS, PORT);
                        *playAgain = false;
                        *playAgainAddress = "^";
                        *playAgainPort = "^";
                        return 0;
                    }
                }
                if (e.key.keysym.sym == SDLK_DOWN && pausePressed) {
                    pauseCurrentOption++;
                    if (pauseCurrentOption == 2) {
                        pauseCurrentOption = 0;
                    }
                }
                if (e.key.keysym.sym == SDLK_UP && pausePressed) {
                    pauseCurrentOption--;
                    if (pauseCurrentOption == -1) {
                        pauseCurrentOption = 1;
                    }
                } 
            }
            if (e.type == SDL_KEYUP && !pausePressed) {
                if (e.key.keysym.sym == SDLK_w) {
                    upr = false;
                }
                if (e.key.keysym.sym == SDLK_s) {
                    dpr = false;
                }
                if (e.key.keysym.sym == SDLK_d) {
                    rpr = false;
                } 
                if (e.key.keysym.sym == SDLK_a) {
                    lpr = false;
                }
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT && !pausePressed) {
                    lmpr = true;
                }
            }
            if (e.type == SDL_MOUSEBUTTONUP) {
                if (e.button.button == SDL_BUTTON_LEFT && !pausePressed) {
                    lmpr = false;
                }
            }

            if (lmpr) {
                SDL_GetMouseState(&mPos[0], &mPos[1]);
            }

            if ((lpr && rpr) || (!lpr & !rpr)) {
                xbutton = 0;
            }
            else if (lpr) {
                xbutton = 1;
            }
            else {
                xbutton = 2;
            }

            if ((upr && dpr) || (!upr && !dpr)) {
                ybutton = 0;
            }
            else if (upr) {
                ybutton = 1;
            }
            else {
                ybutton = 2;
            }
        }
        if (run == false) {
            *playAgain = false;
            *playAgainAddress = "^";
            *playAgainPort = "^";
            return 0;
        }

        json jsData = {
            {"xbutton", xbutton},
            {"ybutton", ybutton},
            {"lmpr", lmpr},
            {"mp1", mPos[0]},
            {"mp2", mPos[1]},
            {"scrDiff1", SCREEN_DIFF[0]},
            {"scrDiff2", SCREEN_DIFF[1]}
        };
        string sData = jsData.dump();
        socket->send(sData.c_str(), 4096, ADDRESS, PORT);


        SDL_SetRenderDrawColor(ren, 20, 20, 20, 255);
        SDL_RenderClear(ren);

        if (socket->receive(rData, 4096, received, rAddr, rPort) != Socket::NotReady){
            if (strcmp(rData, "GAME FINISHED") == 0) {
                *playAgainAddress = addr;
                *playAgainPort = port;
                *playAgain = true;
                return 0;
            }
            else {
                dataParsed = true;
                triedToConn = 0;
                players.clear();
                bullets.clear();
                jData = json::parse(rData);
                for (auto &lPlayer: jData["players"]) {
                    Player player(lPlayer["x"], lPlayer["y"], 0, rAddr, lPlayer["name"]);
                    player.color[0] = lPlayer["color1"];
                    player.color[1] = lPlayer["color2"];
                    player.color[2] = lPlayer["color3"];
                    player.dead = lPlayer["dead"];
                    player.lifes = lPlayer["lifes"];
                    player.port = lPlayer["port"];
                    players.push_back(player);
                }
                for (auto &lBullet: jData["bullets"]) {
                    Bullet bullet(lBullet["x"], lBullet["y"], 0, 0, rPort);
                    bullet.color[0] = lBullet["color1"];
                    bullet.color[1] = lBullet["color2"];
                    bullet.color[2] = lBullet["color3"];
                    bullets.push_back(bullet);
                }
            } 
        }
        else {
            triedToConn++;
        }

        for (auto &b: bullets) {
            b.draw(ren, SCREEN_DIFF);
        }
        for (auto &p: players) {
            p.draw(ren, nameFont, SCREEN_DIFF);
            if (p.port == selfPort) {
                menuOptionFont->render(ren, "LIFES: " + to_string(p.lifes), SCREEN_SIZE[0] - 200 * SCREEN_DIFF[0], SCREEN_SIZE[1] - SCREEN_DIFF[1] * 60, NULL, true, SCREEN_DIFF[0], SCREEN_DIFF[1]);
            }
        }

        if (triedToConn >= 180) {
            menuOptionFont->render(ren, "Connecting to server...", 50, 5, NULL);
        }
        if (triedToConn >= 600) {
            *playAgain = false;
            *playAgainAddress = "^";
            *playAgainPort = "^";
            return 0;
        }

        if (dataParsed && jData["winnerChecked"]) {
            canPressEnter = true;
            string winnerName = jData["winnerName"];
            if (getSurvived(players) == 0) {
                messageFont->render(ren, winnerName + lSurvivourString, 20, SCREEN_SIZE[1] - SCREEN_DIFF[1]*70, NULL);
            }
            else {
                messageFont->render(ren, winnerName + oneSurvivedString, 20, SCREEN_SIZE[1] - SCREEN_DIFF[1]*70, NULL);
            }  
        }
        if (dataParsed && (jData["canPressEnter"] || canPressEnter)) {
            messageFont->render(ren, "Press ENTER to play again", 20, SCREEN_SIZE[1] - SCREEN_DIFF[1]*40, NULL);
        }
        if (dataParsed) {
            messages.messages.clear();
            for (auto m: jData["messages"]) {
                messages.messages.push_back(m);
            }
            messages.draw(ren, messageFont, SCREEN_SIZE, SCREEN_DIFF, false);
        }

        if (pausePressed) {
            menu::drawPause(ren, pauseCurrentOption, menuOptionFont, SCREEN_SIZE, SCREEN_DIFF);
        }

        SDL_RenderPresent(ren);

        fEnd = SDL_GetTicks();
        fDelta = (fEnd - fStart);
        if (fDelta < fTime) {
            SDL_Delay(fTime - fDelta);
        }
    }

    return 0;
}

int main (int argc, char **argv) {
    init();
    UdpSocket socket;
    socket.bind(Socket::AnyPort);
    socket.setBlocking(false);

    SDL_ShowCursor(SDL_DISABLE);

    string playerName, port, address, playAgainAddress = "^", playAgainPort = "^";
    bool playAgain = false;
    menu::drawTitle(ren, titleFont, SCREEN_SIZE, SCREEN_DIFF);

    while (true) {
        SDL_ShowCursor(SDL_DISABLE);
        if (!playAgain) {
            int menuRes = menu::main_menu(ren, &address, &port, &playerName, menuOptionFont, SCREEN_SIZE, SCREEN_DIFF);
            if (menuRes == -1) {
                return 0;
            }
            if (menuRes == 1) {
                if (HOST(playerName, &socket, &playAgain) == -1) {
                    break;
                };
            }
            if (menuRes == 2) {
               if (CLIENT(address, port, playerName, &socket, &playAgainAddress, &playAgainPort, &playAgain) == -1) {
                   break;
               };
            }
        }
        else {
            if (playAgainAddress != "^" && playAgainPort != "^") {
                if (CLIENT(address, port, playerName, &socket, &playAgainAddress, &playAgainPort, &playAgain) == -1) {
                    break;
                }
            }
            else {
                if (HOST(playerName, &socket, &playAgain) == -1) {
                    break;
                };
            }

        }
         
    }
    quit();
    return 0;
}