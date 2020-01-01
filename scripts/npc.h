#ifndef NPC_H
#define NPC_H

#include "cube.h"
#include "bullet.h"
#include <cmath>
#include "sizeProperties.h"

class NPC {
	public:
		int playerIndex;
		int randomNextPosX;
		int randomNextPosY;

		NPC(int index) {
			this->playerIndex = index;
			this->randomNextPosX = rand()%(1366 - playerSize);
			this->randomNextPosY = rand()%(768 - playerSize);
		}

		void getBulletPos(vector<Player> players, short int bX, short int bY, char *xDir, char *yDir) {
			if (bX < players[this->playerIndex].rect.x) {*xDir = 'l';}
			else {*xDir = 'r';}
			if (bY < players[this->playerIndex].rect.y) {*yDir = 'u';}
			else {*yDir = 'd';}
		}

		void update(vector<Player> players, vector<Bullet> bullets, int *xButton, int *yButton, int *mPos1, int *mPos2) {
			float xDirSum = 0, yDirSum = 0, minDistance = 10000;
			int minDistancePlayerIndex = -1;
			for (int player = 0; player < players.size(); player++) {
				if (this->playerIndex != player && !players[player].dead) {
					float distance = std::sqrt((std::pow(abs(players[this->playerIndex].rect.x - players[player].rect.x), 2) + std::pow(abs(players[this->playerIndex].rect.y - players[player].rect.y), 2)));
					if (distance < minDistance) {
						minDistance = distance;
						minDistancePlayerIndex = player;
					}
					if (distance < 600) {
						if (players[this->playerIndex].rect.x < players[player].rect.x) {
							xDirSum -= 600 - distance;
						}
						else if (players[this->playerIndex].rect.x > players[player].rect.x) {
							xDirSum += 600 - distance;
						}

						if (players[this->playerIndex].rect.y < players[player].rect.y) {
							yDirSum -= 600 - distance;
						}
						else if (players[this->playerIndex].rect.y > players[player].rect.y) {
							yDirSum += 600 - distance;
						}
					}
				}
			}

			for (auto &bullet: bullets) {
				char xDir, yDir;
				getBulletPos(players, bullet.rect.x, bullet.rect.y, &xDir, &yDir);
					if (std::pow(std::abs(players[this->playerIndex].rect.x - bullet.rect.x), 2) + std::pow(std::abs(players[this->playerIndex].rect.y - bullet.rect.y), 2) < std::pow(100, 2) && \
					bullet.ownerIdentifier != players[this->playerIndex].selfIdentifier) {
						int xDiff = std::abs(players[this->playerIndex].rect.x - bullet.rect.x), yDiff = std::abs(players[this->playerIndex].rect.y - bullet.rect.y);
						if (xDir == 'l' && yDir == 'd' && xDiff >= 80 && yDiff >= 80) {
							xDirSum += 500;
							yDirSum -= 500;
							if (1366 - players[this->playerIndex].rect.x < 150) {
								xDirSum -= 1200;
							}
							if (players[this->playerIndex].rect.y < 110) {
								yDirSum +=1200;
							}
						}
						if (xDir == 'r' && yDir == 'd' && xDiff >= 80 && yDiff >= 80) {
							xDirSum -= 500;
							yDirSum -= 500;
							if (players[this->playerIndex].rect.x < 110) {
								xDirSum += 1200;
							}
							if (players[this->playerIndex].rect.y < 110) {
								yDirSum +=1200;
							}
						}
						if (xDir == 'l' && yDir == 'u' && xDiff >= 80 && yDiff >= 80) {
							xDirSum += 500;
							yDirSum += 500;
							if (1366 - players[this->playerIndex].rect.x < 150) {
								xDirSum -= 1200;
							}
							if (768 - players[this->playerIndex].rect.y < 150) {
								yDirSum -=1200;
							}
						}
						if (xDir == 'r' && yDir == 'u' && xDiff >= 80 && yDiff >= 80) {
							xDirSum -= 500;
							yDirSum += 500;
							if (players[this->playerIndex].rect.x < 110) {
								xDirSum += 1200;
							}
							if (768 - players[this->playerIndex].rect.y < 150) {
								yDirSum -=1200;
							}
						}
						if (xDir == 'l' && xDiff >= 80 && yDiff < 80) {
							if (yDir == 'u') {
								yDirSum += 500;
								if (768 - players[this->playerIndex].rect.y < 110) {
									yDirSum -= 1000;
								}
							}
							else {
								yDirSum -= 500;
								if (players[this->playerIndex].rect.y < 150) {
									yDirSum += 1000;
								}
							}
							xDirSum += 500;
						}
						if (xDir == 'r' && xDiff >= 80 && yDiff < 80) {
							if (yDir == 'u') {
								yDirSum += 500;
								if (768 - players[this->playerIndex].rect.y < 110) {
									yDirSum -= 1000;
								}
							}
							else {
								yDirSum -= 500;
								if (players[this->playerIndex].rect.y < 150) {
									yDirSum += 1000;
								}
							}
							xDirSum -= 500;
						}
						if (yDir == 'u' && yDiff >= 80 && xDiff < 80) {
							if (xDir == 'r') {
								xDirSum += 500;
								if (players[this->playerIndex].rect.x < 110) {
									xDirSum -= 1000;
								}
							}
							else {
								xDirSum -= 500;
								if (1366 - players[this->playerIndex].rect.x < 110) {
									xDirSum += 1000;
								}
							}
							yDirSum += 500;
						}
						if (yDir == 'd' && yDiff >= 80 && xDiff < 80) {
							if (xDir == 'r') {
								xDirSum += 500;
								if (players[this->playerIndex].rect.x < 110) {
									xDirSum -= 1000;
								}
							}
							else {
								xDirSum -= 500;
								if (1366 - players[this->playerIndex].rect.x < 110) {
									xDirSum += 1000;
								}
							}
							yDirSum -= 500;
						}
					}
			}

			if (xDirSum < 0) {*xButton = 1;}
			if (xDirSum > 0) {*xButton = 2;}
			if (xDirSum == 0) {*xButton = 0;}

			if (yDirSum < 0) {*yButton = 1;}
			if (yDirSum > 0) {*yButton = 2;}
			if (yDirSum == 0) { *yButton = 0;}

			*mPos1 = players[minDistancePlayerIndex].rect.x + (players[minDistancePlayerIndex].rect.w / 2) - 40 + std::rand()%80;
			*mPos2 = players[minDistancePlayerIndex].rect.y + (players[minDistancePlayerIndex].rect.h / 2) - 40 + std::rand()%80;

			if (players[this->playerIndex].winner) {
				if (players[this->playerIndex].rect.x < this->randomNextPosX) {*xButton = 2;}
				else if (players[this->playerIndex].rect.x > this->randomNextPosX) {*xButton = 1;}
				else {*xButton = 0;}

				if (players[this->playerIndex].rect.y < this->randomNextPosY) {*yButton = 2;}
				else if (players[this->playerIndex].rect.y > this->randomNextPosY) {*yButton = 1;}
				else {*yButton = 0;}

				if (std::abs(players[this->playerIndex].rect.x - this->randomNextPosX) < 10 &&\
					std::abs(players[this->playerIndex].rect.y - this->randomNextPosY) < 10) {
						this->randomNextPosX = rand()%(1366 - playerSize);
						this->randomNextPosY = rand()%(768 - playerSize);
					}
					

				*mPos1 = -1366 + rand()%2732;
				*mPos2 = -768 + rand()%1538;
			}	
		}		
};

#endif