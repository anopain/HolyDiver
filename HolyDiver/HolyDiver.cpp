
/*
*
* Holy diver - an epic adventure at object-oriented world way beneath the surface!
* Template code for implementing the rich features to be specified later on.
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <random>

using namespace std;

/****************************************************/
// declaring functions:
/****************************************************/
void start_splash_screen(void);
void startup_routines(void);
void quit_routines(void);
char** load_level(int& rows, int& cols); // a routine to load a level map from a file
//void freeLevel(int rows);
//void printLevel(int rows, int cols);
int read_input(char*);
void update_state(char);  // assuming only one input char (key press) at most at a time ("turn-based" execution flow)
void render_screen(void);
void Information(void);
void Restart(void);
void destroyObjects(void);	//Free all objects
void GameOver(void);	//Game over status

class World;
class Character;
class Player;
class EnemyStat;
class EnemyMov;


/****************************************************/
// global variables:
/****************************************************/
//char** map;// pointer pointer equals to array of arrays = 2-dimensional array of chars
// above is virtually identical, as a variable, compared to for example:
//	    char map[MAXSTR][MAXLEN] = {{0}}; // declare a static 2-dim array of chars, initialize to zero
// However pointer to pointer has not allocated memory yet attached to it, this is done dynamically when actual size known

int map_rows = 0;
int map_cols = 0;

int gameOver = 0;

const int MAX_HEALTH = 100;
const int MAX_OXYGEN = 100;
const int MAX_BATTERY = 100;
const int INIT_LIVES = 3;

/****************************************************************
 *
 * CLASS World
 *
 * Class for world actions
 *
 * **************************************************************/

class World {										//Class for world actions
protected:
private:
	int map_rows = 0;
	int map_cols = 0;
	vector<Character*> characters;					//Store characters in vector

public:
	char** map;										//Pointer to map
	bool** discoveredMask;							//Pointer to discovered mask

	void addCharacter(Character* character);			//Add character to vector
	Character* getCharacter(int x, int y);			//Get character from vector
	World();
	void printWorld();
	void loadWorld();
	vector<int> randEnemy(char chr);					//Get player position as vector
	vector<int> getPosOnMap(char chr);				//Get player position as vector
	bool canMove(vector<int> pos);						//Check if player can move to new position
	vector<int> affectDamage(vector<int> pos);		//Check what is in position, affect damage to who is at position
	void freeLevel(int rows);						//*************Fix this to use map_rows !!!!!!******************??????	
	void printLevel(int rows, int cols);
	bool** getMask();
	void expandMask(int x, int y);
	~World();
};



World* world = nullptr; // Global pointer definition

/****************************************************************
 *
 * CLASS Character
 *
 * Base class for player and enemy classes
 *
 * **************************************************************/
Player* player1 = nullptr; //Character object as nullptr

class Character {										//Base class for players and enemies
protected:
private:
public:
	int health;
	int oxygen;
	int lives;
	int X;
	int Y;

	virtual ~Character() {}

	Character(int x, int y)
	{
		health = MAX_HEALTH;
		oxygen = MAX_OXYGEN;
		lives = INIT_LIVES;
	}
	virtual void move(int x = 0, int y = 0)			//Move
	{
		//Player can and will move
		world->map[Y][X] = 'o';						//Update old position to free pos
		X = x;										//Set new X position
		Y = y;										//Set new Y position
		world->map[Y][X] = 'P';						//Set net player position
		world->expandMask(0, 0);					//Expand discovered mask
	}
	void print_info()
	{
		cout << "Player info: " << endl;
		cout << "Health: " << health << endl;
		cout << "Oxygen: " << oxygen << endl;
		cout << "Lives: " << lives << endl;
		//		cout << "Position: [" << X << "," << Y << "]" << endl;		//For debugging purposes
	}
	void health_decrease(int amount)
	{
		health -= amount;
		if (health < 0)
		{
			health = 0;
			delete this;							//Free player object
		}
	}
	void health_increase(int amount)
	{
		health += amount;
		if (health > MAX_HEALTH)
		{
			health = MAX_HEALTH;
		}
	}
	void lives_decrease(int amount)
	{
		lives -= amount;
		if (lives <= 0)
		{
			lives = 0;
			delete this;
		}
	}
	vector<int> getPosition()						//Get player position as vector
	{
		vector<int> pos;
		pos.push_back(X);
		pos.push_back(Y);
		return pos;
	}
};

