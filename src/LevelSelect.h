#include <string>
#include <vector>

#include "GameState.h"

class LevelSelect : public GameState {
public:
	LevelSelect(sf::RenderWindow &window, Resources &res, Settings &set, bool to_edit);
	virtual ~LevelSelect() {}
	GameState *update();
	void draw();

private:
	void set_view();
	int nlevels;
	const bool to_edit;
	static constexpr int button_radius = 32;
	static constexpr int button_spacing = 16;
	std::vector<sf::Vector2i> buttons;
};
