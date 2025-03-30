#include <iostream> // Input/Output
#include <fstream>
#include <sstream>
#include <string>
#include <vector> // Containers
#include <array>
#include <list>
#include <deque>
#include <queue>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <algorithm> // Algorithms & Iterators
#include <iterator>
#include <numeric>
#include <functional>
#include <memory> // Memory & Smart Pointers
#include <tuple> // Utility
#include <limits>
#if __has_include(<ranges>)
#include <ranges>
#endif
#include <cmath> // Math & Complex Numbers
#include <complex>
#include <regex> // Miscellaneous
#include <valarray>

// Your code will start here
using namespace std;

struct xy
{
	int x;
	int y;
};

struct Button
{
	string id;
	xy top_left;
	xy bottom_right;
	set<string>users_who_pushed;
};

struct Click
{
	string button_clicked;
	xy coords;
};

struct User
{
	string name;
	vector<Click> click;
};

map<string, Button>button_db;
map<string, User>user_db;

int main(int argc, char** argv)
{
	// Keep this line and only modify the input name here
	const char* filename = (argc < 2) ? "inputA.txt" : argv[1];
	ifstream fin(filename);

	//Citiri
	int screen_l, screen_c;
	fin >> screen_l >> screen_c;

	int no_buttons;
	fin >> no_buttons;
	for (int i = 0; i < no_buttons; i++)
	{
		Button but;
		fin >> but.id; 
		fin >> but.top_left.x >> but.top_left.y >> but.bottom_right.x >> but.bottom_right.y;
		button_db.insert(make_pair(but.id, but)); 
	}

	int no_clicks;
	fin >> no_clicks;
	for (int i = 0; i < no_clicks; i++)
	{
		string user;
		int x, y;
		fin >> user >> x >> y;
		string pushed_button = "null";//initializare cu valoare aleatorie

		//acum verificam daca User a apasat vreun buton in functie de coordonatele x,y ale clickului sau.
 		for (auto& elem : button_db) // elem.first = numele butonului, elem.second = struct de Buton.
		{
			if ((elem.second.top_left.x <= x && x <= elem.second.bottom_right.x) &&
				(elem.second.top_left.y <= y && y <= elem.second.bottom_right.y))
			{
				//a fost apasat
				pushed_button = elem.first;
				elem.second.users_who_pushed.insert(user); 
				break;
			}
		}

		//baza de date (map-ul) cu butoanele pe care a dat click fiecare user e necesara, dar nu la challenge A
		/* 
		Click aux;
		aux.button_clicked = pushed_button;
		aux.coords.x = x;
		aux.coords.y = y;

		user_db[user].name = user;
		user_db[user].click.push_back(aux);
		*/
	}
	
	//rezolvare
	for (auto& elem : button_db) // elem.first numele butonului, elem.second struct de Buton
	{
		if (!elem.second.users_who_pushed.empty()) //daca a apasat vreun user acest buton
		{
			cout << elem.first << ": ";
			for (auto user_who_pushed : elem.second.users_who_pushed)
			{
				cout << user_who_pushed << " ";
			}
			cout << "\n";
		}
	}

	return 0;
}

// Your code will end here