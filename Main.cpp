#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <windows.h>
#include <conio.h>
#include <algorithm>
#include <filesystem>

#define pi 3.14159265359

using namespace std;
namespace fs = std::filesystem;

class pt {
public:
	int x, y;

	pt() : x(0), y(0) {}
	pt(const int a, const int b) : x(a), y(b) {}
	pt operator+(const pt& a) const {
		return pt(this->x + a.x, this->y + a.y);
	}
	pt operator-(const pt& a) const {
		return pt(this->x - a.x, this->y - a.y);
	}
	pt operator*(const int& n) const {
		if (n)
			return pt(this->x * n, this->y * n);
		return pt(0, 0);
	}
	void operator=(const pt& a) {
		this->x = a.x;
		this->y = a.y;
	}
	bool operator==(const pt& a) const {
		return ((this->x == a.x) && (this->y == a.y)) ? true : false;
	}
	bool operator!=(const pt& a) const {
		return ((this->x != a.x) || (this->y != a.y)) ? true : false;
	}
};

class dpt {
public:
	double x, y;

	dpt() : x(0), y(0) {}
	dpt(const double a, const double b) : x(a), y(b) {}
	dpt operator+(const dpt& a) const {
		return dpt(this->x + a.x, this->y + a.y);
	}
	dpt operator-(const dpt& a) const {
		return dpt(this->x - a.x, this->y - a.y);
	}
	dpt operator*(const double& n) const {
		if (n)
			return dpt(this->x * n, this->y * n);
		return dpt(0, 0);
	}
	void operator=(const dpt& a) {
		this->x = a.x;
		this->y = a.y;
	}
	bool operator==(const dpt& a) const {
		return ((this->x == a.x) && (this->y == a.y)) ? true : false;
	}
	bool operator!=(const dpt& a) const {
		return ((this->x != a.x) || (this->y != a.y)) ? true : false;
	}
	pt to_pt() const {
		return pt(round(this->x), round(this->y));
	}
};

struct mb {
	const vector<pt>* _sides = nullptr;
	pt pos;
	int direct = 0;

	mb() : pos(pt(0, 0)), direct(0) {}
	mb(const pt& pos1, const int& dir) : pos(pos1), direct(dir)	 {}
	void operator=(const mb& a) {
		this->pos = a.pos;
		this->direct = a.direct;
	}
	void assign(const vector<pt>* new_sides) {
		_sides = new_sides;
	}
	void move(const int& steps) {
		pos = pos + ((*_sides)[direct] * steps);
	}
	void turn_on(const short& turns) {
		if (abs(turns) == 1) {
			direct += turns * 2;

			if (direct < 0)
				direct += (*_sides).size();
			else if (direct >= (*_sides).size())
				direct -= (*_sides).size();
		}
	}
};

const char wall = char(219), empt = char(176), invisible = char(32), man = char(202), purpose = char(157), mark = '+', tnt = 'O';
const vector<pt> sides = { pt(0, -1), pt(1, -1), pt(1, 0), pt(1, 1), pt(0, 1), pt(-1, 1), pt(-1, 0),  pt(-1, -1)};
const vector<char> csides = { 's', 'c', 'd', 'e', 'w', 'q', 'a', 'z' };
const string name_file = "\\mapp.txt";

void stopp(const std::string& message, const int i) {
	std::cout << message;
	system("pause >nul");
	if (i != 0)
		exit(0);
}

void setcur(int x, int y)
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

