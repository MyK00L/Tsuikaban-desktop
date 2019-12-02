#include "Game.h"
#include "LevelSelect.h"

#include <fstream>

const sf::Vector2i Game::dir[4] = {sf::Vector2i(1, 0), {0, -1}, {-1, 0}, {0, 1}};

void Game::set_view() {
	if (h * window.getSize().x < w * window.getSize().y) {
		window.setView(sf::View(sf::FloatRect(0, 0, w, (window.getSize().y * w) / (float)window.getSize().x)));
	} else {
		window.setView(sf::View(sf::FloatRect(0, 0, (window.getSize().x * h) / (float)window.getSize().y, h)));
	}
}
Game::Game(sf::RenderWindow &window, Resources &res, Settings &set, int level) : GameState(window, res, set), level(level) {
	window.setTitle("Tsuikaban - Level " + std::to_string(level));
	std::ifstream ifs("./res/levels/level" + std::to_string(level) + ".txt");
	if (!ifs.is_open())
		throw;
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

bool Game::move(sf::Vector2i pos, Game::Direction d, Game::UndoState &us) {
	if (pos.x < 0 || pos.y < 0 || pos.x >= w || pos.y >= h || map[pos.y][pos.x] == '#')
		return 0;
	const sf::Vector2i new_pos = pos + dir[d];
	if (pos == player.pos) {
		if (move(new_pos, d, us)) {
			player.pos = new_pos;
			return 1;
		} else {
			return 0;
		}
	}
	for (unsigned i = 0; i < cubes.size(); ++i) {
		if (cubes[i].pos == pos) {
			if (new_pos.x < 0 || new_pos.y < 0 || new_pos.x >= w || new_pos.y >= h || map[new_pos.y][new_pos.x] == 'D')
				return 0;
			for (unsigned j = 0; j < cubes.size(); ++j) {
				if (cubes[j].pos == new_pos && cubes[j].col == cubes[i].col) {
					us.num = cubes[i].num;
					us.col = cubes[i].col;
					us.alm = new_pos + dir[d];
					cubes[j].num += cubes[i].num;
					cubes[i].num = 0;
					return 1;
				}
			}
			if (move(new_pos, d, us)) {
				if (us.num == 0) {
					us.num = cubes[i].num;
					us.col = cubes[i].col;
				}
				cubes[i].pos = new_pos;
				return 1;
			} else {
				return 0;
			}
		}
	}
	us.alm = new_pos;
	return 1;
}

bool Game::undo() {
	if (undos.empty())
		return 0;
	sf::Vector2i d = undos.back().alm - player.pos;
	if (d.x == 1 || d.x == -1 || d.y == 1 || d.y == -1) {
		player.pos -= d;
		undos.pop_back();
		return 1;
	}
	if (d.y)
		d.y /= (d.y < 0 ? -d.y : d.y);
	if (d.x)
		d.x /= (d.x < 0 ? -d.x : d.x);
	player.pos -= d;
	unsigned lc = cubes.size();
	for (sf::Vector2i i = player.pos + d; i != undos.back().alm; i += d) {
		for (unsigned j = 0; j < cubes.size(); ++j) {
			if (cubes[j].pos == i) {
				if (cubes[j].pos == undos.back().alm - d)
					lc = j;
				cubes[j].pos -= d;
			}
		}
	}
	if (lc == cubes.size()) {
		cubes.push_back({undos.back().alm - d - d, undos.back().col, 0});
	}
	if (cubes[lc].num != undos.back().num) {
		cubes.push_back({undos.back().alm - d, undos.back().col, cubes[lc].num - undos.back().num});
		cubes[lc].num = undos.back().num;
	}
	undos.pop_back();
	return 1;
}

GameState *Game::update() {
	bool go_dir[4] = {0, 0, 0, 0};
	sf::Event ev;
	while (window.pollEvent(ev)) {
		if (ev.type == sf::Event::Closed) {
			return 0;
		} else if (ev.type == sf::Event::Resized) {
			set_view();
		} else if (ev.type == sf::Event::KeyPressed) {
			if (ev.key.code == sf::Keyboard::Escape) {
				GameState *ret = new LevelSelect(window, res, set, 0);
				delete this;
				return ret;
			} else if (ev.key.code == sf::Keyboard::Right) {
				go_dir[Direction::R] = 1;
			} else if (ev.key.code == sf::Keyboard::Up) {
				go_dir[Direction::U] = 1;
			} else if (ev.key.code == sf::Keyboard::Left) {
				go_dir[Direction::L] = 1;
			} else if (ev.key.code == sf::Keyboard::Down) {
				go_dir[Direction::D] = 1;
			} else if (ev.key.code == sf::Keyboard::U) {
				undo();
			} else if (ev.key.code == sf::Keyboard::R) {
				GameState *ret = new Game(window, res, set, level);
				delete this;
				return ret;
			}
		}
	}
	if (go_dir[0] && go_dir[2])
		go_dir[0] = go_dir[2] = 0;
	if (go_dir[1] && go_dir[3])
		go_dir[1] = go_dir[3] = 0;
	for (unsigned d = 0; d < 4; ++d) {
		if (go_dir[d]) {
			UndoState us;
			us.num = 0;
			if (move(player.pos, (Game::Direction)d, us)) {
				cubes.resize(std::remove_if(cubes.begin(), cubes.end(), [](Cube &c) -> bool { return c.num == 0; }) - cubes.begin());
				undos.push_back(us);
				if (undos.size() > set.get_max_undos()) {
					undos.erase(undos.begin());
				}
			}
		}
	}
	if (map[player.pos.y][player.pos.x] == 'D') {
		if (set.get_solved_levels() < level + 1)
			set.set_solved_levels(level + 1);
		GameState *ret = new LevelSelect(window, res, set, 0);
		delete this;
		return ret;
	}
	return this;
}

void Game::draw() {
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
	window.display();
}
