/*
 * space_fighter.cpp
 *
 *  Created on: Jan 3, 2013
 *      Author: Antonis Kalou
 */

#include <iostream>
#include <vector>
#include<allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "object.h"

using std::vector;

#define WIDTH 800
#define HEIGHT 400

//GLOBALS****************************************************

bool keys[6] = {false, false, false, false, false, false};

int score_level = 5000; // The interval at which the difficulty increases
bool is_game_over = false;

int NUM_BULLETS = 5; // Max number of bullets
int NUM_COMETS = 10; // Max number of comets

ALLEGRO_EVENT_QUEUE* event_queue;
ALLEGRO_TIMER* timer;
ALLEGRO_DISPLAY* display;

//*************************************************************

// Prototypes***************************

void init_ship(spaceship &ship);
void draw_ship(spaceship &ship);
void move_ship_up(spaceship &ship);
void move_ship_down(spaceship &ship);
void move_ship_right(spaceship &ship);
void move_ship_left(spaceship &ship);

void init_bullet(vector<Bullet> &);
void draw_bullet(const vector<Bullet> &);
void fire_bullet(vector<Bullet> &, spaceship &ship);
void update_bullet(vector<Bullet> &);
void collide_bullet(vector<Bullet> &, vector<Comet>&, spaceship &ship);

void init_comet(vector<Comet>&);
void draw_comet(const vector<Comet>&);
void start_comet(vector<Comet>&);
void update_comet(vector<Comet>&);
void collide_comet(vector<Comet>&, spaceship &ship);

void init_lComet(life_comet &lComet);
void draw_lComet(life_comet &lComet);
void start_lComet(life_comet &lComet);
void update_lComet(life_comet &lComet);
void collide_lComet(life_comet &lComet, vector<Bullet> &, spaceship &ship);

void init_pComet(point_comet &pComet);
void draw_pComet(point_comet &pComet);
void start_pComet(point_comet &pComet);
void update_pComet(point_comet &pComet);
void collide_pComet_bullet(point_comet &pComet, vector<Bullet> &, spaceship &ship);
void collide_pComet(point_comet &pComet, spaceship &ship);

void check_score(spaceship &ship, vector<Comet> &comet , point_comet &pComet);
//****************************************

static int init_allegro(void)
{
	int return_value = 0;

	// Init Allegro
	if(!al_init())
		return_value = 1;
	//Install Keyboard
	if(!al_install_keyboard())
		return_value = 1;

	// Create timer
	timer = al_create_timer(1.0 / 60); // 60 FPS
	if(!timer)
		return_value = 1;

	// Create display
	al_set_new_display_flags(ALLEGRO_WINDOWED);
	display = al_create_display(WIDTH, HEIGHT);
	if(!display)
		return_value = 1;

	// Create event queue
	event_queue = al_create_event_queue();
	if(!event_queue)
		return_value = 1;

	// Initialize primitives addon
	al_init_primitives_addon();

	// Initialize font addons
	al_init_font_addon();
	al_init_ttf_addon();

	// Register events
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_display_event_source(display));

	srand(time(NULL)); // Initialize random number generator. No need for headers in C++?

	return return_value; // For error checking
}

