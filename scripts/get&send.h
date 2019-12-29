#include "cube.h"
#include "bullet.h"
#include <vector>
#include <SFML/Network.hpp>
#include <string>
#include <string.h>
#include "json.hpp"
#include <cmath>

using namespace std;
using namespace nlohmann;
using sf::UdpSocket;
using sf::Socket;
using sf::IpAddress;

int getPlayerIndex(vector<Player> players, unsigned short sPort) {
    for (int i = 0; i < players.size(); i++) {
        if (players[i].port == sPort) {
            return i;
        }
    }

    return -1;
}

int findPort(vector<int> ports, int port) {
	for (int p = 0; p < ports.size(); p++) {
		if (ports[p] == port) {
			return p;
		}
	}

	return -1;
}

bool checkReadyPlayers(vector<Player> players) {
	for (auto &player: players) {
		if (!player.rPressed) {return false;}
	}
	return true;
}

bool checkWinner(vector<Player> players) {
	short int countOfNotDead = 0;
	for (auto &player: players) {
		if (!player.dead) {countOfNotDead++;}
	}
	if (countOfNotDead == 1) {
		return true;
	}
	return false;
}

int getSurvived(vector<Player> players) {
	short int countOfNotDead = 0;
	for (auto &player: players) {
		if (!player.dead) {countOfNotDead++;}
	}
	return countOfNotDead;
}

string getWinner(vector<Player> players) {
	for (auto &player: players) {
		if (!player.dead) {return player.name;};
	}
	return "-";
}

int getWinnerIndex(vector<Player> players) {
	for (int p = 0; p < players.size(); p++) {
		if (!players[p].dead) {return p;}
	}
	return -1;
}

bool getBotsLeft(vector<Player> players) {
	for (auto &player: players) {
		if (player.port != 0 && !player.dead) {return false;};
	}
	return true;
}