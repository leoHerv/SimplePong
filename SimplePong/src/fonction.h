#ifndef __FONCTION__H__
#define __FONCTION__H__

#define WINDOW_WIDTH 1280 
#define WINDOW_HEIGHT 720  

#define FIELD_WIDTH 1280
#define FIELD_Y 100
#define FIELD_HEIGHT 620

#define RAQUETTE_WIDTH 20
#define RAQUETTE_HEIGHT 100

#define FPS 120
#define FPS_LIMIT (1000 / FPS) // 60 fps (33 = 30 fps -> 1000 ms / 30 = 33)

#define VITESSE_RAQUETTES 4 // plus le nombre est grand plus la raquette ralenti
#define VITESSE_BALLE 2     // plus le nombre est grand plus la balle ralenti

typedef struct SDL_return
{
	SDL_Window *w; //window
	SDL_Renderer *r; //renderer
	SDL_Texture *t; //texture
} SDL_return;

typedef enum SDL_direction
{
	HAUT_D,
	HAUT_G,
	BAS_D,
	BAS_G
} SDL_direction;

typedef struct SDL_ball
{
	int x1;
	int y1;
	int x2;
	int y2;
	int etat; // il y a 7 etats : de 0 à 6 
	int etape; // il y a 6 etapes dans un etat, exemple : [0,1,0,0,1,0] donc de 0 à 5 etapes1
	SDL_direction dir; //direction de la balle
} SDL_ball;

typedef struct Score
{
	int ScoreJ1;
	int ScoreJ2;
	int change;
	int fin;
} Score;

// fenetre
void SDL_ExitWithError(const char *message);
void SDL_Exit_RendererAndWindow(const char *message, SDL_Renderer *renderer, SDL_Window *window);
void SDL_Exit_Texture(const char *message, SDL_Renderer *renderer, SDL_Window *window, SDL_Texture *texture);
void printImage(SDL_return *retour, char texte[], SDL_Rect *rect);
SDL_return SDL_InitFenetre(void);
void SDL_RenderClearRect(SDL_Rect *field, SDL_return retour);

//rectangle
SDL_bool inRect(int x, int y, SDL_Rect *rect);
SDL_bool rectInRect(SDL_Rect *rect1, SDL_Rect *rect2);

//balle
SDL_ball initBall(void);
void changeStateBall(SDL_ball *ball, int etat);
void changeDirectionBall(SDL_ball *ball, SDL_direction dir);
int lookEtapeFromEtat(int cas[], int etape);
void modifCoorBall(SDL_ball *ball, int x1, int y1, int x2, int y2);
void modifBall(SDL_ball *ball, int etat);
void moveBall(SDL_ball *ball);
void printBall(SDL_ball *ball, SDL_return retour);
void printWall(SDL_Rect *mur, SDL_return retour);
void collitionWall(SDL_ball *ball, SDL_Rect *mur1, SDL_Rect *mur2, SDL_Rect *mur3, SDL_Rect *mur4, SDL_Rect *raquette1, SDL_Rect *raquette2,
																						int up1, int down1, SDL_Rect *r2, int up2, int down2, Score *score);

//raquette
void moveRaquettes(SDL_Rect *r1, int up1, int down1, SDL_Rect *r2, int up2, int down2);

//gestion des fps
void limitFps(unsigned int valeurAvantLaBouclePlusLimit);

//nombre random
int randMinMax(int min, int max);

//Score
void texteAuFormatBMP(char debut[], int nombre, char varStock[]);
void printScore(SDL_return retour, Score *score);
#endif