#include "main.hpp"

unsigned int count_players = 0UL;

const std::vector<std::string> aviableNamesForBots = {
	"Liam",
	"Noah",
	"James",
	"Lucas",
	"Jacob",
	"Henry",
	"Michael",
	"Deniel",
	"David",
	"John",
	"Jack",
	"Luke",
	"Gabriel",
	"Ryan",
	"Charles",
	"Nolan",
	"Adam",
	"Amelia",
	"Olivia",
	"Sophia",
	"Isabella",
	"Riley",
	"Elsa",
	"Sarah",
	"Mary",
	"Zoji",
	"Lily",
	"Grace",
	"Zoe",
	"Bella",
	"Julia",
	"Piper",
	"Alice",
	"Samanta",
	"Maya",
	"Emma",
	"Anna",
	"Lucy",
	"Allison",
	"Sam",
	"Ford",
	"Jimmy",
	"Douglass",
	"Nelson",
	"Ben",
	"Bob",
	"Carl",
	"Ray",
	"Byron",
	"Flynn",
	"Kelvin",
	"Fox",
	"Billy",
	"Miller",
	"Willie",
	"Ralph",
	"Matilda",
	"Holly",
	"Julieta",
	"Emmy",
	"Greta",
	"Poppy",
	"Leah",
	"Olya",
	"Rosa",
	"Yara",
	"Anya",
	"Frida",
	"Lara",
	"Dana",
	"Della",
	"Erica",
	"Patricia",
	"Zola",
	"Robin",
	"Maliah"
};

Player::Player(const std::string &nickname)
	: name(nickname.empty() ? aviableNamesForBots[rd::integral(0UL, aviableNamesForBots.size() - 1)] : nickname)
	, id_color(100 + ++count_players)
	, lifes(3)
{
    Color::CreateColor(id_color);
}