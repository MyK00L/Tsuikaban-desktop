#pragma once

#include "Resources.h"
#include "Settings.h"
#include <SFML/Graphics.hpp>

class GameState {
public:
	GameState(sf::RenderWindow &window, Resources &res, Settings &set) : window(window), res(res), set(set) {}
	virtual ~GameState() {}
	virtual GameState *update() = 0;
	virtual void draw() = 0;

protected:
	sf::RenderWindow &window;
	Resources &res;
	Settings &set;
};
