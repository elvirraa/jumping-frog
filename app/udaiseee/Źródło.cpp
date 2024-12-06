#include <conio.h>
#include <cstdlib>
#include <string.h>
#include <threads.h>
#include <stdio.h>


using namespace std;


const int NC = 7;
const int W = 30;
const int H = 9;

typedef struct
{
	int X;
	int Speed;
	int Direction;
} Car;


typedef struct
{
	int X;
	int Y;
	bool moveActive;
} Frog;


typedef struct
{
	int X;
	int Y;
	int Speed;
} Stork;



// Function to read a value from a configuration file
int gCP(const char* fileName, const char* key) {
	// Open the file
	FILE* file = fopen(fileName, "r");
	// If the file could not be opened
	if (file == NULL) {
		_cprintf("Otwarcie pliku nie powiodlo siê\n");
		return 0;
	}

	char line[256];
	// Read the file line by line
	while (fgets(line, sizeof(line), file)) {
		// Find the separator
		char* pos = strchr(line, '=');
		// If you found the separator
		if (pos != NULL) {
			*pos = '\0';
			const char* found = line;
			// Convert value
			int value = atoi(pos + 1);
			// Close the file if you found the value
			if (strcmp(found, key) == 0) {
				fclose(file);
				return value;
			}
		}
	}

	fclose(file);
	return 0;
}


// Function to save a score to a file
void svS(int number) {
	// Open the file
	FILE* file = fopen("score.txt", "w");
	// Check if the file was opened successfully
	if (file != NULL) {
		fprintf(file, "%d", number);
		fclose(file);
	}
}


// Function to get the current score from a file
int gtC() {
	int number = 0;
	// Open the file
	FILE* file = fopen("score.txt", "r");
	// Check if the file was opened successfully
	if (file != NULL) {
		fscanf(file, "%d", &number);

		fclose(file);
	}

	return number;
}


// Function that has every frame of the game saved
void saveFrame(char (*board)[W]) {
	FILE* file = fopen("replay.txt", "a");
	if (file != NULL) {
		for (int i = 0; i < H; i++) {
			for (int j = 0; j < W; j++) {
				fprintf(file, "%c", board[i][j]);
			}
			fprintf(file, "\n");
		}
		fprintf(file, "+\n");
		fclose(file);
	}
}

// Function that clears the screen
void cScreen() {
	_cprintf("\033[H");
}


// Function to initialize the game board and the car positions
void iB(Car* cars, char (*board)[W], int level) {
	// Fills everything in the board
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			board[i][j] = '#';
		}
	}

	srand(time(0));
	// Initialize the cars with random positions, speeds, and directions
	for (int i = 0; i < NC; i++) {
		cars[i].X = rand() % (W - 2) + 1;
		if (level == 1) {
			cars[i].Speed = rand() % 5 + 1; // Slower speeds
		}
		else if (level == 2 || level == 3) {
			cars[i].Speed = rand() % 3 + 1; // Faster speeds
		}
		if (rand() % 2 == 0)
		{
			cars[i].Direction = -1;

		}
		else cars[i].Direction = 1;
	}
}


