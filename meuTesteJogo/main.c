#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include "objects.h"


//Variáveis globais
const int WIDTH = 1500;
const int HEIGHT = 800;
int NUM_BULLETS = 6;
int NUM_COMETS = 1;
int metaScore = 20;
int limitTimer = 1.5 * 60;
enum KEYS { UP, DOWN, LEFT, RIGHT, SPACE };
bool keys[5] = { false, false, false, false };

//Protótipos

void InitShip(SpaceShip* ship);
void DrawShip(SpaceShip* ship);
void MoveShipUp(SpaceShip* ship);
void MoveShipDown(SpaceShip* ship);
void MoveShipLeft(SpaceShip* ship);
void MoveShipRight(SpaceShip* ship);

void InitBullet(Bullet bullet[], int size);
void DrawBullet(Bullet bullet[], int size);
void FireBullet(Bullet bullet[], int size, SpaceShip* ship);
void UpdateBullet(Bullet bullet[], int size);
void CollideBullet(Bullet bullet[], int bSize, Comet comets[], int cSize);

void InitComet(Comet comets[], int size);
void DrawComet(Comet comets[], int size);
void StartComet(Comet comets[], int size);
void UpdateComet(Comet comets[], int size, mouseScope* scope);
void CollideComet(Comet comets[], int cSize, SpaceShip* ship);

void InitScope(mouseScope *scope);
void DrawScope(mouseScope* scope);
void MoveScope(mouseScope* scope, ALLEGRO_EVENT ev);
void CollideScope(mouseScope* scope, ALLEGRO_EVENT ev, Comet comets[], int cSize);



int main(void)
{

	//Variável primitiva
	bool done = false;
	bool redraw = true;
	const int FPS = 60;
	bool isGameOver = false;
	bool gameWon = false;

	//Variável de objeto
	SpaceShip ship;
	Bullet bullets[6];
	Comet comets[1];
	mouseScope scope;

	//Variáveis do Allegro
	ALLEGRO_DISPLAY* display = NULL;
	ALLEGRO_EVENT_QUEUE* event_queue = NULL;
	ALLEGRO_TIMER* timer = NULL;


	if (!al_init()) //Inicializar Allegro
		return -1;

	display = al_create_display(WIDTH, HEIGHT);

	if (!display)
		return -1;

	al_init_primitives_addon();
	al_install_keyboard();
	al_install_mouse();
	al_init_font_addon();
	al_init_ttf_addon();

	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0 / FPS);

	ALLEGRO_FONT* info = al_load_font("CooperBlackStd.otf", 30, 0);
	ALLEGRO_FONT* endGame = al_load_font("CooperBlackStd.otf", 60, 0);
	ALLEGRO_FONT* endGame2 = al_load_font("CooperBlackStd.otf", 40, 0);

	srand(time(NULL));
	InitShip(&ship);
	InitBullet(bullets, NUM_BULLETS);
	InitComet(comets, NUM_COMETS);
	InitScope(&scope);

	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_mouse_event_source());
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_display_event_source(display));

	al_rest(1.0);

	al_start_timer(timer);
	al_hide_mouse_cursor(display);

	while (!done)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if (ev.type == ALLEGRO_EVENT_TIMER)
		{
			redraw = true;
			if (keys[UP])
				MoveShipUp(&ship);
			if (keys[DOWN])
				MoveShipDown(&ship);
			if (keys[LEFT])
				MoveShipLeft(&ship);
			if (keys[RIGHT])
				MoveShipRight(&ship);

			UpdateBullet(bullets, NUM_BULLETS);
			StartComet(comets, NUM_COMETS);
			UpdateComet(comets, NUM_COMETS, &scope);
			CollideBullet(bullets, NUM_BULLETS, comets, NUM_COMETS);
			CollideComet(comets, NUM_COMETS, &ship);
		}

		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			done = true;
		}

		if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch (ev.keyboard.keycode)
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
				FireBullet(bullets, NUM_BULLETS, &ship);
				break;
			}
		}
		if (ev.type == ALLEGRO_EVENT_KEY_UP)
		{
			switch (ev.keyboard.keycode)
			{
			case ALLEGRO_KEY_ESCAPE:
				done = true;
				break;
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
			}
		}

		if (ev.type == ALLEGRO_EVENT_MOUSE_AXES)
		{
			MoveScope(&scope, ev);
		}

		if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
		{
			if (ev.mouse.button & 1)
			{
				CollideScope(&scope, ev, comets, NUM_COMETS);
			}
		}

		if (scope.lives <= 0 || scope.score >= metaScore)
		{
			isGameOver = true;

			if (scope.lives > 0)
			{
				gameWon = true;
			}
		}

		if (redraw && al_is_event_queue_empty(event_queue))
		{

			if (!isGameOver)
			{
				redraw = false;

				DrawShip(&ship);
				DrawBullet(bullets, NUM_BULLETS);
				DrawComet(comets, NUM_COMETS);
				DrawScope(&scope);

				al_draw_textf(info, al_map_rgb(0, 255, 0), WIDTH / 2, 20, ALLEGRO_ALIGN_CENTRE, "META: %d PONTOS", metaScore);
				al_draw_textf(info, al_map_rgb(255, 255, 255), 20, 20, 0, "Seus pontos: %d", scope.score);
				al_draw_textf(info, al_map_rgb(255, 255, 255), WIDTH - 20, 20, ALLEGRO_ALIGN_RIGHT, "Vidas restantes: %d", scope.lives);
			}
			else
			{
				done = true;

				if (gameWon)
				{
					al_draw_text(endGame, al_map_rgb(0, 255, 0), WIDTH / 2, 350, ALLEGRO_ALIGN_CENTRE, "VOCE VENCEU!");
					al_draw_textf(endGame2, al_map_rgb(0, 255, 0), WIDTH / 2, 415, ALLEGRO_ALIGN_CENTRE, "Vidas restantes: %d", scope.lives);
				}
				else
				{
					al_draw_text(endGame, al_map_rgb(255, 0, 0), WIDTH / 2, 350, ALLEGRO_ALIGN_CENTRE, "GAME OVER");
					al_draw_textf(endGame2, al_map_rgb(255, 0, 0), WIDTH / 2, 415, ALLEGRO_ALIGN_CENTRE, "Pontuacao final: %d", scope.score);
				}
			}

			al_flip_display();
			al_clear_to_color(al_map_rgb(0, 0, 0));
		}

	}

	if (isGameOver)
	{
		al_rest(5.0);
	}

	al_destroy_display(display);

	return 0;

}

