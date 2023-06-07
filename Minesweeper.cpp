// Minesweeper.cpp by R.D.S.

#include <array>
#include <algorithm>    // std::find_if
#include <iostream>
#include <vector>

struct Coordinate {
	int rowIndex = 0;
	int colIndex = 0;
};

struct Cell {
	bool containsMine = false;
	bool isMarkedWithFlag = false;
	bool isMarkedWithQuestion = false;
	bool isReserved = false;
	bool isRevealed = false;
	Coordinate coord;
	int numNeighbouringMines = 0;
};

struct GameBoard {
	const int minHeight = 8;
	const int maxHeight = 50;
	const int minWidth = 8;
	const int maxWidth = 50;

	bool isPlayerAlive = true;
	int gridHeight = 0;
	int gridWidth = 0;
	int numMines = 0;
	int remainingCleanCells = 0;
	std::vector<std::vector<Cell>> grid;
	std::vector<Coordinate> revealedCells;
};


bool isCoordValid (const GameBoard& gameBoard, const Coordinate& coord);
char getLowercase (const char ch);
char getUppercase (const char ch);
int countNeighbouringMines (const GameBoard& gameBoard, const Coordinate& targetCoord);
void chord (GameBoard& gameBoard, Cell& targetCell);
void createBoard (GameBoard& gameBoard, const int kNumRows, const int kNumCols);
void displayGrid(const GameBoard& gameBoard);
void printCell (const GameBoard& gameBoard, const Cell& targetCell);
void printTableHorizontalBorder (const GameBoard& gameBoard);
void setMine(GameBoard& gameBoard, const Coordinate& targetCoord);
void revealCell(GameBoard& gameBoard, const Coordinate& targetCoord, const bool isInitialCall = true);


int main() {
	GameBoard gameBoard;

	createBoard(gameBoard, gameBoard.maxHeight, gameBoard.maxWidth);

	setMine(gameBoard, {0, 0});
	setMine(gameBoard, {0, 7});
	setMine(gameBoard, {0, 9});
	setMine(gameBoard, {0, 10});
	setMine(gameBoard, {2, 0});
	setMine(gameBoard, {3, 3});
	setMine(gameBoard, {3, 7});
	setMine(gameBoard, {4, 9});
	setMine(gameBoard, {5, 11});
	setMine(gameBoard, {6, 1});
	setMine(gameBoard, {6, 2});
	setMine(gameBoard, {6, 3});
	setMine(gameBoard, {7, 8});
	setMine(gameBoard, {7, 10});

	revealCell(gameBoard, {5, 7}, true);
	displayGrid(gameBoard);
	revealCell(gameBoard, {3, 0}, true);
	displayGrid(gameBoard);

	std::cout << "\n\n\n";
	return 0;
}

// Returns true if the target cell has a valid index within the game board.
bool isCoordValid (const GameBoard& gameBoard, const Coordinate& coord) {
	return (coord.rowIndex >= 0) && (coord.rowIndex < gameBoard.gridHeight) && (coord.colIndex >= 0) && (coord.colIndex < gameBoard.gridWidth);
}

// Returns a lowercase equivalent of an uppercase char if passed, otherwise returns the char unchanged.
char getLowercase (const char ch) {
	return (ch >= 'A' && ch <= 'Z') ? ('z' - ('Z' - ch)) : ch;
}

// Returns an uppercase equivalent of a lowercase char if passed, otherwise returns the char unchanged.
char getUppercase (const char ch) {
	return (ch >= 'a' && ch <= 'z') ? ('Z' - ('z' - ch)) : ch;
}

// If the target cell itself is located at invalid coordinates, function will return -1, otherwise returns the number of valid neighbouring cells that contain mines.
int countNeighbouringMines (const GameBoard& gameBoard, const Coordinate& targetCoord) {
	if (isCoordValid(gameBoard, targetCoord) == false)
		return -1;

	int numNeighbouringMines = 0;

	// Scan the 8 neighbouring cells for mines
	for (int i = targetCoord.rowIndex - 1; i <= targetCoord.rowIndex + 1; i++) {
		for (int j = targetCoord.colIndex - 1; j <= targetCoord.colIndex + 1; j++) {
			if (i == j)
				continue;  // Ignore target cell (can't be a neighbour of itself)

			if (isCoordValid(gameBoard, gameBoard.grid[i][j].coord) && gameBoard.grid[i][j].containsMine)
				numNeighbouringMines++;
		}
	}

	return numNeighbouringMines;
}

// [INCOMPLETE] Reveals all neighbouring cells which don't contain mines IF the target cell has the correct amount of flags around it.
void chord (GameBoard& gameBoard, Cell& targetCell){}

// Creates/resets a new/existing game board of/to a specified size.  (Does not contain any mines.)
void createBoard (GameBoard& gameBoard, const int kNumRows, const int kNumCols) {
	gameBoard.grid.clear();
	gameBoard.grid.resize(kNumRows, std::vector<Cell> (kNumCols));
	gameBoard.gridHeight = kNumRows;
	gameBoard.gridWidth = kNumCols;
	gameBoard.numMines = 0;

	Cell blankCell;
	for (int currRow = 0; currRow < gameBoard.gridHeight; currRow++) {
		for (int currCol = 0; currCol < gameBoard.gridWidth; currCol++) {
			blankCell.coord = {currRow, currCol};
			gameBoard.grid[currRow][currCol] = blankCell;
		}
	}
}