class Player : public Character {					//HumanPlayer, class derived from Character
protected:
private:
	int battery;
public:
	Player(int x, int y) : Character(x, y)
	{
		battery = MAX_BATTERY;
		vector<int> tmp = world->getPosOnMap('P');	//Get player position from map		!!!TO-DO!!! This needs to be on player class
		if (tmp.size() > 0)
		{
			X = tmp[0];
			Y = tmp[1];
		}
		else
		{
			X = 0;
			Y = 0;
		}

		world->addCharacter(this);					//Add player to world
	}
	void print_info()
	{
		cout << "Player info: " << endl;
		cout << "Health: " << health << endl;
		cout << "Oxygen: " << oxygen << endl;
		cout << "Battery: " << battery << endl;
		cout << "Lives: " << lives << endl;
		cout << "Position: [" << X << "," << Y << "]" << endl;	//For debugging purposes
	}
	void flashlight(int x, int y)
	{
		if (battery > 0)
		{
			battery_decrease(5);
			cout << "Flashlight used!" << endl;
			world->expandMask(x, y);							//Expand discovered mask              //********PROBLEM HERE!!!!******
		}
		else
		{
			cout << "Flashlight is out of battery!" << endl;
		}
	}
	void battery_increase(int amount)
	{
		battery += amount;
		if (battery > MAX_BATTERY)
		{
			battery = MAX_BATTERY;
		}
	}
	int battery_decrease(int amount)
	{
		battery -= amount;
		if (battery <= 0)
		{
			battery = 0;
			cout << "Out of battery!" << endl;
			return -1;
		}
		return 1;
	}
	void oxygen_decrease(int amount)
	{
		oxygen -= amount;
		if (oxygen <= 0)
		{
			oxygen = 0;
			gameOver = -3;
		}
	}
	void oxygen_increase(int amount)
	{
		oxygen += amount;
		if (oxygen > MAX_OXYGEN)
		{
			oxygen = MAX_OXYGEN;
		}
	}
	void takeDamage(int amount)
	{
		health_decrease(amount);
		cout << "You took damage!" << endl;
	}
	~Player()
	{
		cout << "Player is DEAD now! RIP!!!" << endl;
		gameOver = -3;										//Game over status
	}
};

EnemyStat* enemyStat = nullptr;								//Enemy object as nullptr

class EnemyStat : public Character {							//Marked as 'm'		Static enemy class derived from Player
protected:

private:
public:
	EnemyStat(int x, int y) : Character(x, y)
	{
		vector<int> ePos = world->randEnemy('m');			//Randomize enemy position
		if (ePos[0] == -1 || ePos[1] == -1)
		{
			cout << "Enemy location returned negative value!" << endl;
		}
		else
		{
			X = ePos[0];
			Y = ePos[1];
		}
		world->addCharacter(this);							//Add player to world
	}

	void giveDamage()
	{
		health_decrease(10);
		cout << "Enemy attacked you!" << endl;
	}
	void takeDamage(int amount)
	{
		health_decrease(amount);
		cout << "Enemy took damage!" << endl;
	}
	~EnemyStat()
	{
		cout << "Static enemy killed!!!" << endl;
	}
};

EnemyMov* enemyMov = nullptr;								//Enemy object as nullptr

