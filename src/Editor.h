#include <string>
#include <vector>

#include "GameState.h"

class Editor : public GameState {
public:
	Editor(sf::RenderWindow &window, Resources &res, Settings &set, int level);
	virtual ~Editor() {}
	GameState *update();
	void draw();

private:
	struct Player {
		sf::Vector2i pos;
	} player;
	struct Cube {
		sf::Vector2i pos;
		sf::Color col;
		int num;
	};
	unsigned level;
	int h;
	int w;
	std::vector<std::string> map;
	std::vector<Cube> cubes;
	void set_view();
	void save();
	sf::Vector2i pos;
	sf::Keyboard::Key last_edit;
	void apply(sf::Keyboard::Key);
};
