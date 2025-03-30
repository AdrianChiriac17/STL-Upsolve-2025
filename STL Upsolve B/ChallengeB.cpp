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
map<string, User>old_user_db;
map<string, User>new_user_db;

//pentru cerinta B
//in mapa cheia va fi un buton X si valoarea va fi alta mapa de frecventa care inregistreaza
// cresteri de fiecare data cand se apasa pe un buton Y fix dupa butonul X (cheia este Y, valoarea este fr[Y])
map<string, map<string, int>>most_likely_to_be_pushed;

//structul il va folosi un set care se comporta ca un priority queue si sorteaza apasarile de buton (cheia)
//de la mapa de mai sus (descrescator dupa nr de apasari si in caz de egalitate, alfabetic (crescator lexicografic)
struct CustomComparator
{
	bool operator() (const pair<string, int>& a, const pair<string, int>& b) const
	{
		if (a.second == b.second)
			return a.first < b.first;
		return b.second < a.second;
	}
};

int main(int argc, char** argv)
{
	// Keep this line and only modify the input name here
	const char* filename = (argc < 2) ? "inputB.txt" : argv[1];
	ifstream fin(filename);

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

		for (auto& elem : button_db)
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

		if (aux.button_clicked != "null")//in cazul in care utilizatorul a apasat un buton pana la urma
		{
			if (old_user_db[user].click.empty()) //daca utilizatorul nu a apasat niciun buton
			{
				old_user_db[user].click.push_back(aux);//adaugam butonul apasat acum fara logica suplimentara
			}
			else if (old_user_db[user].click[old_user_db[user].click.size() - 1].button_clicked != aux.button_clicked) 
				//daca ultimul buton apasat de User1 este diferit de butonul apasat acum, adaugam butonul
				//apasat acum (pt a elimina apasarile multiple si succesive ale aceluiasi buton de catre User1)
			{
				old_user_db[user].click.push_back(aux);
			}
		}
	}

	//PT B
	int no_clicks_new_db;
	fin >> no_clicks_new_db;
	for (int i = 0; i < no_clicks_new_db; i++)
	{
		string user;
		int x, y;
		fin >> user >> x >> y;

		string pushed_button = "null";
		for (auto& elem : button_db)
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

		new_user_db[user].name = user;

		//numai in cazul in care User a apasat vreun buton il adaugam la lista sa
		if (pushed_button != "null") 
			new_user_db[user].click.push_back(aux);
		//ce difera fata de baza de data veche este ca ne intereseaza doar ultima apasare de buton deci
		// nu trebuie sa eliminam apasarile multiple de buton.
	}

	for (auto user : old_user_db)//parsam prin fiecare utilizator din map-ul vechi
	{
		//for de la 0 la n-2
		for (int i = 0; i < user.second.click.size() - 1; i++)//pentru a crea noua mapa de mape de frecvente
		{
			most_likely_to_be_pushed[user.second.click[i].button_clicked][user.second.click[i + 1].button_clicked]++;
		}
	}

	for (auto new_user : new_user_db)//pentru fiecare utilizator din cei noi veniti
	{
		//memoram ultima sa apasare de buton
		string pentru = new_user.second.click[new_user.second.click.size() - 1].button_clicked;

		set<pair<string, int>, CustomComparator> s;//se comporta ca un pq
		for (auto curr_butt : most_likely_to_be_pushed[pentru])
		{
			//incarcam setul cu valorile din mapa de frecventa a butonului "pentru"
			s.insert({ curr_butt.first,curr_butt.second });
		}
		
		//din cauza logicii de la linia 133 unde am prevenit apasarea de mai multe ori a aceluiasi
		// buton consecutiv, cand se construieste mapa de frecventa la for-ul de la linia 181 nu voi avea o
		// cheie numita B1 in map-ul de frecventa a butonului B1. nu e nevoie decat de extragerea primului
		// element din pq.
		pair<string, int> extracted = {"null",0 };
		if(!s.empty())  extracted = *(s.begin());

		//afisare
		cout << new_user.first << ": " << extracted.first << "\n";
		s.clear();
	}

	return 0;
}

// Your code will end here