// Function to draw the game board with the frog, cars, stork
void dB(Frog* p, Car* c, Stork* s, char (*b)[W], int fC, int cC, int fnC, int sC, int l) {
	char tempBoard[H][W];
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			// Check if the current position matches the frog's position
			if (i == p[0].Y && j == p[0].X) {
				_cprintf("\033[%dm%c\033[0m", fC, 'o');
				tempBoard[i][j] = 'o';
			}
			// Check if the current position matches the stork's position
			else if (i == s[0].Y && j == s[0].X)
			{
				_cprintf("\033[%dm%c\033[0m", sC, '^');
				tempBoard[i][j] = '^';
			}
			// Handle the positions where cars can appear
			else if (i > 0 && i <= NC) {
				bool isC = false;
				for (int k = 0; k < NC; k++) {
					if (l == 1 && (i == 7 || i == 5 || i == 3)) continue; // Does not print these cars so that it is not so hard
					if (i == k + 1 && j == c[k].X) {
						// Ensure the car is not inside obstacles
						if (!((i == 3 && (j == 5 || j == 6 || j == 7)) || (i == 4 && (j == 18 || j == 19 || j == 20)))) {
							// Different colors for specific rows
							if (i == 1 || i == 3)  _cprintf("\033[33mX\033[0m");
							else if (i == 4 || i == 6) {
								if (p[0].moveActive) _cprintf("\033[96mX\033[0m");
								else _cprintf("X");
							}
							else _cprintf("\033[%dmX\033[0m", cC);
							tempBoard[i][j] = 'X';
							isC = true;
							break;
						}
					}
				}
				if (!isC) {
					// Print the right and left border
					if (j == 0 || j == W - 1) {
						_cprintf("|");
						tempBoard[i][j] = '|';
					}
					// Check for obstacle positions and print them with color
					else {
						if ((i == 3 && (j == 5 || j == 6 || j == 7)) || (i == 4 && (j == 18 || j == 19 || j == 20))) {
							_cprintf("\033[%dm#\033[0m", fnC);
							tempBoard[i][j] = '#';

						}
						else {
							_cprintf("_");
							tempBoard[i][j] = '_';
						}
					}
				}
			}
			// Print the top and bottom borders
			else {
				if (i == 0 || i == H - 1) {
					_cprintf("\033[%dm%c\033[0m", fnC, b[i][j]);

				}
				else _cprintf("%c", b[i][j]);

				tempBoard[i][j] = b[i][j];
			}
		}
		_cprintf("\n");
	}

	saveFrame(tempBoard);
}


// Checks the distance
int checkAbs(int x) {
	if (x < 0) return -x;
	else return x;
}


// Function to update the positions of cars
void uCr(int frame, Car* cars, Frog* player) {
	for (int i = 0; i < NC; i++) {
		// If the frog is close to the car, the car stops moving
		if (i == 0) {
			if ((player[0].Y == 1 || player[0].Y == 2) && checkAbs(player[0].X - cars[0].X) <= 2)
				continue;
		}
		else if (i == 2) {
			if ((player[0].Y == 3 || player[0].Y == 4) && checkAbs(player[0].X - cars[2].X) <= 2)
				continue;
		}
		// Check if moving the car is possible depending on the frame
		if (frame % cars[i].Speed == 0) {
			cars[i].X += cars[i].Direction;
			// If the car reaches the border
			if (cars[i].X >= W - 1) {
				if (i == 1 || i == 4) { // If the cars are specific then they appear randomly somewhere else
					do {
						cars[i].X = rand() % (W - 2) + 1;
					} while (cars[i].X == player[0].X); // So that there won't be a collision

					// Has a new direction 
					if (rand() % 2 == 0)
					{
						cars[i].Direction = -1;

					}
					else cars[i].Direction = 1;
				}
				else {
					cars[i].X = W - 2; // Ensure it doesn't go out of bounds
					cars[i].Direction = -1;
				}
			}
			// If the car reaches the border
			else if (cars[i].X <= 0) {
				if (i == 1 || i == 4) { // If the cars are specific then they appear randomly somewhere else
					do {
						cars[i].X = rand() % (W - 2) + 1;
					} while (cars[i].X == player[0].X); // So that there won't be a collision

					// Has a new direction 
					if (rand() % 2 == 0)
					{
						cars[i].Direction = -1;

					}
					else cars[i].Direction = 1;
				}
				// Updates the car if they are not specific, so that they can bounce
				else {
					cars[i].X = 1;
					cars[i].Direction = 1;
				}
			}
		}
		// Make random speed for some cars 
		if (i == 4 || i == 5) {
			cars[i].Speed = rand() % 3 + 1;
		}
	}
}



