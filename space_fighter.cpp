/*
 * space_fighter.cpp
 *
 *  Created on: Jan 3, 2013
 *      Author: slacker
 */

#include <iostream>
#include<allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "object.h"

#define WIDTH 800
#define HEIGHT 400

//GLOBALS****************************************************

bool keys[6] = {false, false, false, false, false, false};

int score_level = 5000; // The interval at which the difficulty increases
bool is_game_over = false;

const int NUM_BULLETS = 5; // Max number of bullets
const int NUM_COMETS = 10; // Max number of comets

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

void init_bullet(bullets bullet[]);
void draw_bullet(bullets bullet[]);
void fire_bullet(bullets bullet[], spaceship &ship);
void update_bullet(bullets bullet[]);
void collide_bullet(bullets bullet[], comets comet[], spaceship &ship);

void init_comet(comets comet[]);
void draw_comet(comets comet[]);
void start_comet(comets comet[]);
void update_comet(comets comet[]);
void collide_comet(comets comet[], spaceship &ship);

void init_lComet(life_comet &lComet);
void draw_lComet(life_comet &lComet);
void start_lComet(life_comet &lComet);
void update_lComet(life_comet &lComet);
void collide_lComet(life_comet &lComet, bullets bullet[], spaceship &ship);

void check_score(spaceship &ship, comets comet[], life_comet &lComet);
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
	bullets bullet[NUM_BULLETS];
	comets comet[NUM_COMETS];
	life_comet lComet = { 0 };

	// Object Initialization
	init_ship(ship);
	init_bullet(bullet);
	init_comet(comet);
	init_lComet(lComet);

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
				init_ship(ship);
				init_bullet(bullet);
				init_comet(comet);
				init_lComet(lComet);
			}
			if(!is_game_over)
			{
				// Update bullets
				update_bullet(bullet);

				//Start comets
				start_comet(comet);
				start_lComet(lComet);

				//Update comets
				update_comet(comet);
				update_lComet(lComet);

				// Check for collisions
				collide_bullet(bullet, comet, ship);
				collide_comet(comet, ship);
				collide_lComet(lComet, bullet, ship);

				// Check score
				check_score(ship, comet, lComet);

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
					fire_bullet(bullet, ship); // Fire one every keypress
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
				draw_bullet(bullet);
				draw_comet(comet);
				draw_lComet(lComet);

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

void init_bullet(bullets bullet[])
{
	for(int i = 0; i < NUM_BULLETS; i++)
	{
		bullet[i].ID = BULLET;
		bullet[i].speed = 10;
		bullet[i].live = false;
	}
}
void draw_bullet(bullets bullet[])
{
	for(int i = 0; i < NUM_BULLETS; i++)
	{
		if(bullet[i].live)
			al_draw_filled_circle(bullet[i].x, bullet[i].y, 2, al_map_rgb(255, 255, 255));
	}
}
void fire_bullet(bullets bullet[], spaceship &ship)
{
	for(int i = 0; i < NUM_BULLETS; i++)
	{
		if(!bullet[i].live)
		{
			bullet[i].x = ship.x + 17; // Up to me
			bullet[i].y = ship.y;
			bullet[i].live = true;
			break; // Dont want to fire all the bullets at the same time
		}
	}
}
void update_bullet(bullets bullet[])
{
	for(int i = 0; i < NUM_BULLETS; i++)
	{
		if(bullet[i].live)
		{
			bullet[i].x += bullet[i].speed;
			if(bullet[i].x > WIDTH)
				bullet[i].live = false;
		}
	}
}
void collide_bullet(bullets bullet[], comets comet[], spaceship &ship)
{
	for(int i = 0; i < NUM_BULLETS; i++)
	{
		if(bullet[i].live)
		{
			for(int j = 0; j < NUM_COMETS; j++)
			{
				if(comet[j].live)
				{
					if((bullet[i].x > (comet[j].x - comet[j].boundx)) &&
							(bullet[i].x < (comet[j].x + comet[j].boundx)) &&
							(bullet[i].y > (comet[j].y - comet[j].boundy)) &&
							(bullet[i].y < (comet[j].y + comet[j].boundy)))
					{
						/* If all these conditions are true, we know that the bullet is between
						 * the top and the bottom and the left and the right of the bounding box.
						 *
						 * comet[j].x - comet[j].boundx is the position of the far left of the
						 * bounding box*/
						bullet[i].live = false;
						comet[j].live = false;
						ship.score += 100;
					}
				}
			}
		}
	}
}

// Comet definitions

void init_comet(comets comet[])
{
	for(int i = 0; i < NUM_COMETS; i++)
	{
		comet[i].ID = ENEMY;
		comet[i].live = false;
		comet[i].speed = 5; // Up to me, might increase according to difficulty
		comet[i].boundx = 18;
		comet[i].boundy = 18;
	}
}
void draw_comet(comets comet[])
{
	for(int i = 0; i < NUM_COMETS; i++)
	{
		if(comet[i].live)
			al_draw_filled_circle(comet[i].x, comet[i].y, 20, al_map_rgb(0, 255, 0));
	}
}
void start_comet(comets comet[])
{
	for(int i = 0; i < NUM_COMETS; i++)
	{
		if(rand() % 500 == 0) // One out of 500 times. Random generation of comets
		{
			comet[i].live = true;
			comet[i].x = WIDTH;
			comet[i].y = 30 + rand() % (HEIGHT - 60); // Spawn comets at random location but not at top or bottom
		}
	}
}
void update_comet(comets comet[])
{
	for(int i = 0; i < NUM_COMETS; i++)
	{
		if(comet[i].live)
		{
			comet[i].x -= comet[i].speed;
			if(comet[i].x < 0)
				comet[i].live = false;
		}
	}
}
void collide_comet(comets comet[], spaceship &ship)
{
	for(int i = 0; i < NUM_COMETS; i++)
	{
		if(comet[i].live)
		{
			if(((comet[i].x - comet[i].boundx) < (ship.x + ship.boundx)) &&
					((comet[i].x + comet[i].boundx) > (ship.x - ship.boundx)) &&
					((comet[i].y - comet[i].boundy) < (ship.y + ship.boundy)) &&
					((comet[i].y + comet[i].boundy) > (ship.y - ship.boundy)))
			{
				ship.lives--;
				comet[i].live = false;
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
	if(rand() % 10000 == 0) // Once in every 10,000 cycles
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
void collide_lComet(life_comet &lComet, bullets bullet[], spaceship &ship)
{
	// Not like collide_comet, as it only counts bullet collisions
	for(int i = 0; i < NUM_BULLETS; i++)
	{
		if(bullet[i].live)
		{
			if(lComet.live)
			{
				if((bullet[i].x > (lComet.x - lComet.boundx)) &&
						(bullet[i].x < (lComet.x + lComet.boundx)) &&
						(bullet[i].y > (lComet.y - lComet.boundy)) &&
						(bullet[i].y < (lComet.y + lComet.boundy)))
				{
					bullet[i].live = false;
					lComet.live = false;
					ship.lives += 1;
				}
			}
		}
	}
}


// Logic

void check_score(spaceship &ship, comets comet[], life_comet &lComet)
{
	if(ship.score >= score_level)
	{
		for(int i = 0; i < NUM_COMETS; i++)
		{
			comet[i].speed += 1;
		}
		ship.level += 1;
		score_level *= 2;
	}
}
