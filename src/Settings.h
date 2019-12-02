#include <fstream>

class Settings {
public:
	Settings() : solved_levels(solved_levels_def), max_undos(max_undos_def) {
		std::ifstream ifs;
		ifs.open("data.dat");
		if (!ifs.is_open()) {
			save();
			ifs.open("data.dat");
		}
		unsigned solved_levels_encoded, max_undos_encoded;
		ifs >> solved_levels_encoded >> max_undos_encoded;
		solved_levels = decode(solved_levels_encoded);
		max_undos = decode(max_undos_encoded);
		if (solved_levels > solved_levels_max || max_undos > max_undos_max || solved_levels_encoded != encode(solved_levels) || max_undos_encoded != encode(max_undos)) {
			solved_levels = solved_levels_def;
			max_undos = max_undos_def;
			save();
		}
	}
	unsigned get_solved_levels() { return solved_levels; }
	unsigned get_max_undos() { return max_undos; }
	void set_solved_levels(unsigned val) {
		solved_levels = val;
		save();
	}
	void set_max_undos(unsigned val) {
		max_undos = val;
		save();
	}
	static constexpr unsigned builtin_levels = 17;

private:
	template <typename T> T gray(T n) { return n ^ (n >> 1); }
	template <typename T> T rev_gray(T g) {
		T n = 0;
		for (; g; g >>= 1)
			n ^= g;
		return n;
	}
	template <typename T> T encode(T n) { return (gray(n) * magic_mul) ^ magic_xor; }
	template <typename T> T decode(T n) { return rev_gray((n ^ magic_xor) / magic_mul); }
	void save() {
		std::ofstream ofs;
		ofs.open("data.dat");
		ofs << encode(solved_levels) << std::endl;
		ofs << encode(max_undos) << std::endl;
		ofs.close();
	}
	unsigned solved_levels;
	static constexpr unsigned solved_levels_max = 64;
	static constexpr unsigned solved_levels_def = 0;
	unsigned max_undos;
	static constexpr unsigned max_undos_max = 20;
	static constexpr unsigned max_undos_def = 3;
	static constexpr int magic_xor = 26959;
	static constexpr int magic_mul = 2719;
};