class EnemyMov : public Character {							//Marked as 'M'		//Moving enemy class derived from Player
protected:

private:
	bool active = false;									//Enemy status
public:
	EnemyMov(int x, int y) : Character(x, y)
	{
		vector<int> ePos = world->randEnemy('M');			//Randomize enemy position
		if (ePos[0] == -1 || ePos[1] == -1)
		{
			cout << "Enemy location returned negative value!" << endl;
		}
		else
		{
			X = ePos[0];
			Y = ePos[1];
		}
		world->addCharacter(this);							//Add player to world
	}
	void moveEnemy(vector<int> pos)							//Move enemy
	{
		if (world->discoveredMask[pos[1]][pos[0]] == true)	//Activate enemy if discovered
			active = true;

		if (!active) return;								//If enemy is not active, do nothing

		vector<int> newLocat = rndEnemyMove();				//Randomize enemy movement

		world->map[Y][X] = 'o';								//Set old position to free space
		X = pos[0];											//Set new X position
		Y = pos[1];											//Set new Y position
		world->map[Y][X] = 'M';								//Set new position to enemy
	}

	vector<int> rndEnemyMove()								//Move enemy (Randomized movement)
	{	//Randomize either -1 or 1
		random_device rd;									//Create random device generator
		mt19937 gen(rd());

		uniform_int_distribution<> distr(-1, 1);			//Set distribution either -1 or 1
		int rndVal = distr(gen);							//Randomize either -1 or 1
		if (rndVal == 0)
		{
			vector<int> pos;
			pos.push_back(X);
			pos.push_back(Y);
			return pos;										//If 0, set to 1
		}

		if (distr(gen) < 0)									//Randomize either X or Y movement
		{
			X += rndVal;									//Move X
		}
		else
		{
			Y += rndVal;									//Move Y
		}

		vector<int> pos;
		pos.push_back(X);
		pos.push_back(Y);
		return pos;

	}
	void giveDamage()
	{
		health_decrease(10);
		cout << "Enemy attacked you!" << endl;
	}
	void takeDamage(int amount)
	{
		health_decrease(amount);
		cout << "Enemy took damage!" << endl;
	}
	~EnemyMov()
	{
		cout << "Moving enemy killed!!!" << endl;
	}
};

/****************************************************************
*
* World METHOD DEFINITIONS
*
* ****************************************************************/

void World::addCharacter(Character* character)			//Add character to vector
{
	characters.push_back(character);
}

Character* World::getCharacter(int x, int y)			//Get character from vector
{
	for (auto c : characters)
	{
		if (c->X == x && c->Y == y)				//Check if character is at position
		{
			return c;							//Return character
		}
	}
	return nullptr;								//Return nullptr if not found
}

World::World()
{
	loadWorld();
	discoveredMask = getMask();
}
void World::printWorld()
{
	cout << "World info: " << endl;
	cout << "Map size: [" << map_rows << "," << map_cols << "]" << endl;
}
void World::loadWorld()
{
	map = load_level(map_rows, map_cols);
}
vector<int> World::randEnemy(char chr)					//Get player position as vector
{
	random_device erd;							//Create enemy random device generator
	mt19937 gen(erd());
	uniform_int_distribution<int> distX(0, map_cols - 1);	//Set distribution for X position
	uniform_int_distribution<int> distY(0, map_rows - 1);	//Set distribution for Y position

	int c = 0;
	while (c < 50)								//Try 50 times find empty position
	{
		int x = distX(gen);						//Randomize X position
		int y = distY(gen);						//Randomize Y position
		if (map[y][x] == 'o')					//If empty space found
		{
			map[y][x] = chr;					//Set enemy to the map
			vector<int> pos;
			pos.push_back(x);
			pos.push_back(y);
			return pos;
		}
		c++;
	}
	vector<int> zeroVector = { -1, -1 };		//Return -1,-1 if not found	
	return zeroVector;
}