static void game_loop(void)
{
	bool done = false;
	bool redraw = true;
	spaceship ship = { 0 };
	vector<Bullet> bullets;
	vector<Comet> comets;
	life_comet lComet = { 0 };
	point_comet pComet = { 0 };

	// Object Initialization
	init_ship(ship);
	init_bullet(bullets);
	init_comet(comets);
	init_lComet(lComet);
	init_pComet(pComet);

	// Assign a font
	ALLEGRO_FONT *arial18 = al_load_font("arial.ttf", 18, 0);

	al_start_timer(timer); // Start the timer

	while(!done)
	{
		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);

		if(event.type == ALLEGRO_EVENT_TIMER)
		{
			redraw = true;

			// Key events processed here at 60 FPS for smoother movement
			if(keys[UP])
				move_ship_up(ship);
			if(keys[DOWN])
				move_ship_down(ship);
			if(keys[LEFT])
				move_ship_left(ship);
			if(keys[RIGHT])
				move_ship_right(ship);
			if((keys[ENTER]) && (is_game_over))
			{
				is_game_over = false;

				// Re-initialize all objects
				// Note: Might make goto call
				init_ship(ship);
				init_bullet(bullets);
				init_comet(comets);
				init_lComet(lComet);
				init_pComet(pComet);
			}
			if(!is_game_over)
			{
				// Update bullets
				update_bullet(bullets);

				//Start comets
				start_comet(comets);
				start_lComet(lComet);
				start_pComet(pComet);

				//Update comets
				update_comet(comets);
				update_lComet(lComet);
				update_pComet(pComet);

				// Check for collisions
				collide_bullet(bullets, comets, ship);
				collide_comet(comets, ship);
				collide_lComet(lComet, bullets, ship);
				collide_pComet(pComet, ship);
				collide_pComet_bullet(pComet, bullets, ship);

				// Check score
				check_score(ship, comets, pComet);

				if(ship.lives <= 0)
					is_game_over = true;
			}
		}
		else if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			done = true;
		else if(event.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch(event.keyboard.keycode)
			{
				case ALLEGRO_KEY_ESCAPE:
					done = true;
					break;
				case ALLEGRO_KEY_UP:
					keys[UP] = true;
					break;
				case ALLEGRO_KEY_DOWN:
					keys[DOWN] = true;
					break;
				case ALLEGRO_KEY_LEFT:
					keys[LEFT] = true;
					break;
				case ALLEGRO_KEY_RIGHT:
					keys[RIGHT] = true;
					break;
				case ALLEGRO_KEY_SPACE:
					keys[SPACE] = true;
					fire_bullet(bullets, ship); // Fire one every keypress
					break;
				case ALLEGRO_KEY_ENTER:
					keys[ENTER] = true;
					break;
			}
		}
		else if(event.type == ALLEGRO_EVENT_KEY_UP)
		{
			switch(event.keyboard.keycode)
			{
				case ALLEGRO_KEY_UP:
					keys[UP] = false;
					break;
				case ALLEGRO_KEY_DOWN:
					keys[DOWN] = false;
					break;
				case ALLEGRO_KEY_LEFT:
					keys[LEFT] = false;
					break;
				case ALLEGRO_KEY_RIGHT:
					keys[RIGHT] = false;
					break;
				case ALLEGRO_KEY_SPACE:
					keys[SPACE] = false;
					break;
			}
		}
		if((redraw) && (al_is_event_queue_empty(event_queue)))
		{
			redraw = false;

			if(!is_game_over)
			{
				//Object Drawing
				draw_ship(ship);
				draw_bullet(bullets);
				draw_comet(comets);
				draw_lComet(lComet);
				draw_pComet(pComet);

				// Display score and lives
				al_draw_textf(arial18, al_map_rgb(0, 255, 255), 30, 20, 0, "Score: %i", ship.score);
				al_draw_textf(arial18, al_map_rgb(0, 255, 255), 180, 20, 0, "Level: %i", ship.level);
				al_draw_textf(arial18, al_map_rgb(0, 255, 255), 30, 40, 0, "Lives: %i", ship.lives);
			}
			else
			{
				al_draw_textf(arial18, al_map_rgb(0, 255, 255), WIDTH / 2, HEIGHT / 2, ALLEGRO_ALIGN_CENTRE,
						"Game over! Final score: %i", ship.score);
				al_draw_text(arial18, al_map_rgb(0, 255, 255), WIDTH / 2, (HEIGHT / 2) + 25,
						ALLEGRO_ALIGN_CENTRE, "Press Enter to retry.");
			}


			al_flip_display();
			al_clear_to_color(al_map_rgb(0, 0, 0));
		}
	}
}

static void shutdown(void)
{
	if(timer)
		al_destroy_timer(timer);
	if(display)
		al_destroy_display(display);
	if(event_queue)
		al_destroy_event_queue(event_queue);
}

int main(void)
{
	int r = init_allegro();
	game_loop();
	shutdown();
	return r;
}

// Ship Definitions
void init_ship(spaceship &ship)
{
	ship.x = 20;
	ship.y = HEIGHT / 2;
	ship.ID = PLAYER;
	ship.lives = 3;
	ship.speed = 7;
	ship.boundx = 6;
	ship.boundy = 7;
	ship.score = 0;
	ship.level = 1;
}
void draw_ship(spaceship &ship)
{
	//Turrets
	al_draw_filled_rectangle(ship.x, ship.y - 9, ship.x + 10, ship.y - 7, al_map_rgb(255, 0, 0));
	al_draw_filled_rectangle(ship.x, ship.y + 9, ship.x + 10, ship.y + 7, al_map_rgb(255, 0, 0));

	//Body of ship
	al_draw_filled_triangle(ship.x - 12, ship.y - 17, ship.x + 12, ship.y, ship.x -12, ship.y + 17,
				al_map_rgb(0, 255, 0));
	al_draw_filled_rectangle(ship.x - 12, ship.y - 2, ship.x + 15, ship.y +2, al_map_rgb(0, 0, 255));
}
void move_ship_up(spaceship &ship)
{
	ship.y -= ship.speed;
	if(ship.y < 0)
		ship.y = 0;
}
void move_ship_down(spaceship &ship)
{
	ship.y += ship.speed;
	if(ship.y > HEIGHT)
		ship.y = HEIGHT;
}
void move_ship_right(spaceship &ship)
{
	ship.x += ship.speed;
	if(ship.x > WIDTH / 3)
		ship.x = WIDTH / 3;
}
void move_ship_left(spaceship &ship)
{
	ship.x -= ship.speed;
	if(ship.x < 0)
		ship.x = 0;
}

