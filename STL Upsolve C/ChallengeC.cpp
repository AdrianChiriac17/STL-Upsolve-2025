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

	//CITIRI
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

		if (aux.button_clicked != "null")
		{
			old_user_db[user].click.push_back(aux);
		}

	}
	
	//REZOLVARE

	//generez mapa de frecvente (cod identic cu cerinta B, dar necesar ca sa creez flowul principal
	for (auto user : old_user_db)
	{
		for (int i = 0; i < user.second.click.size() - 1; i++)
		{
			most_likely_to_be_pushed[user.second.click[i].button_clicked][user.second.click[i + 1].button_clicked]++;
		}
	}

	//CERINTA C
	vector<string>flow_principal;
	//ni se da un input
	//de la acel input generam pq-ul (bazat pe mapa de frecventa)
	//pentru fiecare posibil raspuns la pasul i din flow_principal
	//verificam daca a aparut pana acum posibilul raspuns de la pasul 0 pana la pasul i-1 (pt a evita ciclurile)
	//primul pe care il gasim care nu a aparut deja in flow il bagam, trecem la pasul urmator
	//daca trecem prin tot pq ul si nu avem niciun raspuns, inseamna ca acela e finalul flow ului

	string prev_flow_item;
	fin >> prev_flow_item;

	//generare flow principal
	flow_principal.push_back(prev_flow_item);
	string rasp = "null";
	do
	{
		rasp = "null";

		set<pair<string, int>, CustomComparator> s;
		for (auto curr_butt : most_likely_to_be_pushed[prev_flow_item])
		{
			s.insert({ curr_butt.first,curr_butt.second });
		}

		while (!s.empty())
		{
			pair<string, int> extracted = *(s.begin());
			//il caut daca e deja in vectorul meu  de string
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

		s.clear();//in cazul in care nu s a golit deja pq-ul

		if (rasp != "null")
		{
			flow_principal.push_back(rasp);
			prev_flow_item = rasp;
		}
		else
		{
			break;//am ajuns la finalul flowului
		}

	} while (rasp != "null");

	//afisez elementele flowului principal
	for (string elem : flow_principal)
		cout << elem << " ";
	cout << "\n";

	
	//formez toate subarray-urile posibile din flow_principal care au cel putin 3 elemente
	//le bag intr un vector de vector
	vector<vector<string>>sub_flow_principal;
	int flow_size = flow_principal.size();
	for (int flow_st = 0; flow_st <= flow_size - 3; flow_st++)
	{
		for (int flow_dr = flow_size - 1; flow_dr >= flow_st + 2; flow_dr--)
		{
			vector<string> search_me(flow_principal.begin() + flow_st, flow_principal.begin() + flow_dr + 1);
			sub_flow_principal.push_back(search_me);
		}
	}

	for (auto elem : old_user_db)
	{
		//pt fiecare user voi crea un vector de string al clickurilor sale
		// (ei au stocat vector de struct de click)
		vector<string>user_flow;
		for (auto st : elem.second.click)
			user_flow.push_back(st.button_clicked);

		string username = elem.first;
		int best_len = -1;
		int left_sol = -1, right_sol = 0;

		//trecem prin fiecare subsir al flowului principal si il cautam in intregime in user_flow.
		for (const auto& subarray : sub_flow_principal)
		{
			//cautam in user_flow pe subarray
			auto it = search(user_flow.begin(), user_flow.end(), subarray.begin(), subarray.end());

			if (it != user_flow.end())//am gasit toata secventa subarray in user_flow
			{
				int left_pretendent = distance(user_flow.begin(), it);
				int len = subarray.size();
				int right_pretendent = left_pretendent + len - 1;

				if (len > best_len) {
					best_len = len;
					left_sol = left_pretendent;
					right_sol = right_pretendent;
				}
				else if (len == best_len)
				{
					//caz in care am gasit deja o secventa de aceeasi marime
					//dar cea de acum e mai in stanga
					if (left_pretendent < left_sol)
					{
						best_len = len;
						left_sol = left_pretendent;
						right_sol = right_pretendent;
					}
				}
			}
		}
	
		//discutie daca sa afisez
		if (best_len >= 3)
		{
			cout << username << " ";
			for (auto elem : user_flow)
				cout << elem << " ";
			cout << left_sol << " " << right_sol << endl;
		}
	}

	return 0;
}

// Your code will end here