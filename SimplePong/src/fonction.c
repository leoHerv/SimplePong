#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "fonction.h"


// Message d'erreur et fin du programme
void SDL_ExitWithError(const char *message)
{
	SDL_Log(" ERREUR : Initialisation SDL > %s\n", message);
	SDL_Delay(2000);
	SDL_Quit();
	exit(EXIT_FAILURE);
}

// Message d'erreur + destruction du rendu, et de la fenetre
void SDL_Exit_RendererAndWindow(const char *message, SDL_Renderer *renderer, SDL_Window *window)
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_ExitWithError(message);
}		

// Message d'erreur + destruction de la texture, du rendu, et de la fenetre
void SDL_Exit_Texture(const char *message, SDL_Renderer *renderer, SDL_Window *window, SDL_Texture *texture)
{
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_ExitWithError(message);
}

// Affiche une image sur le rendu 
void printImage(SDL_return *retour, char texte[], SDL_Rect *rect)
{
	SDL_Surface *image = NULL;
	SDL_Texture *texture = NULL;

	//Chargement de l'image
	image = SDL_LoadBMP(texte);

	if(image == NULL)
		SDL_Exit_Texture("Impossible de charger l'image", retour->r, retour->w, texture);

	// Création de la texture avec l'image et le rendu
	texture = SDL_CreateTextureFromSurface(retour->r, image);
	// On libère l'image de la mémoire
	SDL_FreeSurface(image);

	if(texture == NULL)
		SDL_Exit_Texture("Impossible de creer la texture", retour->r, retour->w, texture);

	// Chargement de la texture en mémoire
	if(SDL_QueryTexture(texture, NULL, NULL, &rect->w, &rect->h) != 0)
		SDL_Exit_Texture("Impossible de charger la texture", retour->r, retour->w, texture);

	// Copie de la texture sur le rendu
	if(SDL_RenderCopy(retour->r, texture, NULL, rect) != 0)
		SDL_Exit_Texture("Impossible d'afficher la texture", retour->r, retour->w, texture);

	SDL_RenderPresent(retour->r);
	retour->t = texture;
}

// Affiche la page principale de selection et retourne la texture, la fenetre et le rendu (SDL_return retour)
SDL_return SDL_InitFenetre(void)
{
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	//Lancement SDL
	if(SDL_Init(SDL_INIT_VIDEO) != 0)
		SDL_ExitWithError("Inialisation SDL");

	//Création fenêtre + rendu
	if(SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer) != 0)
		SDL_ExitWithError("Impossible de creer la fenetre et le rendu");

	SDL_return retour;
	retour.w = window;
	retour.r = renderer;
	retour.t = NULL;

	return retour;
}

// fait un clear avec du rectangle indiquer en noir
void SDL_RenderClearRect(SDL_Rect *field, SDL_return retour)
{
	if(SDL_SetRenderDrawColor(retour.r, 0, 0, 0, SDL_ALPHA_OPAQUE) != 0) // noir
		SDL_Exit_Texture("Impossible de changer la couleur du rendu dans SDL_RenderClearRect", retour.r, retour.w, retour.t);
	if(SDL_RenderFillRect(retour.r, field) != 0)
		SDL_Exit_Texture("Impossible d afficher le rectangle de SDL_RenderClearRect", retour.r, retour.w, retour.t);
}

// regarde si la pos x,y est dans le rectangle rect
SDL_bool inRect(int x, int y, SDL_Rect *rect)
{
	return((x >= rect->x)
		&& (x <= (rect->w + rect->x) )
		&& (y >= rect->y)
		&& (y <= (rect->h + rect->y) ) );
}

// regarde si un rectangle1 est dans un autre rectangle2
SDL_bool rectInRect(SDL_Rect *rect1, SDL_Rect *rect2)
{
	  return !( (rect2->x >= rect1->x + rect1->w) // trop à droite
	  		 || (rect2->x + rect2->w <= rect1->x) // trop à gauche
	  		 || (rect2->y >= rect1->y + rect1->h) // trop en bas
	  		 || (rect2->y + rect2->h <= rect1->y) ); // trop en haut
}

