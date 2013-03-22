/*
 * object.h
 *
 *  Created on: Jan 5, 2013
 *      Author: Antonis Kalou
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
struct Bullet {
	int ID;
	int x;
	int y;
	int speed;
	bool live;
};

// Comets
struct Comet {
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

// Point comet. Gives you extra points when you shoot it
struct point_comet {
	int ID;
	int x;
	int y;
	int speed;
	int boundx;
	int boundy;
	bool live;
};

#endif /* OBJECT_H_ */