double distance(const dpt& a, const dpt& b) {
	return sqrt((double)pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

bool in_square(const pt& a, const pt& b) {
	return ((a.x > -1) && (a.y > -1) && (a.x < b.x) && (a.y < b.y)) ? true : false;
}

int move_to(const pt& from, const pt& to) {
	if (from != to) {
		const int a = to.x - from.x;
		if (!a) {
			if (to.y > from.y)
				return 4;
			else
				return 0;
		}
		else {
			const double k = double(to.y - from.y) / a;
			int index = (((8.0 * a * atan(k)) + (9.0 * pi * a) - (4.0 * pi * abs(a))) / (2.0 * pi * a));
			if (index == 8)
				index = 0;
			return index;
		}
	}
	else
		return -1;
}

int filesize(const string& way_to_file, const bool mode) {
	int size = 0;
	string s = "";
	ifstream f(way_to_file);
	if (f) {
		while (!f.eof()) {
			getline(f, s);
			if (mode) {
				if (s != "")
					++size;
			}
			else
				++size;
		}
		f.close();
		return size;
	}
	else
		stopp(string(__FUNCTION__) + "() -> File \"" + way_to_file + "\" not exist", 1);
}

string filestr(const string& way_to_file, const int index) {
	string s = "";
	if ((index < filesize(way_to_file, 0)) && (index > -1)) {
		ifstream f(way_to_file);
		for (int i = 0; i <= index; ++i)
			getline(f, s);
		f.close();
	}
	else
		stopp( string(__FUNCTION__) + "(" + way_to_file + ", " + to_string(index) + ") -> index out of filesize(" + way_to_file + ")", 1);
	return s;
}

vector<vector<vector<char>>> fill(const string& way_to_file, mb& bot) {
	if (filesize(way_to_file, 0) == filesize(way_to_file, 1)) {
		const int fsize = filesize(way_to_file, 1), strsize = filestr(way_to_file, 0).size();
		vector<vector<vector<char>>> v(strsize, vector<vector<char>> (fsize, vector<char> (2, empt)));
		ifstream f(way_to_file);
		char cur = ' ';

		for (int j = fsize - 1; j > -1; --j)
			for (int i = 0; i < strsize; ++i) {
				f >> cur;
				if (cur == '$') {
					bot = mb(pt(i, j), bot.direct);
					v[i][j][1] = empt;
				}
				else if (cur == '!')
					v[i][j][1] = purpose;
				else if (cur == '*')
					v[i][j][0] = tnt;
				else if (cur != '-')
					v[i][j][1] = wall;
			}
				

		f.close();

		return v;
	}
	else
		stopp(string(__FUNCTION__) + "() -> file \"" + way_to_file + "\" has empty lines. It's not right", 1);
}

vector<vector<vector<char>>> cut(const vector<vector<vector<char>>>& place, const mb& bot, const int& size) {
	const pt start = bot.pos - pt(size, size), ptsize(place.size(), place[0].size());
	vector<vector<vector<char>>> cutter(size * 2 + 1, vector<vector<char>> (size * 2 + 1, vector<char> (2, invisible)));

	for (int i = 0; i < cutter.size(); ++i)
		for (int j = 0; j < cutter[0].size(); ++j) {
			pt temp = start + pt(i, j);
			if (in_square(temp, ptsize))
				cutter[i][j] = place[temp.x][temp.y];
		}

	return cutter;
}

bool is_visible(const vector<vector<vector<char>>>& place, const pt& bot, const pt& point) {
	if (bot != point) {
		const double leng = sqrt(pow(point.x - bot.x, 2) + pow(point.y - bot.y, 2));
		dpt direction(double(point.x - bot.x) / leng, double(point.y - bot.y) / leng);
		int counter = 0;
		while (true) {
			dpt dtemp = dpt(bot.x, bot.y) + (direction * (counter * 0.25));
			pt temp = dtemp.to_pt();
			if (!in_square(temp, pt(place.size(), place[0].size())))
				return false;
			if (temp == point)
				return true;
			if (place[temp.x][temp.y][1] == wall)
				return false;
			if (distance(dtemp, dpt(bot.x, bot.y)) > leng)
				return true;
			++counter;
		}
	}
	else
		return true;
}

vector<vector<vector<char>>> hide(const vector<vector<vector<char>>>& place, const pt& bot) {
	vector<vector<vector<char>>> hiden_place = place;
	const pt ptsize(place.size(), place[0].size());
	for (int i = 0; i < ptsize.x; ++i)
			for (int j = 0; j < ptsize.y; ++j) {
			if (place[i][j][1] != invisible && place[i][j][1] != wall) {
				if ((!is_visible(place, bot, pt(i	, j))))
					hiden_place[i][j][1] = invisible;
			}
		}
	return hiden_place;
}

void show(const vector<vector<vector<char>>>& place, const mb& bot) {
	vector<vector<vector<char>>> cutter = cut(place, bot, 30);
	const pt ptsize(cutter.size(), cutter[0].size());
	string res = "";

	cutter[ptsize.x / 2][ptsize.y / 2][1] = man;

	/*if (bot.direct == 0) {
		for (int j = 0; j < ptsize.y; ++j) {
			for (int i = ptsize.x - 1; i > -1; --i) {
				res = res + cutter[i][j][1];
			}
			res += "|\n";
		}
	}
	else if (bot.direct == 2) {
		for (int i = ptsize.x - 1; i > -1; --i) {
			for (int j = ptsize.y - 1; j > -1; --j)
				res = res + cutter[i][j][1];
			res += "|\n";
		}
	}
	else if (bot.direct == 4) {
		for (int j = ptsize.y - 1; j > -1; --j) {
			for (int i = 0; i < ptsize.x; ++i)
				res = res + cutter[i][j][1];
			res += "|\n";
		}
	}
	else if (bot.direct == 6) {
		for (int i = 0; i < ptsize.x; ++i) {
			for (int j = 0; j < ptsize.y; ++j)
				res = res + cutter[i][j][1];
			res += "|\n";
		}
	}
	for (int i = 0; i <= ptsize.x; ++i)
		res += "=";

	cout << res << endl;
	res = "";*/
	cutter = hide(cutter, pt(ptsize.x / 2, ptsize.y / 2));

	if (bot.direct == 0) {
		for (int j = 0; j < ptsize.y; ++j) {
			for (int i = ptsize.x - 1; i > -1; --i) {
				res = res + cutter[i][j][1];
			}
			res += "|\n";
		}
	}
	else if (bot.direct == 2) {
		for (int i = ptsize.x - 1; i > -1; --i) {
			for (int j = ptsize.y - 1; j > -1; --j)
				res = res + cutter[i][j][1];
			res += "|\n";
		}
	}
	else if (bot.direct == 4) {
		for (int j = ptsize.y - 1; j > -1; --j) {
			for (int i = 0; i < ptsize.x; ++i)
				res = res + cutter[i][j][1];
			res += "|\n";
		}
	}
	else if (bot.direct == 6) {
		for (int i = 0; i < ptsize.x; ++i) {
			for (int j = 0; j < ptsize.y; ++j)
				res = res + cutter[i][j][1];
			res += "|\n";
		}
	}
	for (int i = 0; i <= ptsize.x; ++i)
		res += "=";

	cout << res;
}

bool isfile(string way_to_file) {
	ifstream f(way_to_file);
	if (!f)
		return false;
	return true;
}

int lastsign(string s, char symbol) { //index
	int index = -1;
	if (s.size() != 0) {
		for (int i = s.size() - 1; i >= 0; --i)
			if (s[i] == symbol) {
				index = i;
				break;
			}
	}
	return index;
}

void mkfile(string way_to_file, vector<string> lines) { //create a file with default lines in this
	string w = way_to_file.substr(0, lastsign(way_to_file, '\\'));
	fs::create_directories(w);
	ofstream f(way_to_file);
	if (lines.size() == 0)
		f << "";
	else {
		for (size_t i = 0; i < lines.size() - 1; ++i)
			f << lines[i] << endl;
		f << lines[lines.size() - 1];
	}
	f.close();
}

string chegin() {
	wchar_t buffer[MAX_PATH];
	wstring wway = L"";
	GetModuleFileName(NULL, buffer, sizeof(buffer) / sizeof(buffer[0]));
	wway = buffer;
	string way(wway.begin(), wway.end());
	way = way.substr(0, lastsign(way, '\\')) + name_file;
	if (isfile(way) == false) { //creating file with standard matrix of game map
		string s = "";
		vector<string> v(1, s);
		mkfile(way, v);
	}
	else {}
	return way;
}

int main() {
	const string wtf = chegin();
	mb bot;
	vector<vector<vector<char>>> place = fill(wtf, bot);
	const pt ptsize(place.size(), place[0].size());

	bot.assign(&sides);
	while (true){
		char c = _getch();
		if (c == csides[4]) {
			if (in_square(bot.pos + sides[bot.direct], ptsize)) {
				pt temp = bot.pos + sides[bot.direct];
				if (place[temp.x][temp.y][1] == purpose)
					stopp("You win!!!", 1);
				else if (place[temp.x][temp.y][0] == tnt)
					stopp("You lose!!!", 1);
				else if (place[temp.x][temp.y][1] != wall)
					bot.move(1);
			}
		}
		else if (c == csides[0]) {
			if (in_square(bot.pos + (sides[bot.direct] * (-1)), ptsize)) {
				pt temp = bot.pos + (sides[bot.direct] * (-1));
				if (place[temp.x][temp.y][1] == purpose)
					stopp("You win!!!", 1);
				else if (place[temp.x][temp.y][0] == tnt)
					stopp("You lose!!!", 1);
				else if (place[temp.x][temp.y][1] != wall)
					bot.move(-1);
			}
		}
		else if (c == csides[2])
			bot.turn_on(-1);
		else if (c == csides[6])
			bot.turn_on(1);
		else if (c == 'm') {
			if (place[bot.pos.x][bot.pos.y][1] == mark)
				place[bot.pos.x][bot.pos.y][1] = empt;
			else if (place[bot.pos.x][bot.pos.y][1] == empt)
				place[bot.pos.x][bot.pos.y][1] = mark;
		}
		else if (c == 'p') {
			pt temp = bot.pos + sides[bot.direct];
			if (in_square(temp, ptsize)) {
				place[temp.x][temp.y][1] = (place[temp.x][temp.y][1] == empt) ? wall : 
					((place[temp.x][temp.y][1] == wall) ? empt : place[temp.x][temp.y][1]);
			}
		}
		show(place, bot);
		Sleep(100);
		setcur(0, 0);
	}
}