/*-----------------------------------------------------------------------*/
/*---------------------------FONCTION DE BALLE---------------------------*/
/*------------------------------------------------------------------------*/

// initialisation d'une balle
SDL_ball initBall(void)
{
	SDL_ball bal;
	int ballW = 10;
	int ballH = 10;
	bal.x1 = (WINDOW_WIDTH - ballW) / 2;
	bal.y1 = (WINDOW_HEIGHT - ballH) / 2 + (FIELD_Y / 2) ; 
	bal.x2 = (bal.x1 + ballW);
	bal.y2 = (bal.y1 + ballH);
	bal.etat = 0;
	bal.etape = 0;
	bal.dir = randMinMax(0, 3);
	return bal;
}

// change l'etat de la balle
void changeStateBall(SDL_ball *ball, int etat)
{
	ball->etat = etat;
	ball->etape = 0;
}

// change la direction de la balle
void changeDirectionBall(SDL_ball *ball, SDL_direction dir)
{
	ball->dir = dir;
}

// regarde le etat qui ce trouve à l'etape "etape"
int lookEtapeFromEtat(int etat[], int etape)
{
	if(etape < 0 || etape > 5)
		printf("erreur lookEtapeFromEtat : etape est négatif ou superieur a 6");
	return etat[etape];
}

// change les pos x1, x2, y1, y2 de la balle selon les parametres
void modifCoorBall(SDL_ball *ball, int x1, int y1, int x2, int y2)
{
	int deplacement = (1000 / FPS) / VITESSE_BALLE;
	ball->x1 += x1 * deplacement;
	ball->y1 += y1 * deplacement;
	ball->x2 += x2 * deplacement;
	ball->y2 += y2 * deplacement;
	if(ball->etape == 5)
		ball->etape = 0;
	else
		ball->etape++;
}

// modifie x et/ou y en fonction de l'etat
void modifBall(SDL_ball *ball, int etat)
{
	switch(ball->dir)
	{
		case HAUT_D :
			modifCoorBall(ball, 1, etat, 1, etat);
			break;
		case HAUT_G :
			modifCoorBall(ball, -1, etat, -1, etat);
			break;
		case BAS_D :
			modifCoorBall(ball, 1, -(etat), 1, -(etat));
			break;
		case BAS_G :
			modifCoorBall(ball, -1, -(etat), -1, -(etat));
			break;
		default:
			break;
	}
}

// fait bouger la balle avec la direction et l'angle qui lui est donnée
void moveBall(SDL_ball *ball)
{
 	int cas0[6] = {0, 0, 0, 0, 0, 0};
 	int cas1[6] = {0, 0, 1, 0, 0, 0};
 	int cas2[6] = {0, 1, 0, 0, 1, 0};
 	int cas3[6] = {0, 1, 1, 0, 1, 0};
 	int cas4[6] = {0, 1, 1, 1, 1, 0};
 	int cas5[6] = {1, 1, 1, 1, 1, 0};
 	int cas6[6] = {1, 1, 1, 1, 1, 1};
 	int new_etat = 0;

 	switch(ball->etat)
 	{
 		case 0:
 			new_etat = lookEtapeFromEtat(cas0, ball->etape);
 			modifBall(ball, new_etat);
 			break;
 		case 1:
 			new_etat = lookEtapeFromEtat(cas1, ball->etape);
 			modifBall(ball, new_etat);
 			break;
 		case 2:
 			new_etat = lookEtapeFromEtat(cas2, ball->etape);
 			modifBall(ball, new_etat);
 			break;
 		case 3:
 			new_etat = lookEtapeFromEtat(cas3, ball->etape);
 			modifBall(ball, new_etat);
 			break;
 		case 4:
 			new_etat = lookEtapeFromEtat(cas4, ball->etape);
 			modifBall(ball, new_etat);
 			break;
 		case 5:
 			new_etat = lookEtapeFromEtat(cas5, ball->etape);
 			modifBall(ball, new_etat);
 			break;
 		case 6:
 			new_etat = lookEtapeFromEtat(cas6, ball->etape);
 			modifBall(ball, new_etat);
 			break;
 		default:
 			break;
 	}
}

