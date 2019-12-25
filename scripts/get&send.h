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