void InitShip(SpaceShip* ship)
{
	ship->x = 20;
	ship->y = HEIGHT / 2;
	ship->ID = PLAYER;
	ship->lives = 3;
	ship->speed = 6;
	ship->boundx = 6;
	ship->boundy = 7;
	ship->score = 0;
}

void DrawShip(SpaceShip* ship)
{
	al_draw_filled_rectangle(ship->x, ship->y - 9, ship->x + 10, ship->y - 7, al_map_rgb(0, 255, 0));
	al_draw_filled_rectangle(ship->x, ship->y + 9, ship->x + 10, ship->y + 7, al_map_rgb(0, 255, 0));
	al_draw_filled_triangle(ship->x - 12, ship->y - 17, ship->x + 12, ship->y, ship->x - 12, ship->y + 17, al_map_rgb(255, 255, 255));
	al_draw_filled_rectangle(ship->x - 12, ship->y - 2, ship->x + 15, ship->y + 2, al_map_rgb(0, 255, 0));
}
void MoveShipUp(SpaceShip* ship)
{
	ship->y -= ship->speed;
	if (ship->y < 0)
	{
		ship->y = 0;
	}
}
void MoveShipDown(SpaceShip* ship)
{
	ship->y += ship->speed;
	if (ship->y > HEIGHT)
	{
		ship->y = HEIGHT;
	}
}
void MoveShipLeft(SpaceShip* ship)
{
	ship->x -= ship->speed;
	if (ship->x < 0)
	{
		ship->x = 0;
	}
}
void MoveShipRight(SpaceShip* ship)
{
	ship->x += ship->speed;
	if (ship->x > 750)
	{
		ship->x = 750;
	}
}

void InitBullet(Bullet bullet[], int size)
{
	for (int i = 0; i < size; i++)
	{
		bullet[i].ID = BULLET;
		bullet[i].speed = 10;
		bullet[i].live = false;
	}
}
void DrawBullet(Bullet bullet[], int size)
{
	for (int i = 0; i < size; i++)
	{
		if (bullet[i].live)
			al_draw_filled_circle(bullet[i].x, bullet[i].y, 4, al_map_rgb(0, 255, 0));
	}
}
void FireBullet(Bullet bullet[], int size, SpaceShip* ship)
{
	for (int i = 0; i < size; i++)
	{
		if (!bullet[i].live)
		{
			bullet[i].x = ship->x + 17;
			bullet[i].y = ship->y;
			bullet[i].live = true;
			break;
		}

	}
}
void UpdateBullet(Bullet bullet[], int size)
{
	for (int i = 0; i < size; i++)
	{
		if (bullet[i].live)
		{
			bullet[i].x += bullet[i].speed;
			if (bullet[i].x > WIDTH)
			{
				bullet[i].live = false;
			}
		}
	}
}

