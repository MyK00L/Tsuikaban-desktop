#pragma once

#include <SFML/Graphics.hpp>

class Resources {
public:
	Resources() {
		if (!(groundTexture.loadFromFile("res/ground.png") & wallTexture.loadFromFile("res/wall.png") & playerTexture.loadFromFile("res/player.png") & cubeTexture.loadFromFile("res/cube.png") &
					doorTexture.loadFromFile("res/door.png") & cursorTexture.loadFromFile("res/cursor.png") & font.loadFromFile("res/Hack-Regular.ttf"))) {
			throw;
		}
		wallSprite.setTexture(wallTexture);
		wallSprite.setScale(1.0f / wallTexture.getSize().x, 1.0f / wallTexture.getSize().y);
		groundSprite.setTexture(groundTexture);
		groundSprite.setScale(1.0f / groundTexture.getSize().x, 1.0f / groundTexture.getSize().y);
		doorSprite.setTexture(doorTexture);
		doorSprite.setScale(1.0f / doorTexture.getSize().x, 1.0f / doorTexture.getSize().y);
		cubeSprite.setTexture(cubeTexture);
		cubeSprite.setScale(1.0f / cubeTexture.getSize().x, 1.0f / cubeTexture.getSize().y);
		playerSprite.setTexture(playerTexture);
		playerSprite.setScale(1.0f / playerTexture.getSize().x, 1.0f / playerTexture.getSize().y);
		cursorSprite.setTexture(cursorTexture);
		cursorSprite.setScale(1.0f / cursorTexture.getSize().x, 1.0f / cursorTexture.getSize().y);
	}
	sf::Texture wallTexture, groundTexture, doorTexture, cubeTexture, playerTexture, cursorTexture;
	sf::Sprite wallSprite, groundSprite, doorSprite, cubeSprite, playerSprite, cursorSprite;
	sf::Font font;
};