// Bullet definitions

void init_bullet(vector<Bullet> &bullets)
{
	for(int i=0; i < NUM_BULLETS; i++)
	{
		struct Bullet single_bullet;
		single_bullet.ID = BULLET;
		single_bullet.speed = 10;
		single_bullet.live = false;

		bullets.push_back(single_bullet);
	}
}
void draw_bullet(const vector<Bullet> &bullets)
{
	for(vector<Bullet>::const_iterator i = bullets.begin(); i != bullets.end(); i++)
	{
		if(i->live)
			al_draw_filled_circle(i->x, i->y, 2, al_map_rgb(255, 255, 255));
	}
}
void fire_bullet(vector<Bullet> &bullets, spaceship &ship)
{
	for(vector<Bullet>::iterator i = bullets.begin(); i != bullets.end(); i++)
	{
		if(!i->live)
		{
			i->x = ship.x + 17; // Up to me
			i->y = ship.y;
			i->live = true;
			break; // Dont want to fire all the bullets at the same time
		}
	}
}
void update_bullet(vector<Bullet> &bullets)
{
	for(vector<Bullet>::iterator i = bullets.begin(); i != bullets.end(); i++)
	{
		if(i->live)
		{
			i->x += i->speed;
			if(i->x > WIDTH)
				i->live = false;
		}
	}
}
void collide_bullet(vector<Bullet> &bullets, vector<Comet> &comets, spaceship &ship)
{
	// Change comets to iter
	for(vector<Bullet>::iterator bullet_iter = bullets.begin();
			bullet_iter != bullets.end(); bullet_iter++)
	{
		if(bullet_iter->live)
		{
			for(vector<Comet>::iterator com_iter = comets.begin();
					com_iter != comets.end(); com_iter++)
			{
				if(com_iter->live)
				{
					if((bullet_iter->x > (com_iter->x - com_iter->boundx)) &&
							(bullet_iter->x < (com_iter->x + com_iter->boundx)) &&
							(bullet_iter->y > (com_iter->y - com_iter->boundy)) &&
							(bullet_iter->y < (com_iter->y + com_iter->boundy)))
					{
						/* If all these conditions are true, we know that the bullet is between
						 * the top and the bottom and the left and the right of the bounding box.
						 *
						 * comet[j].x - comet[j].boundx is the position of the far left of the
						 * bounding box*/
						bullet_iter->live = false;
						com_iter->live = false;
						ship.score += 100;
					}
				}
			}
		}
	}
}

// Comet definitions

void init_comet(vector<Comet> &comets)
{
	for(int i = 0; i < NUM_COMETS; i++)
	{
		struct Comet each_comet;
		each_comet.ID = ENEMY;
		each_comet.live = false;
		each_comet.speed = 5; // Up to me, might increase according to difficulty
		each_comet.boundx = 18;
		each_comet.boundy = 18;

		comets.push_back(each_comet);
	}
}
void draw_comet(const vector<Comet> &comets)
{
	for(vector<Comet>::const_iterator i = comets.begin(); i != comets.end(); i++)
	{
		if(i->live)
			al_draw_filled_circle(i->x, i->y, 20, al_map_rgb(0, 255, 0));
	}
}
void start_comet(vector<Comet> &comets)
{
	for(vector<Comet>::iterator i = comets.begin(); i != comets.end(); i++)
	{
		if(rand() % 500 == 0) // One out of 500 times. Random generation of comets
		{
			i->live = true;
			i->x = WIDTH;
			i->y = 30 + rand() % (HEIGHT - 60); // Spawn comets at random location but not at top or bottom
		}
	}
}
void update_comet(vector<Comet> &comets)
{
	for(vector<Comet>::iterator i = comets.begin(); i != comets.end(); i++)
	{
		if(i->live)
		{
			i->x -= i->speed;
			if(i->x < 0)
				i->live = false;
		}
	}
}
void collide_comet(vector<Comet> &comets, spaceship &ship)
{
	for(vector<Comet>::iterator i = comets.begin(); i != comets.end(); i++)
	{
		if(i->live)
		{
			if(((i->x - i->boundx) < (ship.x + ship.boundx)) &&
					((i->x + i->boundx) > (ship.x - ship.boundx)) &&
					((i->y - i->boundy) < (ship.y + ship.boundy)) &&
					((i->y + i->boundy) > (ship.y - ship.boundy)))
			{
				ship.lives--;
				i->live = false;
			}
		}
	}
}

