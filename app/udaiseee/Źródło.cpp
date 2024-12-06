#include <iostream>
#include <conio.h>
#include <windows.h>
#include <cstdlib>
#include <ctime>

using namespace std;


const int WIDTH = 30;
const int HEIGHT = 8; 


char board[HEIGHT][WIDTH];


int playerX = 1;
int playerY = HEIGHT - 1;


int carX[5]; 
int carSpeed[5]; 
int carDirection[5]; 


void clearScreen() {
	COORD cursorPosition = { 0, 0 };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);
}


void initializeBoard() {
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			board[i][j] = ' ';
		}
	}

	
	for (int i = 0; i < HEIGHT; i++) {
		board[i][0] = '|';
		board[i][WIDTH - 1] = '|';
	}

	
	srand(time(0));
	for (int i = 0; i < 5; i++) {
		carX[i] = rand() % (WIDTH - 2) + 1;
		carSpeed[i] = rand() % 3 + 2; 
		carDirection[i] = rand() % 2 == 0 ? -1 : 1; 
	}
}


void drawBoard() {
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			if (i == playerY && j == playerX) {
				cout << 'O'; 
			}
			else if (i > 0 && i <= 5) { 
				bool isCar = false;
				for (int k = 0; k < 5; k++) { 
					if (i == k + 1 && j == carX[k]) {
						cout << 'X'; 
						isCar = true;
						break;
					}
				}
				if (!isCar) cout << ' ';
			}
			else {
				cout << board[i][j]; 
			}
		}
		cout << endl;
	}
}

void updateCars(int frame) {
	for (int i = 0; i < 5; i++) {
		if (frame % carSpeed[i] == 0) { 
			carX[i] += carDirection[i];
			if (carX[i] >= WIDTH - 1) { 
				carX[i] = 1; 
			}
			else if (carX[i] <= 0) { 
				carX[i] = WIDTH - 2; 
			}
		}
	}
}

void updatePlayer(char input) {
	if (input == 'a' && playerX > 1) { 
		playerX--;
	}
	else if (input == 'd' && playerX < WIDTH - 2) { 
		playerX++;
	}
	else if (input == 'w' && playerY > 0) { 
		playerY--;
	}
	else if (input == 's' && playerY < HEIGHT - 1) { 
		playerY++;
	}
}


bool checkCollision() {
	for (int i = 0; i < 5; i++) {
		if (playerY == i + 1 && playerX == carX[i]) {
			return true;
		}
	}
	return false;
}


bool checkWin() {
	return playerY == 0;
}


int main() {
	initializeBoard();
	int frame = 0; 

	while (true) {
		clearScreen();
		drawBoard();

		if (checkCollision()) {
			cout << "Przegrales! Zderzyles sie z samochodem." << endl;
			break;
		}

		if (checkWin()) {
			cout << "Gratulacje! Dotarles do mety!" << endl;
			break;
		}

		if (_kbhit()) {
			char input = _getch();
			updatePlayer(input);
		}

		updateCars(frame);

		frame++;
		Sleep(100); 
	}

	return 0;
}
