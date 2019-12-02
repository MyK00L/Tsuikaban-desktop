#include <string>
#include <vector>

#include "GameState.h"

class Game : public GameState {
public:
	Game(sf::RenderWindow &window, Resources &res, Settings &set, int level);
	virtual ~Game() {}
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
	enum Direction { R, U, L, D };
	struct UndoState {
		sf::Vector2i alm; // position after the last moved thing
		sf::Color col;		// color of the last moved cube
		int num;					// number of last moved cube (before moving)
	};
	std::vector<UndoState> undos;
	static const sf::Vector2i dir[4];

	unsigned level;
	int h;
	int w;
	std::vector<std::string> map;
	std::vector<Cube> cubes;
	void set_view();
	bool undo();
	bool move(sf::Vector2i pos, Direction dir, UndoState &us);
};
