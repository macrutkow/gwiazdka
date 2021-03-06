// a-gwiazdka.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "iostream"
#include "iomanip"
#include "queue"
#include "string"
#include "math.h"
#include "ctime"

using namespace std;

const int n = 60; // rozmier mapy
const int m = 60; // rozmiar mapy
static int map[n][m];
static int closed_nodes_map[n][m]; // mapa zamkniętych pól (wypróbowanych)
static int open_nodes_map[n][m]; // mapa otwartych pól (niewypróbowanych)
static int dir_map[n][m]; // mapa kierunków
const int dir = 8; // liczba możliwych kierunków

static int dx[dir] = { 1, 1, 0, -1, -1, -1, 0, 1 };
static int dy[dir] = { 0, 1, 1, 1, 0, -1, -1, -1 };

class node
{
	// aktualna pozycja
	int xPos;
	int yPos;
	// dystans przebyty dotychczas
	int level;
	// priority=level + szacowany pozostały dystans
	int priority;  // im mnijszy tym wyższy priorytet

public:
	node(int xp, int yp, int d, int p)
	{
		xPos = xp; yPos = yp; level = d; priority = p;
	}

	int getxPos() const { return xPos; }
	int getyPos() const { return yPos; }
	int getLevel() const { return level; }
	int getPriority() const { return priority; }

	void updatePriority(const int & xDest, const int & yDest)
	{
		priority = level + estimate(xDest, yDest) * 10; //A*
	}

	// nadaje wyższy priorytet drogą prostą zamiast po skosie
	void nextLevel(const int & i) // i: kierunek
	{
		level += (dir == 8 ? (i % 2 == 0 ? 10 : 14) : 10);
	}

	// Funkcja szacuje pozostały dystans do celu.
	const int & estimate(const int & xDest, const int & yDest) const
	{
		static int xd, yd, d;
		xd = xDest - xPos;
		yd = yDest - yPos;


		// za pomocą algorytmu Manhattan
		d=abs(xd)+abs(yd);


		return(d);
	}
};

// Ustal priorytet ( w kolejce)
bool operator<(const node & a, const node & b)
{
	return a.getPriority() > b.getPriority();
}

// Algorytm A*
// Trasa jest zwracana jako łańcuch liczb kierunków
string pathFind(const int & xStart, const int & yStart,
	const int & xFinish, const int & yFinish)
{
	static priority_queue<node> pq[2]; // lista otwartych (niewypróbowanych) pól 
	static int pqi; // indeks listy pq
	static node* n0;
	static node* m0;
	static int i, j, x, y, xdx, ydy;
	static char c;
	pqi = 0;

	// resetuje pola 
	for (y = 0; y<m; y++)
	{
		for (x = 0; x<n; x++)
		{
			closed_nodes_map[x][y] = 0;
			open_nodes_map[x][y] = 0;
		}
	}

	// tworzy punkt startu i dodaje go na listę otwartych
	n0 = new node(xStart, yStart, 0, 0);
	n0->updatePriority(xFinish, yFinish);
	pq[pqi].push(*n0);
	open_nodes_map[x][y] = n0->getPriority(); // zaznacz na mapie otwartych pól

											  // A* szukanie
	while (!pq[pqi].empty())
	{
		// weź aktualne pole z największym priorytetem z listy otwartych pól
		n0 = new node(pq[pqi].top().getxPos(), pq[pqi].top().getyPos(),
			pq[pqi].top().getLevel(), pq[pqi].top().getPriority());

		x = n0->getxPos(); y = n0->getyPos();

		pq[pqi].pop(); // usuń pole z listy otwartych
		open_nodes_map[x][y] = 0;
		// zaznacz na liście zamkniętych pól
		closed_nodes_map[x][y] = 1;

		// zakończ szukanie gdy osiągniesz cel
		if (x == xFinish && y == yFinish)
		{
			// wygeneruj ścieżkę ze startu do mety
			// poprzez podążanie za kierunkami
			string path = "";
			while (!(x == xStart && y == yStart))
			{
				j = dir_map[x][y];
				c = '0' + (j + dir / 2) % dir;
				path = c + path;
				x += dx[j];
				y += dy[j];
			}

			// zbieranie śmieci
			delete n0;
			// opróżnij pozostałe pola
			while (!pq[pqi].empty()) pq[pqi].pop();
			return path;
		}

		// wygeneruj ruchy (pola dzieci) w pozostałych kierunkach
		for (i = 0; i<dir; i++)
		{
			xdx = x + dx[i]; ydy = y + dy[i];

			if (!(xdx<0 || xdx>n - 1 || ydy<0 || ydy>m - 1 || map[xdx][ydy] == 1
				|| closed_nodes_map[xdx][ydy] == 1))
			{
				// wygeneruj pole-dziecko
				m0 = new node(xdx, ydy, n0->getLevel(),
					n0->getPriority());
				m0->nextLevel(i);
				m0->updatePriority(xFinish, yFinish);

				// jeśli nie jest na liście dodaj je na nią
				if (open_nodes_map[xdx][ydy] == 0)
				{
					open_nodes_map[xdx][ydy] = m0->getPriority();
					pq[pqi].push(*m0);
					// oznacz jako kierunek pola rodzica
					dir_map[xdx][ydy] = (i + dir / 2) % dir;
				}
				else if (open_nodes_map[xdx][ydy]>m0->getPriority())
				{
					// uaktualni informacje o priorytecie
					open_nodes_map[xdx][ydy] = m0->getPriority();
					// uaktualnij informacje o polu-rodzicu
					dir_map[xdx][ydy] = (i + dir / 2) % dir;

					// zamień pole poprzez opróżnienie indeksu pq do innego poza polem
					//które ma zostać zamienione będzie zignorowane a nowe pole bedzie wepchniete na liste
					while (!(pq[pqi].top().getxPos() == xdx &&
						pq[pqi].top().getyPos() == ydy))
					{
						pq[1 - pqi].push(pq[pqi].top());
						pq[pqi].pop();
					}
					pq[pqi].pop(); // usunięcie chcianego pola

								   // opróżnij większy indeks pq do mniejszego
					if (pq[pqi].size()>pq[1 - pqi].size()) pqi = 1 - pqi;
					while (!pq[pqi].empty())
					{
						pq[1 - pqi].push(pq[pqi].top());
						pq[pqi].pop();
					}
					pqi = 1 - pqi;
					pq[pqi].push(*m0); // zastąp "lepszym" polem
				}
				else delete m0; // usuwa niepotrzebne dane
			}
		}
		delete n0; // usuwa niepotrzebne dane
	}
	return ""; // nie odnaleziono drogi
}

