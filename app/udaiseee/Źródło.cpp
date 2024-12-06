#include <conio.h>
#include <cstdlib>
#include <string.h>
#include <threads.h>
#include <stdio.h>


using namespace std;

const int WIDTH = 30;
const int HEIGHT = 7;


int gCP(const char* fileName, const char* key) {

	FILE* file = fopen(fileName, "r");

	if (file == NULL) {
		_cprintf("Otwarcie pliku nie powiodlo siê\n");
		return 0;
	}

	char line[256];
	while (fgets(line, sizeof(line), file)) {
		char* pos = strchr(line, '=');

		if (pos != NULL) {
			*pos = '\0';
			const char* found = line;

			int value = atoi(pos + 1);

			if (strcmp(found, key) == 0) {
				fclose(file);
				return value;
			}
		}
	}

	fclose(file);
	return 0;
}


void svS(int number) {

	FILE* outFile = fopen("score.txt", "w");

	if (outFile != NULL) {
		fprintf(outFile, "%d", number);
		fclose(outFile);
	}
}

int gtC() {
	int number = 0;

	FILE* inFile = fopen("score.txt", "r");

	if (inFile != NULL) {
		fscanf(inFile, "%d", &number);

		fclose(inFile);
	}

	return number;
}

struct Car
{
	int X;
	int Speed;
	int Direction;
};

struct Frog
{
	int X;
	int Y;
};

struct Stork
{
	int X;
	int Y;
	int Speed;
};

void clearScreen() {
	_cprintf("\033[H");
}

void initializeBoard(Car* cars, char (*board)[WIDTH]) {
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			board[i][j] = '#';
		}
	}

	srand(time(0));
	for (int i = 0; i < 5; i++) {
		cars[i].X = rand() % (WIDTH - 2) + 1;
		cars[i].Speed = rand() % 5 + 2;
		if (rand() % 2 == 0)
		{
			cars[i].Direction = -1;

		}
		else cars[i].Direction = 1;
	}
}

void drawBoard(Frog* p, Car* c, Stork* s, char (*b)[WIDTH], int fC, int cC, int fnC, int sC) {
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			if (i == p[0].Y && j == p[0].X) {
				_cprintf("\033[%dm%c\033[0m", fC, 'o');
			}
			else if (i == s[0].Y && j == s[0].X)
			{
				_cprintf("\033[%dm%c\033[0m", sC, '^');
			}
			else if (i > 0 && i <= 5) {
				bool isC = false;
				for (int k = 0; k < 5; k++) {
					if (i == k + 1 && j == c[k].X) {
						if (!((i == 3 && (j == 5 || j == 6 || j == 7)) || (i == 4 && (j == 18 || j == 19 || j == 20)))) {
							if (i == 1)  _cprintf("\033[33mX\033[0m");
							else if (i == 4) _cprintf("\033[96mX\033[0m");
							else _cprintf("\033[%dmX\033[0m", cC);
							isC = true;
							break;
						}
					}
				}
				if (!isC) {
					if (j == 0 || j == WIDTH - 1) {
						_cprintf("|");
					}
					else {
						if ((i == 3 && (j == 5 || j == 6 || j == 7)) || (i == 4 && (j == 18 || j == 19 || j == 20))) {
							_cprintf("\033[%dm#\033[0m", fnC);

						}
						else _cprintf("_");
					}
				}
			}
			else {
				if (i == 0 || i == HEIGHT - 1) {
					_cprintf("\033[%dm%c\033[0m", fnC, b[i][j]);
				}
				else _cprintf("%c", b[i][j]);
			}
		}
		_cprintf("\n");
	}
}

int checkAbs(int x) {
	if (x < 0) return -x;
	else return x;
}


void updateCars(int frame, Car* cars, Frog* player) {
	for (int i = 0; i < 5; i++) {

		if (i == 0) {
			if ((player[0].Y == 1 || player[0].Y == 2) && checkAbs(player[0].X - cars[0].X) <= 2)
				continue;
		}

		if (frame % cars[i].Speed == 0) {
			cars[i].X += cars[i].Direction;
			if (cars[i].X >= WIDTH - 1) {
				do {
					cars[i].X = rand() % (WIDTH - 2) + 1;
				} while (cars[i].X == player[0].X);

				if (rand() % 2 == 0)
				{
					cars[i].Direction = -1;

				}
				else cars[i].Direction = 1;
			}
			else if (cars[i].X <= 0) {
				do {
					cars[i].X = rand() % (WIDTH - 2) + 1;
				} while (cars[i].X == player[0].X);

				if (rand() % 2 == 0)
				{
					cars[i].Direction = -1;

				}
				else cars[i].Direction = 1;
			}
		}
		cars[i].Speed = rand() % 5 + 2;
	}
}