vector<int> World::getPosOnMap(char chr)				//Get player position as vector
{
	for (int i = 0; i < map_rows; ++i)
	{
		for (int j = 0; j < map_cols; ++j)
		{
			if (map[i][j] == chr)				//Find player position
			{
				vector<int> pos;
				pos.push_back(j);
				pos.push_back(i);
				return pos;
			}
		}
	}
	vector<int> zeroVector = { -1, -1 };			//Return -1,-1 if not found	
	return zeroVector;
}

bool World::canMove(vector<int> pos)						//Check if player can move to new position
{
	if (pos[0] < 0 || pos[0] >= map_cols || pos[1] < 0 || pos[1] >= map_rows) return false;	//Check if within map boundaries
	if (map[pos[1]][pos[0]] == 'x') return false;	//Check if wall element
	if (map[pos[1]][pos[0]] == 'm')					//Check if enemy 'm' element
	{
		discoveredMask[pos[1]][pos[0]] = true;		//Set discovered mask to true for enemy position
		return false;
	}
	if (map[pos[1]][pos[0]] == 'M')					//Check if enemy 'M' element
	{
		discoveredMask[pos[1]][pos[0]] = true;		//Set discovered mask to true for enemy position
		return false;
	}

	if (map[pos[1]][pos[0]] == 'P') return false;	//Check if player element

	return true;
}

vector<int> World::affectDamage(vector<int> pos)		//Check what is in position, affect damage to who is at position
{
	Character* target = getCharacter(pos[0], pos[1]);	//Get character at position
	if (target != nullptr)
	{
		target->health_decrease(25);			//Give damage to character
		return pos;								//Return position
	}
	return { -1, -1 };							//Return -1,-1 if no one found from coordinates
	/*
			vector<int> damageTo;
			if (map[pos[1]][pos[0]] == 'm')				//If enemy 'm' found
			{
				enemyStat->giveDamage();				//Give damage to enemy		***Not working for some reason***

				damageTo = pos;
				return damageTo;
			}
			else if (map[pos[1]][pos[0]] == 'M')		//If enemy 'M' found
			{
	//			enemyMov->giveDamage();					//Give damage to enemy			***Not working for some reason***
				damageTo = pos;
				return damageTo;
			}
			else if (map[pos[1]][pos[0]] == 'P')		//If player found
			{
	//			player->takeDamage(10);					//Give damage to player			***Not working for some reason***
				damageTo = pos;
				return damageTo;
			}
	*/
}

void World::freeLevel(int rows)						//*************Fix this to use map_rows !!!!!!******************??????	
{
	if (!map) return;
	for (int i = 0; i < rows; ++i)
	{
		delete[] map[i];
	}
	delete[] map;

	if (!discoveredMask) return;
	for (int i = 0; i < rows; ++i)
	{
		delete[] discoveredMask[i];
	}
	delete[] discoveredMask;
}