// affiche la balle 
void printBall(SDL_ball *ball, SDL_return retour)
{
	SDL_Rect rect = {ball->x1, ball->y1, (ball->x2 - ball->x1), (ball->y2 -ball->y1)};

	if(SDL_SetRenderDrawColor(retour.r, 255, 255, 255, SDL_ALPHA_OPAQUE) != 0)
		SDL_Exit_Texture("Impossible de changer la couleur du rendu", retour.r, retour.w, retour.t);

	if(SDL_RenderFillRect(retour.r, &rect) != 0)
		SDL_Exit_Texture("Impossible de mettre le rectangle", retour.r, retour.w, retour.t);
}

// affiche un rectangle
void printWall(SDL_Rect *mur, SDL_return retour)
{
	if(SDL_SetRenderDrawColor(retour.r, 255, 255, 255, SDL_ALPHA_OPAQUE) != 0)
		SDL_Exit_Texture("Impossible de changer la couleur du rendu", retour.r, retour.w, retour.t);

	if(SDL_RenderFillRect(retour.r, mur) != 0)
		SDL_Exit_Texture("Impossible de mettre le rectangle", retour.r, retour.w, retour.t);
}


// regarde si la balle rencontre un objet pour repartir dans la direction opposé
void collitionWall(SDL_ball *ball, SDL_Rect *murG, SDL_Rect *murH, SDL_Rect *murD, SDL_Rect *murB, SDL_Rect *raquetteG, SDL_Rect *raquetteD,
																						int up1, int down1, SDL_Rect *r2, int up2, int down2 ,Score *score)
{
	// le rectangle que forme la balle
	SDL_Rect balle = {ball->x1, ball->y1, (ball->x2 - ball->x1), (ball->y2 - ball->y1)};

	if(rectInRect(&balle, murG))
	{
		if(ball->dir == HAUT_G)
			changeDirectionBall(ball, HAUT_D);
		if(ball->dir == BAS_G)
			changeDirectionBall(ball, BAS_D);
		if(score->ScoreJ1 != 1)
			score->ScoreJ1--;
		else
			score->fin = 1;
		score->change = 1;
	}
	if(rectInRect(&balle, murH))
	{
		if(ball->dir == BAS_G)
			changeDirectionBall(ball, HAUT_G);
		if(ball->dir == BAS_D)
			changeDirectionBall(ball, HAUT_D);
	}
	if(rectInRect(&balle, murD))
	{
		if(ball->dir == HAUT_D)
			changeDirectionBall(ball, HAUT_G);
		if(ball->dir == BAS_D)
			changeDirectionBall(ball, BAS_G);
		if(score->ScoreJ2 != 1)
			score->ScoreJ2--;
		else
			score->fin = 1;
		score->change = 1;
	}
	if(rectInRect(&balle, murB))
	{
		if(ball->dir == HAUT_D)
			changeDirectionBall(ball, BAS_D);
		if(ball->dir == HAUT_G)
			changeDirectionBall(ball, BAS_G);
	}
	// Raquette de Gauche = joueur 1
	if(rectInRect(&balle, raquetteG))
	{
		if(!up1 && !down1)
		{
			if(ball->dir == HAUT_G)
				changeDirectionBall(ball, HAUT_D);
			if(ball->dir == BAS_G)
				changeDirectionBall(ball, BAS_D);
		}
		if(up1 && !down1)
		{
			if(ball->dir == HAUT_G)
				changeDirectionBall(ball, BAS_D);
				changeStateBall(ball, randMinMax(0,6));
			if(ball->dir == BAS_G)
				changeDirectionBall(ball, BAS_D);
				changeStateBall(ball, randMinMax(0,6));
		}
		if(!up1 && down1)
		{
			if(ball->dir == HAUT_G)
				changeDirectionBall(ball, HAUT_D);
				changeStateBall(ball, randMinMax(0,6));
			if(ball->dir == BAS_G)
				changeDirectionBall(ball, HAUT_D);
				changeStateBall(ball, randMinMax(0,6));
		}
		
	}
	// Raquette de Droite = joueur 2
	if(rectInRect(&balle, raquetteD))
	{
		if(!up2 && !down2)
		{
			if(ball->dir == HAUT_D)
				changeDirectionBall(ball, HAUT_G);
			if(ball->dir == BAS_D)
				changeDirectionBall(ball, BAS_G);
		}
		if(up2 && !down2)
		{
			if(ball->dir == HAUT_D)
				changeDirectionBall(ball, BAS_G);
				changeStateBall(ball, randMinMax(0,6));
			if(ball->dir == BAS_D)
				changeDirectionBall(ball, BAS_G);
				changeStateBall(ball, randMinMax(0,6));
		}
		if(!up2 && down2)
		{
			if(ball->dir == HAUT_D)
				changeDirectionBall(ball, HAUT_G);
				changeStateBall(ball, randMinMax(0,6));
			if(ball->dir == BAS_D)
				changeDirectionBall(ball, HAUT_G);
				changeStateBall(ball, randMinMax(0,6));
		}
	}
}

