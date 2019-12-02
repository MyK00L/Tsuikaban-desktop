#include "LevelSelect.h"
#include "Editor.h"
#include "Game.h"
#include <fstream>

void LevelSelect::set_view() {
	window.setView(sf::View(sf::FloatRect(0, 0, window.getSize().x, window.getSize().y)));
	int buttons_per_row = (window.getSize().x - button_spacing) / (button_spacing + button_radius * 2);
	if (buttons_per_row < 1)
		buttons_per_row = 1;
	for (unsigned i = 0; i < buttons.size(); ++i) {
		buttons[i].x = (i % buttons_per_row) * (button_spacing + button_radius * 2) + button_spacing;
		buttons[i].y = (i / buttons_per_row) * (button_spacing + button_radius * 2) + button_spacing;
	}
}

LevelSelect::LevelSelect(sf::RenderWindow &window, Resources &res, Settings &set, bool to_edit) : GameState(window, res, set), to_edit(to_edit) {
	window.setTitle(to_edit ? "Tsuikaban - Level Select (editor)" : "Tsuikaban - Level Select");
	std::ifstream ifs("res/levels/level0.txt");
	nlevels = 0;
	while (ifs.is_open()) {
		nlevels++;
		ifs.close();
		ifs.open("res/levels/level" + std::to_string(nlevels) + ".txt");
	}
	buttons.resize(nlevels + to_edit);
	set_view();
}

GameState *LevelSelect::update() {
	sf::Event ev;
	while (window.pollEvent(ev)) {
		if (ev.type == sf::Event::Closed) {
			return 0;
		} else if (ev.type == sf::Event::Resized) {
			set_view();
		} else if (ev.type == sf::Event::MouseButtonPressed) {
			if (ev.mouseButton.button == sf::Mouse::Left) {
				for (unsigned i = 0; i < buttons.size(); ++i) {
					const int dx = (buttons[i].x + button_radius) - sf::Mouse::getPosition(window).x;
					const int dy = (buttons[i].y + button_radius) - sf::Mouse::getPosition(window).y;
					if (dx * dx + dy * dy <= (button_radius * button_radius) && (i <= set.get_solved_levels() || (i >= Settings::builtin_levels))) {
						GameState *ret = to_edit ? (GameState *)new Editor(window, res, set, i) : (GameState *)new Game(window, res, set, i);
						delete this;
						return ret;
					}
				}
			}
		} else if (ev.type == sf::Event::KeyPressed) {
			if (ev.key.code == sf::Keyboard::Space) {
				GameState *ret = new LevelSelect(window, res, set, !to_edit);
				delete this;
				return ret;
			}
		}
	}
	return this;
}

void LevelSelect::draw() {
	window.clear();
	sf::CircleShape circle(button_radius);
	circle.setFillColor({31, 31, 31});
	circle.setOutlineThickness(4.0f);
	for (unsigned i = 0; i < buttons.size(); ++i) {
		circle.setPosition(buttons[i].x, buttons[i].y);
		const int dx = (buttons[i].x + button_radius) - sf::Mouse::getPosition(window).x;
		const int dy = (buttons[i].y + button_radius) - sf::Mouse::getPosition(window).y;
		if (dx * dx + dy * dy <= (button_radius * button_radius) && (i <= set.get_solved_levels() || (i >= Settings::builtin_levels))) {
			circle.setOutlineColor({255, 255, 255});
		} else {
			circle.setOutlineColor({127, 127, 127});
		}
		window.draw(circle);

		sf::Text txt(std::to_string(i), res.font, button_radius);
		if (to_edit && i == buttons.size() - 1)
			txt.setString("+");
		if (i > set.get_solved_levels() && (i < Settings::builtin_levels))
			txt.setFillColor({0, 0, 0});
		txt.setPosition(buttons[i].x + button_radius - txt.getGlobalBounds().left - txt.getGlobalBounds().width / 2.0f,
										buttons[i].y + button_radius - txt.getGlobalBounds().top - txt.getGlobalBounds().height / 2.0f);
		window.draw(txt);
	}
	window.display();
}