void World::printLevel(int rows, int cols)
{
	if (!map || !discoveredMask) return;
	//		for (int i = 0; i < rows; ++i)
	for (int i = 0; i < map_rows; ++i)			//Print map
	{
		for (int j = 0; j < map_cols; ++j)
		{
			if (discoveredMask[i][j])
			{
				cout << map[i][j];				//Print discovered elements
			}
			else
			{
				cout << ' ';					//Print undiscovered elements
			}
		}
		cout << endl;
	}

}
bool** World::getMask()
{
	discoveredMask = new bool* [map_rows];
	for (int i = 0; i < map_rows; ++i)								//Initialize shadowMap
	{
		discoveredMask[i] = new bool[map_cols];
		for (int j = 0; j < map_cols; ++j)
		{
			//				discoveredMask[i][j] = false;							//Initialize all values to false
			discoveredMask[i][j] = true;				//***FOR DEBUGGING->MAP VISIBLE***//
		}
	}

	for (int i = 0; i < map_rows; ++i)								//Initialize discoveredMask
	{
		for (int j = 0; j < map_cols; ++j)
		{
			if (map[i][j] == 'x') discoveredMask[i][j] = true;		//Set wall elements to true
		}
	}
	vector<int> pPos = getPosOnMap('P');							//Get player position
	if (pPos[0] == -1 || pPos[1] == -1)
	{
		cout << "Player location returned negative value!" << endl;
		return nullptr;
	}
	else
	{
		discoveredMask[pPos[1]][pPos[0]] = true;					//Set player position to true
	}

	return discoveredMask;
}
void World::expandMask(int x = 0, int y = 0)
{
	vector<int> pPos = getPosOnMap('P');									//Get player position
	x += pPos[0];															//Add player position to x if flashlight is used
	y += pPos[1];															//Add player position to y if flashlight is used

	if (x < 0 || x >= map_cols || y < 0 || y >= map_rows) return;			//Check if within map boundaries

	discoveredMask[y][x] = true;											//Light next char
}
World::~World()
{
	if (map) freeLevel(map_rows);									//Free map memory
	if (discoveredMask)												//Free discovered mask memory
	{
		for (int i = 0; i < map_rows; ++i)
		{
			delete[] discoveredMask[i];
		}
		delete[] discoveredMask;
	}
	cout << "World is destroyed!" << endl;
}

/****************************************************************
 *
 * MAIN
 * main function contains merely function calls to various routines and the main game loop
 *
 ****************************************************/
int main(void)
{
	start_splash_screen();
	startup_routines(); // do any necessary startup routines, such as loading level map
	char input;
	int rows = 0, cols = 0;

	// IMPORTANT NOTE: do not exit program without cleanup: freeing allocated dynamic memory etc
	while (true) // infinite loop, should end with "break" in case of game over or user quitting etc.
	{
		input = '\0'; // make sure input resetted each cycle
		if (0 > read_input(&input)) break; // exit loop in case input reader returns negative (e.g. user selected "quit")
		update_state(input);
		render_screen();

	}
	if (input == -3)
	{
		GameOver();
	}
	quit_routines(); // cleanup, bye-bye messages, game save and whatnot
	return 0;
}

/****************************************************************
 *
 * FUNCTION load_level
 *
 * Open a map file and load level map from it.
 * First weekly home assignment is to be implemented mostly here.
 *
 * **************************************************************/
char** load_level(int& rows, int& cols)
{
	string filepath = "level_0.map";						//Default map location
	cout << "Loading map from file: " << filepath << endl;
	cout << "Use default file location (" << filepath << ")? (y/n): ";

	char choice;
	cin >> choice;
	cin.ignore();											//Clear input buffer

	if (choice == 'n' || choice == 'N')
	{
		cout << "Enter file path: ";
		getline(cin, filepath);
	}

	//Open file on location filepath
	ifstream file(filepath);
	if (!file)												//If file not found, error message
	{
		cerr << "Error: Canno't open file: " << filepath << endl;
		return nullptr;
	}

	//Read file to temporary tempMap vector
	vector<string> tempMap;
	string line;
	while (getline(file, line))
	{
		tempMap.push_back(line);
	}
	file.close();

	rows = tempMap.size();
	cols = (rows > 0) ? tempMap[0].size() : 0;

	//Check that map size is correct
	if (rows == 0 || cols == 0)
	{
		cerr << "Error: Invalid map size." << endl;
		return nullptr;
	}

	//Allocate memory for the map
	char** map = new char* [rows];
	for (int i = 0; i < rows; ++i)
	{
		map[i] = new char[cols + 1];						//+1 for null terminator
		copy(tempMap[i].begin(), tempMap[i].end(), map[i]);
		map[i][cols] = '\0';								//Null terminate each row

		//Try to find player pos
		size_t pos = tempMap[i].find('P');
		if (pos != string::npos)
		{
			//			player->X = static_cast<int>(pos);
			//			player->Y = i;
		}
	}
	map_rows = rows;
	map_cols = cols;
	/*
		if (player->X < 0 || player->Y < 0)
		{
			cout << "Player not located in a map.";
		}
		else
		{
			cout << "Player location [" << player->X << "," << player->Y <<"]"<< endl;
		}
	*/
	//	printLevel(rows, cols);

	return map;

	// steps in short:
	// 1) locate, check and open file, if failure, return value indicating error (and check on the calling side)
	// 2) read first row, count number of characters. Assuming all maps are rectangular, use this information
	//    to memory allocation of global "map" (char ** map) pointer.
	//    Assuming first row contains N characters, then you need to allocate 2D table/array of dimensions N x N
	//    -> in practice first allocate to "map" an N-long array of (char *) pointers
	//        and then within a loop allocate an N-long array of chars to each of the previous entries
	//           -> as a result "map" is a pointer to pointer corresponding a 2D-array sized [N][N]
	//              and it's each "slot" can be referred to using syntax: map[x][y], each capable of storing a char.
	// 3) close file
	// 4) return with success value (e.g. zero when OK, negative if error)
	// [  5) outside this function, remember to free() allocated memory eventually ]


}

