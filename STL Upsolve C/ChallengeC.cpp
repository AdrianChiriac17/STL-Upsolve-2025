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



map<string, Button>a_db;
map<string, User>old_user_db;
map<string, User>new_user_db;

struct CustomComparator
{
	bool operator() (const pair<string, int>& a, const pair<string, int>& b) const
	{
		if (a.second == b.second)
			return a.first < b.first;
		return b.second < a.second;
	}
};

set<pair<string, int>, CustomComparator> use_me_later;

map<string, map<string, int>>most_likely_to_be_pushed;



int main(int argc, char** argv)
{
	// Keep this line and only modify the input name here
	const char* filename = (argc < 2) ? "inputC.txt" : argv[1];
	ifstream fin(filename);

	int screen_l, screen_c;
	fin >> screen_l >> screen_c;
	cout << screen_l << " " << screen_c << "\n";
	int no_buttons;
	fin >> no_buttons;
	for (int i = 0; i < no_buttons; i++)
	{
		Button but;
		fin >> but.id;
		fin >> but.top_left.x >> but.top_left.y >> but.bottom_right.x >> but.bottom_right.y;
		a_db.insert(make_pair(but.id, but));
	}

	//BAZA DE DATE VECHE
	int no_clicks;
	fin >> no_clicks;
	for (int i = 0; i < no_clicks; i++)
	{
		string user;
		int x, y;
		fin >> user >> x >> y;
		//acum verificam daca a fost apasat vreun buton
		string pushed_button = "null";

		for (auto& elem : a_db)
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

		Click aux;
		aux.button_clicked = pushed_button;
		aux.coords.x = x;
		aux.coords.y = y;

		old_user_db[user].name = user;

		if (aux.button_clicked != "null")
		{
			old_user_db[user].click.push_back(aux);
		}

	}


	//PT B
	/*
	cout << "Debug pentru cerinta b:\n";
	for (auto elem : new_user_db)
	{
		cout << elem.first << ": ";
		for (auto click : elem.second.click)
		{
			cout << click.button_clicked << " cu coordonatele " << click.coords.x << ", " << click.coords.y << "\n";
		}
		cout << "\n";
	}
	*/
	/*
	for (auto user : old_user_db)
	{
		cout << "Userul " << user.first << " a apasat pe :";
		for (auto elem : user.second.click)
		{
			cout << elem.button_clicked << " ";
		}
		cout << "\n";
	}
	*/

	for (auto user : old_user_db)
	{
		for (int i = 0; i < user.second.click.size() - 1; i++)
		{
			most_likely_to_be_pushed[user.second.click[i].button_clicked][user.second.click[i + 1].button_clicked]++;
		}
	}

	/*
	for (auto elem : most_likely_to_be_pushed)
	{
		cout << "pt " << elem.first << " avem:\n";
		for (auto but : elem.second)
		{
			cout << but.first << " de " << but.second << " ori\n";
		}
	}
	*/

	/*
	for (auto new_user : new_user_db)
	{
		cout << new_user.first << ": ";

		string pentru = new_user.second.click[new_user.second.click.size() - 1].button_clicked;
		string solution = "null";

		//cout << "La mine utilizatorul " << new_user.first << " a apasat ultima oara " << pentru << "\n";

		//pentru butonul pentru.. trebuie sa vad most likely pushed care sa NU fie el insusi
		set<pair<string, int>, CustomComparator> s;
		for (auto curr_butt : most_likely_to_be_pushed[pentru])
		{
			s.insert({ curr_butt.first,curr_butt.second });
			//cout << "inserez in set" << curr_butt.first << " " << curr_butt.second;
			//cout << "\n";
		}

		/*
		cout << "afisare set:\n";
		for (auto elem : s)
			cout << "{" << elem.first << ", " << elem.second << "}, ";
		cout << "\n";


		pair<string, int> extracted = *(s.begin());
		cout << extracted.first << "\n";
		s.clear();
	}
	*/

	//CERINTA C
	vector<string>flow_principal;
	//ni se da un input
	//de la acel input general pq-ul (bazat pe mapa de frecventa)
	//pentru fiecare posibil raspuns la pasul i din flow_principal
	//verificam daca a aparut pana acum posibilul raspuns de la pasul 0 pana la pasul i-1 (pt a evita ciclurile)
	//primul pe care l gasim care nu a aparut deja in flow il bagam, trecem la pasul urmator
	//daca trecem prin tot pq ul si nu avem niciun raspuns, inseamna ca acela e finalul flow ului
	string prev_flow_item;
	fin.ignore();
	fin >> prev_flow_item;
	flow_principal.push_back(prev_flow_item);
	string rasp = "null";
	do
	{
		rasp = "null";
		cout << "acum caut succesorul lui " << prev_flow_item << "\n";
		set<pair<string, int>, CustomComparator> s;
		for (auto curr_butt : most_likely_to_be_pushed[prev_flow_item])
		{
			s.insert({ curr_butt.first,curr_butt.second });
		}


		while (!s.empty())
		{
			pair<string, int> extracted = *(s.begin());
			//il caut daca e in vectorul meu deja de string
			auto it = find(flow_principal.begin(), flow_principal.end(), extracted.first);
			if (it != flow_principal.end())
			{
				//inseamna ca il avem deja
			}
			else
			{
				//buton nou, il adaugam, iesim din cautare.
				rasp = extracted.first;
				break;
			}
			s.erase(s.begin());
		}

		s.clear();

		cout << " acum rasp=" << rasp << "\n";

		if (rasp != "null")
		{
			flow_principal.push_back(rasp);
			prev_flow_item = rasp;
		}
		else
		{
			break;
		}
	} while (rasp != "null");

	cout << "\n";
	for (string elem : flow_principal)
		cout << elem << " ";
	cout << "\n";

	//merge flowul principal.
	//pentru fiecare utilizator de de la old user database, afisez momentan ce apasa el
	for (auto elem : old_user_db)
	{
		cout << elem.first << " a apasat :";
		for (auto st : elem.second.click)
			cout << st.button_clicked << " ";
		cout << "\n";
	}

	//in afara de user 5 la test 3 imi merg bine toate logurile
	for (auto elem : old_user_db)
	{
		vector<string>searchvect;
		for (auto st : elem.second.click)
			searchvect.push_back(st.button_clicked);

		//caut flowul meu in searchvect, gasesc cea mai mare secventa
		//...
	}

	return 0;
}

// Your code will end here