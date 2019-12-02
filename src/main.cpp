#include "Game.h"
#include "LevelSelect.h"

int main() {
	Resources res;
	Settings set;
	sf::ContextSettings cs(0, 0, 4);
	sf::RenderWindow window(sf::VideoMode(512, 512), "Tsuikaban", sf::Style::Default, cs);
	window.setFramerateLimit(30);
	window.setKeyRepeatEnabled(false);
	GameState *game = new LevelSelect(window, res, set, 0);

	while ((game = (game->update()))) {
		game->draw();
	}
	return 0;
}