/****************************************************************
 *
 * FUNCTION read_input
 *
 * read input from user
 *
 * **************************************************************/
int read_input(char* input)
{
	cout << ">>>";	// simple prompt
	try {
		cin >> *input;
	}
	catch (...) {
		return -1; // failure
	}
	cout << endl;  //new line to reset output/input
	cin.ignore();  //clear cin to avoid messing up following inputs
	if (*input == 'q') return -2; // quitting game...
	if (gameOver == -3) return -3; //GameOver
	return 0; // default return, no errors
}

/****************************************************************
 *
 * FUNCTION update_state
 *
 * update game state (player, enemies, artefacts, inventories, health, whatnot)
 * this is a collective entry point to all updates - feel free to divide these many tasks into separate subroutines
 *
 * **************************************************************/
void update_state(char input)
{
	int newX = player1->X;
	int newY = player1->Y;
	int dy = 0;								//Variable for lightning function
	int dx = 0;								//Variable for lightning function

	int rows = 0, cols = 0;

	if (input == 'm' || input == 'M')
	{
		//		map = load_level(filepath, rows, cols);
		world->loadWorld();
	}

	//Player movement input
	switch (input)
	{
	case 'w': newY--;						//Player Controls Move
		break;
	case 'a': newX--;						//Player Controls Move
		break;
	case 's': newY++;						//Player Controls Move
		break;
	case 'd': newX++;						//Player Controls Move
		break;
	case 'h': Information();				//Print help information how to play
		break;
	case 'r':
		Restart();							//Restart game
		newX = player1->X;
		newY = player1->Y;
		break;
	case 'i': dy = -1;						//Lightning function
		break;
	case 'k': dy = 1;						//Lightning function
		break;
	case 'j': dx = -1;						//Lightning function
		break;
	case 'l': dx = 1;
		break;
	default:
		return;
	}

	if (newX != player1->X || newY != player1->Y)				//If player is trying to move
	{
		if (newX >= 0 && newX < map_cols && newY >= 0 && newY < map_rows)
		{
			bool go = world->canMove({ newX, newY });		//Check if player can move to new position
			if (go)
			{
				//Player can and will move
				player1->move(newX, newY);					//Move player
				/*
								world->map[player->Y][player->X] = 'o';		//Update old position to free pos
								player->X = newX;
								player->Y = newY;
								world->map[player->Y][player->X] = 'P';		//Set net player position
								world->expandMask(dx, dy);					//Expand discovered mask
				*/
			}
			else
			{
				world->affectDamage({ newX, newY });		//Check what is in position, affect damage to who is at position
				cout << "Inflicted damage!" << endl;
			}
			player1->oxygen_decrease(2);						//Oxygen decrease by 2 on moving
		}
		else
		{
			cout << "Movement not possible!" << endl;
		}
		//Player not able to go outside map boundaries 
	}
	if (dy != 0 || dx != 0)									//Player is using flashlight function
	{
		player1->flashlight(dx, dy);							//Player use flashlight
		//		world->expandMask(dx, dy);							//Expand discovered mask

		vector<int> enemyPos = world->getPosOnMap('M');		//Get enemy position
		enemyMov->moveEnemy(enemyPos);						//Move enemy (if active)

	}
}
void Restart()
{
	map_rows = 0;
	map_cols = 0;
	//Reload level
	world->loadWorld();
	//	map = load_level(filepath, map_rows, map_cols);
	player1->health = MAX_HEALTH;
	player1->oxygen = MAX_OXYGEN;
	player1->lives = INIT_LIVES;
}