// Life comet
void init_lComet(life_comet &lComet)
{
	lComet.ID = LIFE;
	lComet.live = false;
	lComet.speed = 5;
	lComet.boundx = 18;
	lComet.boundy = 18;
}
void draw_lComet(life_comet &lComet)
{
	if(lComet.live)
		al_draw_filled_circle(lComet.x, lComet.y, 20, al_map_rgb(255, 0, 0));
}
void start_lComet(life_comet &lComet)
{
	if(rand() % 9000 == 0) // Once in every 9,000 cycles
	{
		lComet.live = true;
		lComet.x = WIDTH;
		lComet.y = 30 + rand() % (HEIGHT - 60);
	}
}
void update_lComet(life_comet &lComet)
{
	if(lComet.live)
	{
		lComet.x -= lComet.speed;
		if(lComet.x < 0)
			lComet.live = false;
	}
}
void collide_lComet(life_comet &lComet, vector<Bullet> &bullets, spaceship &ship)
{
	// Not like collide_comet, as it only counts bullet collisions
	for(vector<Bullet>::iterator bullet_iter = bullets.begin();
			bullet_iter != bullets.end(); bullet_iter++)
	{
		if(bullet_iter->live)
		{
			if(lComet.live)
			{
				if((bullet_iter->x > (lComet.x - lComet.boundx)) &&
						(bullet_iter->x < (lComet.x + lComet.boundx)) &&
						(bullet_iter->y > (lComet.y - lComet.boundy)) &&
						(bullet_iter->y < (lComet.y + lComet.boundy)))
				{
					bullet_iter->live = false;
					lComet.live = false;
					ship.lives += 1;
				}
			}
		}
	}
}

// Point comet
void init_pComet(point_comet &pComet)
{
	pComet.ID = ENEMY;
	pComet.live = false;
	pComet.speed = 5;
	pComet.boundx = 18;
	pComet.boundy = 18;
}
void draw_pComet(point_comet &pComet)
{
	if(pComet.live)
		al_draw_filled_circle(pComet.x, pComet.y, 20, al_map_rgb(0, 0, 255));
}
void start_pComet(point_comet &pComet)
{
	if(rand() % 1750 == 0) // Once in every 1750 cycles
	{
		pComet.live = true;
		pComet.x = WIDTH;
		pComet.y = 30 + rand() % (HEIGHT - 60);
	}
}
void update_pComet(point_comet &pComet)
{
	if(pComet.live)
	{
		pComet.x -= pComet.speed;
		if(pComet.x < 0)
			pComet.live = false;
	}
}
void collide_pComet_bullet(point_comet &pComet, vector<Bullet> &bullets, spaceship &ship)
{
	for(vector<Bullet>::iterator bullet_iter = bullets.begin();
			bullet_iter != bullets.end(); bullet_iter++)
	{
		if(bullet_iter->live)
		{
			if(pComet.live)
			{
				if((bullet_iter->x > (pComet.x - pComet.boundx)) &&
						(bullet_iter->x < (pComet.x + pComet.boundx)) &&
						(bullet_iter->y > (pComet.y - pComet.boundy)) &&
						(bullet_iter->y < (pComet.y + pComet.boundy)))
				{
					bullet_iter->live = false;
					pComet.live = false;
					ship.score += 500;
				}
			}
		}
	}
}
void collide_pComet(point_comet &pComet,  spaceship &ship)
{
	if(pComet.live)
	{
		if(((pComet.x - pComet.boundx) < (ship.x + ship.boundx)) &&
				((pComet.x + pComet.boundx) > (ship.x - ship.boundx)) &&
				((pComet.y - pComet.boundy) < (ship.y + ship.boundy)) &&
				((pComet.y + pComet.boundy) > (ship.y - ship.boundy)))
		{
			ship.lives--;
			pComet.live = false;
		}
	}
}
// Logic

int prev_level = 0;

void check_score(spaceship &ship, vector<Comet> &comets,  point_comet &pComet)
{
	if(ship.score >= score_level)
	{
		for(vector<Comet>::iterator i = comets.begin(); i != comets.end() ; i++)
		{
			i->speed += 1;
		}
		ship.level += 1;
		pComet.speed += 1;
		score_level *= 2;

		if(ship.level == prev_level + 3) {
			NUM_BULLETS++; // Every 2 levels after level 3 you get an extra bullet
			NUM_COMETS++; // .. and an extra comet :)
			prev_level += 2;
		}
	}
}