// Function to update the stork's position
void uSt(int frame, Stork* bird, Frog* player, int level) {
	if (level == 3) {
		// Randomly assign the stork's speed
		bird[0].Speed = rand() % 2 + 3;
		// Move the stork only on specific frames based on its speed, its goal is to catch the frog
		if (frame % bird[0].Speed == 0) {
			if (bird[0].X < player[0].X) {
				bird[0].X += 1; // Move right
			}
			else if (bird[0].X > player[0].X) {
				bird[0].X -= 1;// Move left
			}

			if (bird[0].Y < player[0].Y) {
				bird[0].Y += 1;// Move down
			}
			else if (bird[0].Y > player[0].Y) {
				bird[0].Y -= 1;// Move up
			}
		}
	}

}


// Function to update the frog's position
bool uPl(char input, Frog* player) {
	int tempX = 0;
	int tempY = 0;
	// Friendly car move player activate/deactivate
	if (input == 'e') {
		player[0].moveActive = !(player[0].moveActive);
		return false;
	}
	else if (input == 'a') {
		// Check if the frog is at the left border then move left
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
		// Check if the frog is at the right border then move right
		if (player[0].X == 28) {
			tempX = player[0].X;
			tempY = player[0].Y;
		}
		else {
			tempX = player[0].X + 1;
			tempY = player[0].Y;
		}
	}
	// Move the frog up
	else if (input == 'w' && player[0].Y > 0) {
		tempY = player[0].Y - 1;
		tempX = player[0].X;
	}
	// Move the frog down
	else if (input == 's') {
		if (player[0].Y == H - 1) {
			tempY = player[0].Y;
			tempX = player[0].X;
		}
		else {
			tempY = player[0].Y + 1;
			tempX = player[0].X;
		}
	}
	else {
		return false;
	}
	// Prevent the frog from moving into obstacles
	if ((tempY == 3 && (tempX == 5 || tempX == 6 || tempX == 7)) || (tempY == 4 && (tempX == 18 || tempX == 19 || tempX == 20))) { return false; }
	// Makes new place where the forg now is
	else {
		player[0].X = tempX;
		player[0].Y = tempY;
		return true;
	}


}


// Function to check for collisions between the frog and cars
bool chC(Frog* player, Car* cars, int l) {
	for (int i = 0; i < NC; i++) {
		// Check the cars on the first level
		if (l == 1 && (i == 7 || i == 5 || i == 3)) return false;
		// Check when frog meets the friendly car
		if (cars[3].X == player[0].X && player[0].Y == 4 || (cars[5].X == player[0].X && player[0].Y == 6)) {
			if (!player[0].moveActive) return false;
			int newX = 0;
			// Generates new coordinates so that they are not on obstacles
			do {
				newX = rand() % 30;
				if (newX > 2 && newX < 13) break;
				if (newX > 24 && newX < 28) break;
			} while (true);
			// So that there is no collision
			if (cars[3].Direction == 1) {
				cars[3].X = newX + 1; // Move the car right
			}
			else {
				cars[3].X = newX - 1; // Move the car left
			}
			if (cars[5].Direction == 1) {
				cars[5].X = newX + 1; // Move the car right
			}
			else {
				cars[5].X = newX - 1; // Move the car left
			}
			player[0].X = newX; //Move the frog where the car was initially
			return false;
		}
		// General collision check for all cars
		else {
			if (player[0].Y == i + 1 && player[0].X == cars[i].X) {
				return true; //There is a collision
			}
		}
	}

	return false; //There is no collision
}


// Check if the stork ate the frog
bool chS(Frog* player, Stork* bird)
{	// If they meet
	if (player[0].X == bird[0].X && player[0].Y == bird[0].Y) return true; // The stork ate the frog
	return false; // The stork did not eat the frog
}


// Check if the frog reached the finish line
bool chW(Frog* player) {

	if (player[0].Y == 0) {
		return true; // If it is all the way up than yes
	}
	return false; //If it is not all the way up than no
}


// Shows status
void status(int sL, int l) {
	_cprintf("\n Elvira Bizuk 203540 | czas: %d s. | lvl %d\n", sL, l);
}


