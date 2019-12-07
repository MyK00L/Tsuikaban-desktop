#include "Editor.h"
#include "LevelSelect.h"

#include <fstream>

void Editor::set_view() {
	if (h * window.getSize().x < w * window.getSize().y) {
		window.setView(sf::View(sf::FloatRect(0, 0, w, (window.getSize().y * w) / (float)window.getSize().x)));
	} else {
		window.setView(sf::View(sf::FloatRect(0, 0, (window.getSize().x * h) / (float)window.getSize().y, h)));
	}
}

void Editor::save() {
	std::ofstream ofs("./res/levels/level" + std::to_string(level) + ".txt");
	ofs << h << ' ' << w << '\n';
	for (auto &i : map)
		ofs << i << '\n';
	ofs << player.pos.x << ' ' << player.pos.y << '\n';
	ofs << cubes.size() << '\n';
	for (auto &i : cubes) {
		ofs << i.pos.x << ' ' << i.pos.y << ' ' << (int)i.col.r << ' ' << (int)i.col.g << ' ' << (int)i.col.b << ' ' << i.num << '\n';
	}
	ofs.close();
}
Editor::Editor(sf::RenderWindow &window, Resources &res, Settings &set, int level) : GameState(window, res, set), level(level) {
	window.setTitle("Tsuikaban - Editor (level " + std::to_string(level) + ")");
	last_edit = sf::Keyboard::Unknown;
	std::ifstream ifs("./res/levels/level" + std::to_string(level) + ".txt");
	if (!ifs.is_open()) {
		h = w = 16;
		map.resize(16);
		for (auto &i : map)
			i.resize(16, '.');
		player.pos = {1, 1};
		cubes.clear();
		save();
		ifs.open("./res/levels/level" + std::to_string(level) + ".txt");
	}
	ifs >> h >> w;
	map.resize(h);
	for (auto &i : map)
		ifs >> i;
	ifs >> player.pos.x >> player.pos.y;
	int ncubes;
	ifs >> ncubes;
	cubes.resize(ncubes);
	for (auto &i : cubes) {
		int r, g, b;
		ifs >> i.pos.x >> i.pos.y >> r >> g >> b >> i.num;
		i.col = sf::Color(r, g, b);
	}
	ifs.close();
	set_view();
}
void Editor::apply(sf::Keyboard::Key k) {
	static const sf::Color palette[10] = {
		{207, 122, 219},
		{255, 95, 75},
		{255, 219, 102},
		{30, 142, 184},
		{138, 189, 255},
		{255, 149, 74},
		{143, 43, 98},
		{202, 255, 66},
		{76, 77, 235},
		{112, 200, 140}
	};
	if ((k >= sf::Keyboard::Num0 && k <= sf::Keyboard::Num9) || (k >= sf::Keyboard::Numpad0 && k <= sf::Keyboard::Numpad9)) {
		if (k >= sf::Keyboard::Num0 && k <= sf::Keyboard::Num9) {
			k = (sf::Keyboard::Key)((int)k - (int)sf::Keyboard::Num0);
		} else {
			k = (sf::Keyboard::Key)((int)k - (int)sf::Keyboard::Numpad0);
		}
		unsigned j = cubes.size();
		for (unsigned i = 0; i < cubes.size(); ++i) {
			if (cubes[i].pos == pos) {
				j = i;
				break;
			}
		}
		if (j == cubes.size())
			cubes.push_back({pos, palette[k], 0});
		cubes[j].col = palette[k];
	} else if (k == sf::Keyboard::Add || k == sf::Keyboard::RShift || k == sf::Keyboard::Subtract || k == sf::Keyboard::LShift) {
		unsigned j = cubes.size();
		for (unsigned i = 0; i < cubes.size(); ++i) {
			if (cubes[i].pos == pos) {
				j = i;
				break;
			}
		}
		if (j == cubes.size())
			cubes.push_back({pos, palette[0], 0});
		cubes[j].num += (k == sf::Keyboard::Add || k == sf::Keyboard::RShift ? 1 : -1);
	} else if (k == sf::Keyboard::E) {
		for (unsigned i = 0; i < cubes.size(); ++i) {
			if (cubes[i].pos == pos) {
				if (i != cubes.size() - 1)
					std::swap(cubes[i], cubes[cubes.size() - 1]);
				cubes.pop_back();
				break;
			}
		}
	} else if (k == sf::Keyboard::P) {
		player.pos = pos;
	} else if (k == sf::Keyboard::W) {
		map[pos.y][pos.x] = '#';
	} else if (k == sf::Keyboard::Period) {
		map[pos.y][pos.x] = '.';
	} else if (k == sf::Keyboard::D) {
		map[pos.y][pos.x] = 'D';
	}
}
GameState *Editor::update() {
	sf::Event ev;
	while (window.pollEvent(ev)) {
		if (ev.type == sf::Event::Closed) {
			return 0;
		} else if (ev.type == sf::Event::Resized) {
			set_view();
		} else if (ev.type == sf::Event::KeyPressed) {
			if (ev.key.code == sf::Keyboard::Escape) {
				GameState *ret = new LevelSelect(window, res, set, 1);
				delete this;
				return ret;
			} else if (ev.key.code == sf::Keyboard::S) {
				save();
			} else if (ev.key.code == sf::Keyboard::Right) {
				if (pos.x < w - 1)
					pos.x++;
			} else if (ev.key.code == sf::Keyboard::Up) {
				if (pos.y)
					pos.y--;
			} else if (ev.key.code == sf::Keyboard::Left) {
				if (pos.x)
					pos.x--;
			} else if (ev.key.code == sf::Keyboard::Down) {
				if (pos.y < h - 1)
					pos.y++;
			} else {
				last_edit = ev.key.code;
				apply(ev.key.code);
			}
		}
	}
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
		pos = (sf::Vector2i)window.mapPixelToCoords(sf::Mouse::getPosition(window));
		if (pos.x >= 0 && pos.x < w && pos.y >= 0 && pos.y < h)
			apply(last_edit);
	}
	return this;
}

void Editor::draw() {
	window.clear();
	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			if (map[y][x] == '#') {
				res.wallSprite.setPosition(x, y);
				window.draw(res.wallSprite);
			} else if (map[y][x] == '.') {
				res.groundSprite.setPosition(x, y);
				window.draw(res.groundSprite);
			} else if (map[y][x] == 'D') {
				res.doorSprite.setPosition(x, y);
				window.draw(res.doorSprite);
			}
		}
	}
	for (auto &i : cubes) {
		res.cubeSprite.setPosition(i.pos.x, i.pos.y);
		res.cubeSprite.setColor(i.col);
		window.draw(res.cubeSprite);
		sf::Text txt(std::to_string(i.num), res.font, 32);
		txt.setFillColor({0, 0, 0});
		txt.setScale({0.5 / 32.0f, 0.5 / 32.0f});
		txt.setPosition(i.pos.x + 0.5f - txt.getGlobalBounds().left - txt.getGlobalBounds().width / 2.0f, i.pos.y + 0.5f - txt.getGlobalBounds().top - txt.getGlobalBounds().height / 2.0f);
		window.draw(txt);
	}
	res.playerSprite.setPosition(player.pos.x, player.pos.y);
	window.draw(res.playerSprite);
	res.cursorSprite.setPosition(pos.x, pos.y);
	window.draw(res.cursorSprite);
	window.display();
}