void InitComet(Comet comets[], int size)
{
	for (int i = 0; i < size; i++)
	{
		comets[i].ID = ENEMY;
		comets[i].live = false;
		comets[i].boundx = 22;
		comets[i].boundy = 22;
	}
}
void DrawComet(Comet comets[], int size)
{
	for (int i = 0; i < size; i++)
	{
		if (comets[i].live)
		{
			al_draw_filled_circle(comets[i].x, comets[i].y, 22, al_map_rgb(255, 0, 0));
		}
	}
}
void StartComet(Comet comets[], int size)
{
	for (int i = 0; i < size; i++)
	{
		if (!comets[i].live)
		{
			comets[i].live = true;
			comets[i].x = rand() % 1300 + 100;
			comets[i].y = rand() % 600 + 100;
			comets[i].lifetime = 0;
		}
	}
}
void UpdateComet(Comet comets[], int size, mouseScope* scope)
{
	for (int i = 0; i < size; i++)
	{
		if (comets[i].live)
		{
			comets[i].lifetime++;
		}

		if (comets[i].lifetime > limitTimer)
		{
			comets[i].live = false;
			scope->lives--;
		}
	}
}
void CollideBullet(Bullet bullet[], int bSize, Comet comets[], int cSize)
{
	for (int i = 0; i < bSize; i++)
	{
		if (bullet[i].live)
		{
			for (int j = 0; j < cSize; j++)
			{
				if (comets[j].live)
				{
					if (bullet[i].x > (comets[j].x - comets[j].boundx) &&
						bullet[i].x < (comets[j].x + comets[j].boundx) &&
						bullet[i].y >(comets[j].y - comets[j].boundy) &&
						bullet[i].y < (comets[j].y + comets[j].boundy))
					{
						bullet[i].live = false;
						comets[j].live = false;
					}
				}
			}
		}
	}
}
void CollideComet(Comet comets[], int cSize, SpaceShip* ship)
{
	for (int i = 0; i < cSize; i++)
	{
		if (comets[i].live)
		{
			if (comets[i].x - comets[i].boundx < ship->x + ship->boundx &&
				comets[i].x + comets[i].boundx > ship->x - ship->boundx &&
				comets[i].y - comets[i].boundy < ship->y + ship->boundy &&
				comets[i].y + comets[i].boundy > ship->y - ship->boundy)
			{
				ship->lives--;
				comets[i].live = false;
			}
			else if (comets[i].x < 0)
			{
				ship->lives--;
				comets[i].live = false;
			}
		}

	}
}

void InitScope(mouseScope* scope)
{
	scope->ID = SCOPE;
	scope->x = WIDTH / 2;
	scope->y = HEIGHT / 2;
	scope->live = true;
	scope->score = 0;
	scope->lives = 3;
}

void DrawScope(mouseScope* scope)
{
	al_draw_circle(scope->x, scope->y, 10, al_map_rgb(0, 255, 0), 2);
	al_draw_line(scope->x - 10, scope->y, scope->x + 10, scope->y, al_map_rgb(0, 255, 0), 1.5);
	al_draw_line(scope->x, scope->y - 10, scope->x, scope->y + 10, al_map_rgb(0, 255, 0), 1.5);
}

void MoveScope(mouseScope* scope, ALLEGRO_EVENT ev)
{
	scope->x = ev.mouse.x;
	scope->y = ev.mouse.y;
}

void CollideScope(mouseScope* scope, ALLEGRO_EVENT ev, Comet comets[], int cSize)
{
	scope->clickx = ev.mouse.x;
	scope->clicky = ev.mouse.y;

	for (int j = 0; j < cSize; j++)
	{
		if (comets[j].live)
		{
			if (scope->clickx > (comets[j].x - comets[j].boundx) &&
				scope->clickx < (comets[j].x + comets[j].boundx) &&
				scope->clicky >(comets[j].y - comets[j].boundy) &&
				scope->clicky < (comets[j].y + comets[j].boundy))
			{
				comets[j].live = false;
				scope->score++;
			}
		}
	}
}