// Prints the grid of the game board to the console.
void displayGrid(const GameBoard& gameBoard) {
	// Topmost horizontal table border
	std::cout << "\n     |";
	for (int currCol = 0; currCol < gameBoard.gridWidth; currCol++) {
		if (currCol < 10)
			std::cout << " " << currCol << " ";
		else if (currCol < 100)
			std::cout << " " << currCol;
		else
			std::cout << currCol;
		std::cout << "|";
	}
	printTableHorizontalBorder(gameBoard);

	// Table contents
	for (int currRow = 0; currRow < gameBoard.gridHeight; currRow++) {
		if (currRow < 10)
			std::cout << "  " << currRow << "  ";
		else if (currRow < 100)
			std::cout << " " << currRow << "  ";
		else
			std::cout << currRow << "  ";

		std::cout << "|";
		for (int currCol = 0; currCol < gameBoard.gridWidth; currCol++) {
			const Cell& currCell = gameBoard.grid[currRow][currCol];

			std::cout << " ";
			printCell(gameBoard, currCell);
			std::cout << " |";
		}
		printTableHorizontalBorder(gameBoard);
	}
}

// Cell is hidden: Prints 'H' if unmarked/unflagged, 'F' if the cell is flagged, '?' if marked with a question.
// Cell is revealed: Prints 'M' if the cell contains a mine, the number of neighbouring mines if at least 1 is present, and ' ' if there are no neighbouring mines.
void printCell (const GameBoard& gameBoard, const Cell& targetCell) {
	if (targetCell.isRevealed == false) {
		if (targetCell.isMarkedWithFlag)
			std::cout << "F";
		else if (targetCell.isMarkedWithQuestion)
			std::cout << "?";
		else
			std::cout << "H";

	}
	else {
		if (targetCell.containsMine)
			std::cout << "M";

		else if (targetCell.numNeighbouringMines != 0)
			std::cout << targetCell.numNeighbouringMines;

		else std::cout << " ";
	}
}

// Prints the horizontal border for the table/grid drawn in displayGrid().
void printTableHorizontalBorder (const GameBoard& gameBoard) {
	std::cout << "\n+----+";
	for (int currCol = 0; currCol < gameBoard.gridWidth; currCol++)
		std::cout << "---+";
	std::cout << "\n";
}

// Reveals the targeted cell.  If a mine is found during the initial call (i.e., the cell selected by the player) the game ends.
// Otherwise, if possible, a cascade reveal is initiated, recursively checking for and revealing all neighbouring cells that are also clear of mines.
// Mines found during these calls only end that branch of the recursion, NOT the game itself.
void revealCell(GameBoard& gameBoard, const Coordinate& targetCoord, const bool isInitialCall) {
	if (isInitialCall)
		gameBoard.revealedCells.clear();

	if (gameBoard.revealedCells.size() >= (gameBoard.gridHeight * gameBoard.gridWidth) - gameBoard.numMines)
		return;

	if (isCoordValid(gameBoard, targetCoord) == false)    // Don't waste time with invalid cells
		return;

	if (gameBoard.grid[targetCoord.rowIndex][targetCoord.colIndex].isRevealed)    // Don't waste time with already revealed cells
		return;

	auto it = std::find_if(gameBoard.revealedCells.begin(), gameBoard.revealedCells.end(), [targetCoord] (const Coordinate& searchingCoords) {
		return (searchingCoords.rowIndex == targetCoord.rowIndex && searchingCoords.colIndex == targetCoord.colIndex);
	});
	if (it != gameBoard.revealedCells.end())    // Don't waste time checking cells that have already been checked
		return;

	if (gameBoard.grid[targetCoord.rowIndex][targetCoord.colIndex].containsMine) {
		if (isInitialCall)
			gameBoard.isPlayerAlive = false;
		return;
	}

	gameBoard.grid[targetCoord.rowIndex][targetCoord.colIndex].isRevealed = true;
	gameBoard.revealedCells.push_back(gameBoard.grid[targetCoord.rowIndex][targetCoord.colIndex].coord);

	if (gameBoard.grid[targetCoord.rowIndex][targetCoord.colIndex].numNeighbouringMines != 0)
		return;

	const int numNeighbours = 4;
	const std::array<Coordinate, numNeighbours> neighbourCoords = {{
		{targetCoord.rowIndex - 1, targetCoord.colIndex},    // N
		{targetCoord.rowIndex, targetCoord.colIndex + 1},    // E
		{targetCoord.rowIndex + 1, targetCoord.colIndex},    // S
		{targetCoord.rowIndex, targetCoord.colIndex - 1},    // W
	}};
	for (int i = 0; i < numNeighbours; i++) {
		if (gameBoard.revealedCells.size() >= (gameBoard.gridHeight * gameBoard.gridWidth) - gameBoard.numMines)
			break;
		revealCell(gameBoard, neighbourCoords[i], false);
	}
}

// Places a mine at the specified index on the game board (provided that the index is valid)
void setMine(GameBoard& gameBoard, const Coordinate& targetCoord) {
	if (isCoordValid(gameBoard, targetCoord) == false)    // Don't waste time with invalid cells
		return;
	gameBoard.grid[targetCoord.rowIndex][targetCoord.colIndex].containsMine = true;

	for (int i = targetCoord.rowIndex - 1; i <= targetCoord.rowIndex + 1; i++) {
		for (int j = targetCoord.colIndex - 1; j <= targetCoord.colIndex + 1; j++) {
			Coordinate currNeighbour = {i, j};

			if (isCoordValid(gameBoard, {i, j}) && !(i == targetCoord.rowIndex && j == targetCoord.colIndex))
				gameBoard.grid[i][j].numNeighbouringMines++;
		}
	}
}