void Information()
{
	cout << endl << "Holy Diver - an epic adventure at object-oriented world way beneath the surface!" << endl;
	cout << endl << "Movement:" << endl;
	cout << "w - up" << endl;
	cout << "a - left" << endl;
	cout << "s - down" << endl;
	cout << "d - right" << endl;
	cout << endl << "Flashlight:" << endl;
	cout << "i - light up" << endl;
	cout << "j - light left" << endl;
	cout << "k - light down" << endl;
	cout << "l - light right" << endl;
	cout << endl << "Controls:" << endl;
	cout << "h - information" << endl;
	cout << "m - load new map" << endl;
	cout << "r - restart game" << endl;
	cout << "q - quit game" << endl;
	cout << endl << "Press any key to continue..." << endl;
	cin.ignore();
}
/*
 * this function prints out all visuals of the game (typically after each time game state updated)
 *
 * **************************************************************/
void render_screen(void)
{
	player1->print_info();				//Print player info
	world->printLevel(map_rows, map_cols);
}

/****************************************************************
 *
 * FUNCTION start_splash_screen
 *
 * outputs any data or info at program start
 *
 * **************************************************************/
void start_splash_screen(void)
{
	/* this function to display any title information at startup, may include instructions or fancy ASCII-graphics */
	cout << endl << "WELCOME to epic Holy Diver v0.01" << endl;
	cout << "Enter commands and enjoy! (press q to quit at all times)" << endl << endl;
	cin.ignore();
}

/****************************************************************
 *
 * FUNCTION startup_routines
 *
 * Function performs any tasks necessary to set up a game
 * May contain game load dialogue, new player/game dialogue, level loading, random inits, whatever else
 *
 * At first week, this could be suitable place to load level map.
 *
 * **************************************************************/
void startup_routines(void)
{
	world = new World(); 	//World object 
	player1 = new Player(0, 0); //Player object with default values
	enemyStat = new EnemyStat(0, 0); //Enemy object with default values
	enemyMov = new EnemyMov(0, 0); //Enemy object with default values
	// For example if memory allocated here... (*)
	Information();	//Print help information how to play
	render_screen();		//Print initial game state
}

void GameOver()
{
	//Game over routines
}

void destroyObjects()
{
	//Free player object
	if (player1)
	{
		delete player1;
		player1 = nullptr;
	}
	if (enemyStat)
	{
		delete enemyStat;
		enemyStat = nullptr;
	}
	if (enemyMov)
	{
		delete enemyMov;
		enemyMov = nullptr;
	}
}

/****************************************************************
 *
 * FUNCTION quit_routines
 *
 * function performs any routines necessary at program shut-down, such as freeing memory or storing data files
 *
 * **************************************************************/
void quit_routines(void)
{

	// (*) ... the memory should be free'ed here at latest.
	world->freeLevel(map_rows);	//Free memory reserved by level information
	destroyObjects();	//Free player object

	cout << endl << "BYE! Welcome back soon." << endl;
}

