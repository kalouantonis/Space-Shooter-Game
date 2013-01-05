/*
 * object.h
 *
 *  Created on: Jan 5, 2013
 *      Author: slacker
 */

#ifndef OBJECT_H_
#define OBJECT_H_

//ID's
enum IDS {
	PLAYER,
	BULLET,
	ENEMY,
	LIFE
};

//Keys
enum keys {
	UP,
	DOWN,
	LEFT,
	RIGHT,
	SPACE,
	ENTER
};

//Player
struct spaceship {
	int ID;
	int x;
	int y;
	int speed;
	int lives;
	int boundx;
	int boundy;
	int score;
	int level;
};

// Bullets
struct bullets {
	int ID;
	int x;
	int y;
	int speed;
	bool live;
};

// Comets
struct comets {
	int ID;
	int x;
	int y;
	int speed;
	int boundx;
	int boundy;
	bool live;
};

// Life Comet. Gives you life if you hit it
struct life_comet {
	int ID;
	int x;
	int y;
	int speed;
	int boundx;
	int boundy;
	bool live;
};

#endif /* OBJECT_H_ */