// fait bouger les raquettes selon les touche pressée
void moveRaquettes(SDL_Rect *r1, int up1, int down1, SDL_Rect *r2, int up2, int down2)
{
	int deplacement = (1000 / FPS) / VITESSE_RAQUETTES;
	if((!up1 && down1) && (r1->y + r1->h) < WINDOW_HEIGHT)
			r1->y += deplacement;
	if((up1 && !down1) && r1->y > FIELD_Y)
			r1->y -= deplacement;

	if((!up2 && down2) && (r2->y + r2->h) < WINDOW_HEIGHT)
			r2->y += deplacement;
	if((up2 && !down2) && r2->y > FIELD_Y)
			r2->y -= deplacement;
}

// limite le nombre de FPS selon les FPS voulu
void limitFps(unsigned int valeurAvantLaBouclePlusLimit)
{
	unsigned int currentTime = SDL_GetTicks();
	if(valeurAvantLaBouclePlusLimit < currentTime)
		return;
	else if(valeurAvantLaBouclePlusLimit > currentTime)
		SDL_Delay(FPS_LIMIT);
	else
		SDL_Delay(valeurAvantLaBouclePlusLimit - currentTime);
}

// choisi un nombre entre min et max
int randMinMax(int min, int max)
{
	srand(SDL_GetTicks());
    return (rand() % (max - min + 1)) + min;
}

// redonne une chaine de caractère avec le debut + un char et .bmp à la fin => "debutchar.bmp"
void texteAuFormatBMP(char debut[], int nombre, char varStock[])
{
	char bmp[5] = ".bmp";
	char newNumber[2];
	sprintf(newNumber, "%d", nombre);
	strcat(varStock, debut);
	strcat(varStock,newNumber);
	strcat(varStock, bmp);
}

// affiche les scores des deux joueurs
void printScore(SDL_return retour, Score *score)
{
	SDL_Rect ScoreJ1;
	ScoreJ1.x = WINDOW_WIDTH / 10;
	ScoreJ1.y = 40;

	SDL_Rect ScoreJ2;
	ScoreJ2.x = (WINDOW_WIDTH / 10) * 9;
	ScoreJ2.y = 40;
	if(score->ScoreJ1 < 1 || score->ScoreJ1 > 6 || score->ScoreJ2 < 1 || score->ScoreJ2 > 6)
		return;
	else
	{
		char debut[] = "src/number/number_";
		//Score joueur 1 :
		char varStock[256] = "";
		texteAuFormatBMP(debut, score->ScoreJ1, varStock);
		printImage(&retour, varStock, &ScoreJ1);
		//Score joueur 2 :
		strcpy(varStock, "");
		texteAuFormatBMP(debut, score->ScoreJ2, varStock);
		printImage(&retour, varStock, &ScoreJ2);
		score->change = 0;
	}
}