void updateStork(int frame, Stork* bird, Frog* player) {

	bird[0].Speed = rand() % 2 + 2;

	if (frame % bird[0].Speed == 0) {
		if (bird[0].X < player[0].X) bird[0].X += 1;
		else if (bird[0].X > player[0].X) bird[0].X -= 1;

		if (bird[0].Y < player[0].Y) bird[0].Y += 1;
		else if (bird[0].Y > player[0].Y) bird[0].Y -= 1;
	}


}

void updatePlayer(char input, Frog* player) {
	int tempX = 0;
	int tempY = 0;

	if (input == 'a') {
		if (player[0].X == 1) {
			tempX = player[0].X;
			tempY = player[0].Y;
		}
		else {
			tempX = player[0].X - 1;
			tempY = player[0].Y;
		}
	}
	else if (input == 'd') {
		if (player[0].X == 28) {
			tempX = player[0].X;
			tempY = player[0].Y;
		}
		else {
			tempX = player[0].X + 1;
			tempY = player[0].Y;
		}
	}
	else if (input == 'w' && player[0].Y > 0) {
		tempY = player[0].Y - 1;
		tempX = player[0].X;
	}
	else if (input == 's') {
		if (player[0].Y == 6) {
			tempY = player[0].Y;
			tempX = player[0].X;
		}
		else {
			tempY = player[0].Y + 1;
			tempX = player[0].X;
		}
	}

	if ((tempY == 3 && (tempX == 5 || tempX == 6 || tempX == 7)) || (tempY == 4 && (tempX == 18 || tempX == 19 || tempX == 20))) {}
	else {
		player[0].X = tempX;
		player[0].Y = tempY;
	}
}

bool checkCollision(Frog* player, Car* cars) {
	for (int i = 0; i < 5; i++) {
		if (cars[3].X == player[0].X && player[0].Y == 4) {
			int newX = 0;
			do {
				newX = rand() % 30;
				if (newX > 2 && newX < 13) break;
				if (newX > 24 && newX < 28) break;
			} while (true);

			if (cars[3].Direction == 1) {
				cars[3].X = newX + 1;
			}
			else {
				cars[3].X = newX - 1;
			}
			player[0].X = newX;
			return false;
		}
		else {
			if (player[0].Y == i + 1 && player[0].X == cars[i].X) {
				return true;
			}
		}
	}
	//if (player[0].X == bird[0].X && player[0].Y == bird[0].Y) return true;
	return false;
}

bool checkStork(Frog* player, Stork* bird)
{
	if (player[0].X == bird[0].X && player[0].Y == bird[0].Y) return true;
	return false;
}


bool checkWin(Frog* player) {

	if (player[0].Y == 0) {
		return true;
	}
	return false;
}

int main() {
	Frog plr;
	plr.X = WIDTH / 2;
	plr.Y = HEIGHT - 1;
	Car cars[5];
	Stork bird;
	bird.X = 1;
	bird.Y = 1;
	char b[HEIGHT][WIDTH];
	int frm = 0;
	int tr = 0;
	int sL = gCP("cfg.txt", "TIME");
	int fC = gCP("cfg.txt", "FROG");
	int cC = gCP("cfg.txt", "CAR");
	int fnC = gCP("cfg.txt", "FINISH");
	int sC = gCP("cfg.txt", "STORK");

	initializeBoard(cars, b);


	while (true) {

		clearScreen();
		drawBoard(&plr, cars, &bird, b, fC, cC, fnC, sC);

		if (checkCollision(&plr, cars)) {
			_cprintf("Kolizja\n");
			break;
		}
		if (checkStork(&plr, &bird)) {
			_cprintf("Zlapal cie\n");
			break;
		}

		if (checkWin(&plr)) {
			int ps = sL * 10;
			int h = gtC();
			if (ps > h) {
				_cprintf("Sukces wynik to: %d, nowy rekord!\n", ps);
				svS(ps);
			}
			else _cprintf("Sukces wynik to: %d, dotychczasowy rekord: %d\n", ps, h);
			break;
		}

		if (_kbhit()) {
			char input = _getch();
			updatePlayer(input, &plr);
		}

		updateStork(frm, &bird, &plr);
		updateCars(frm, cars, &plr);

		if (sL == 0) {
			_cprintf("Koniec czasu.\n");
			return 0;
		}

		frm++;
		tr++;
		struct timespec ts;
		ts.tv_sec = 0;
		ts.tv_nsec = 100000000;

		thrd_sleep(&ts, NULL);

		if (tr == 10) {
			tr = 0;
			sL--;
		}

		_cprintf("\nczas: %d s.\n", sL);
	}
	int n;
	_cprintf("\n");
	_cscanf("%d", &n);
	return 0;
}