// Shows score message
void showScore(int ps) {
	int h = gtC();// Gets the current score from the file
	// If it is the best score then that's the new record
	if (ps > h) {
		_cprintf("Sukces wynik to: %d, nowy rekord!\n", ps);
		svS(ps);// Saves the new high score to file
	}
	else _cprintf("Sukces wynik to: %d, rekord: %d\n", ps, h);
}


// Makes the replay file empty
void clearFrame() {
	FILE* file = fopen("replay.txt", "w");
	fclose(file);
}


// Printing the game replay
void showReplay() {
	// Open the file
	cScreen();
	FILE* file = fopen("replay.txt", "r");
	// Check if the file was opened successfully
	if (file != NULL) {
		char line[256];
		while (fgets(line, sizeof(line), file)) { // Gets every line
			if (line[0] == '+') {
				cScreen(); // Clears the screen
				struct timespec ts;
				ts.tv_sec = 0;
				ts.tv_nsec = 100000000;

				thrd_sleep(&ts, NULL); // Pauses the recording
			}
			else {
				_cprintf(line);
			}
		}
		fclose(file);
		_cprintf("\033[2J\033[H");
		_cprintf("Koniec nagrania.");
	}
}

int main() {
	// Initialize the positions and other parameters
	Frog plr;
	plr.moveActive = false;
	plr.X = W / 2;
	plr.Y = H - 1;
	Car cr[7];
	Stork br;
	br.X = 1;
	br.Y = 0;
	char b[H][W];
	int frm = 0;
	int tr = 0;
	int dlyT = 0;
	// Getting the parameters from the file
	int sL = gCP("cfg.txt", "TIME");
	int fC = gCP("cfg.txt", "FROG");
	int cC = gCP("cfg.txt", "CAR");
	int fnC = gCP("cfg.txt", "FINISH");
	int sC = gCP("cfg.txt", "STORK");
	// Choosing the level of the game
	int l;
	_cprintf("wybierz poziom: 1-3\n(0 - ostatnia gra)\n");
	_cscanf("%d", &l);
	// If you want to see the recording
	if (l == 0) {
		showReplay(); // Shows the recording
		return 0;
	}
	else if (l < 1 || l>3) return 0; // Checks if there is level for given number

	clearFrame(); // Makes the file empty

	iB(cr, b, l); // Initializes the borad the speeds 

	while (true) {

		cScreen();
		dB(&plr, cr, &br, b, fC, cC, fnC, sC, l);// Drawing the board with everything
		// If there is a collision 
		if (chC(&plr, cr, l)) {
			_cprintf("Kolizja\n");
			break;
		}
		// If stork ate the frog
		if (chS(&plr, &br)) {
			_cprintf("Zlapal cie\n");
			break;
		}
		// If the frog won 
		if (chW(&plr)) {
			int ps = sL * 10 * l; // Calculates the score
			showScore(ps);
			break;
		}

		// Checks if you press something on the keyboard
		if (_kbhit() && dlyT <= 0) {
			char input = _getch();// Indicates what you pressed
			bool isU = uPl(input, &plr); // Delays the frog
			if (isU) { dlyT = 2; }
		}

		uSt(frm, &br, &plr, l); //Moves the stork

		uCr(frm, cr, &plr);// Moves the car
		// Checks the time and if it ended then says it
		if (sL == 0) {
			_cprintf("Koniec czasu.\n");
			return 0;
		}

		frm++;
		tr++;
		struct timespec ts;
		ts.tv_sec = 0;
		ts.tv_nsec = 100000000;

		thrd_sleep(&ts, NULL); // Pauses the game
		dlyT--;
		// If one second has passed, timer decreases and starts counting form the beginning until it is again one second
		if (tr == 10) {
			tr = 0;
			sL--;
		}

		status(sL, l);
	}
	// So that it won't close if you press something after winning
	int n;
	_cprintf("\n");
	_cscanf("%d", &n);

	return 0;
}