int main()
{
	srand(time(NULL));

	// create empty map
	for (int y = 0; y<m; y++)
	{
		for (int x = 0; x<n; x++) map[x][y] = 0;
	}

	// wypełananie macierzy znakiem '+'
	for (int x = n / 8; x<n * 7 / 8; x++)
	{
		map[x][m / 2] = 1;
	}
	for (int y = m / 8; y<m * 7 / 8; y++)
	{
		map[n / 2][y] = 1;
	}

	// losowe wyznaczanie startu i mety
	int xA, yA, xB, yB;
	switch (rand() % 8)
	{
	case 0: xA = 0; yA = 0; xB = n - 1; yB = m - 1; break;
	case 1: xA = 0; yA = m - 1; xB = n - 1; yB = 0; break;
	case 2: xA = n / 2 - 1; yA = m / 2 - 1; xB = n / 2 + 1; yB = m / 2 + 1; break;
	case 3: xA = n / 2 - 1; yA = m / 2 + 1; xB = n / 2 + 1; yB = m / 2 - 1; break;
	case 4: xA = n / 2 - 1; yA = 0; xB = n / 2 + 1; yB = m - 1; break;
	case 5: xA = n / 2 + 1; yA = m - 1; xB = n / 2 - 1; yB = 0; break;
	case 6: xA = 0; yA = m / 2 - 1; xB = n - 1; yB = m / 2 + 1; break;
	case 7: xA = n - 1; yA = m / 2 + 1; xB = 0; yB = m / 2 - 1; break;
	}

	cout << "Rozmiar mapy (X,Y): " << n << "," << m << endl;
	cout << "Start: " << xA << "," << yA << endl;
	cout << "Meta: " << xB << "," << yB << endl;
	// znajdź drogę
	clock_t start = clock();
	string route = pathFind(xA, yA, xB, yB);
	if (route == "") cout << "Pusta droga wygenerowana!" << endl;
	clock_t end = clock();
	double time_elapsed = double(end - start);
	cout << "Czas potrzebny na odkrycie drogi (ms): " << time_elapsed << endl;
	cout << "Droga:" << endl;
	cout << route << endl << endl;

	// podąża za drogą na mapie i wyświetla
	if (route.length()>0)
	{
		int j; char c;
		int x = xA;
		int y = yA;
		map[x][y] = 2;
		for (int i = 0; i<route.length(); i++)
		{
			c = route.at(i);
			j = atoi(&c);
			x = x + dx[j];
			y = y + dy[j];
			map[x][y] = 3;
		}
		map[x][y] = 4;

		// myświetla mapę z drogą
		for (int y = 0; y<m; y++)
		{
			for (int x = 0; x<n; x++)
				if (map[x][y] == 0)
					cout << ".";
				else if (map[x][y] == 1)
					cout << "O"; //przeszkody
				else if (map[x][y] == 2)
					cout << "S"; //start
				else if (map[x][y] == 3)
					cout << "R"; //droga
				else if (map[x][y] == 4)
					cout << "F"; //meta
			cout << endl;
		}
	}
	getchar(); 
	return(0);
}

