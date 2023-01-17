#include <SDL.h>
#include <stdio.h>
#include "fonction.h"
#include <string.h>
#include <stdlib.h>

// C:\Users\Léo\Documents\Sous dossier bordel\code c\SDL action
// gcc src/*.c -o pong -I include -L lib -lmingw32 -lSDL2main -lSDL2
// -mwindows pour kick la console

int main(int argc, char **argv)
{	
	SDL_return retour = SDL_InitFenetre();

	/*-------------------------PROG-------------------------*/

	SDL_bool program_launched = SDL_TRUE;
	SDL_bool accueilOrGame = SDL_TRUE; // si TRUE = accueil, si FALSE = game
	int joueur1UP = 0;
	int joueur1DOWN = 0;
	int joueur2UP = 0;
	int joueur2DOWN = 0;
	unsigned int frameLimit = 0;

	SDL_Rect rectPlay;
	rectPlay.x = (WINDOW_WIDTH - 497) / 2;
	rectPlay.y = (WINDOW_HEIGHT - 161) / 2;

	SDL_Rect rectPong;
	rectPong.x = 0;
	rectPong.y = 0;

	while(program_launched)
	{
		SDL_Event event;
		SDL_Rect rect = {(WINDOW_WIDTH - 497) / 2, (WINDOW_HEIGHT - 161) / 2, 497, 161};

		if(SDL_SetRenderDrawColor(retour.r, 0, 0, 0, SDL_ALPHA_OPAQUE) != 0) // noir
			SDL_Exit_Texture("Impossible de changer la couleur du rendu dans main", retour.r, retour.w, retour.t);
		if(SDL_RenderClear(retour.r) != 0)
			SDL_Exit_Texture("Impossible de faire le clear du rendu dans main", retour.r, retour.w, retour.t);

		printImage(&retour, "src/Play.bmp", &rectPlay);

		printImage(&retour, "src/PONG.bmp", &rectPong);

		//la boucle de l'accueil
		while((program_launched == SDL_TRUE) && (accueilOrGame == SDL_TRUE))
		{
			// limitation des fps
			limitFps(frameLimit);
			frameLimit = SDL_GetTicks() + FPS_LIMIT;

			while(SDL_PollEvent(&event))
			{
				switch(event.type)
				{
					case SDL_MOUSEBUTTONDOWN:
						if(event.button.button == SDL_BUTTON_LEFT) 
						{
							if(inRect(event.button.x, event.button.y, &rect))
							{
								accueilOrGame = SDL_FALSE;
							}
						}
						break;

					case SDL_QUIT:
						program_launched = SDL_FALSE;
						break;

					default:
						break;
				}
			}
		}

		if(SDL_RenderClear(retour.r) != 0)
			SDL_Exit_Texture("Impossible de faire le clear du rendu", retour.r, retour.w, retour.t);

		// Creation de la balle
		SDL_ball ball = initBall();

		// Creation des murs
		SDL_Rect murG = {-20, FIELD_Y, 20, FIELD_HEIGHT};
		SDL_Rect murH = {0, 0, FIELD_WIDTH, FIELD_Y};
		SDL_Rect murD = {FIELD_WIDTH, FIELD_Y, 20, FIELD_HEIGHT};
		SDL_Rect murB = {0, FIELD_HEIGHT + FIELD_Y, FIELD_WIDTH ,20};

		SDL_Rect raquette1 = {WINDOW_WIDTH / 10,       (WINDOW_HEIGHT / 2) + (FIELD_Y / 2) - (RAQUETTE_HEIGHT / 2), RAQUETTE_WIDTH ,RAQUETTE_HEIGHT};
		SDL_Rect raquette2 = {(WINDOW_WIDTH / 10) * 9, (WINDOW_HEIGHT / 2) +  (FIELD_Y / 2) -(RAQUETTE_HEIGHT / 2), RAQUETTE_WIDTH ,RAQUETTE_HEIGHT};

		SDL_Rect rectPong = {((WINDOW_WIDTH - 1280) / 2), 0};
		printImage(&retour, "src/PONG.bmp", &rectPong);

		//Score des joueurs
		Score score;
		score.ScoreJ1 = 6;
		score.ScoreJ2 = 6;
		score.change = 0;
		score.fin = 0;

		printScore(retour, &score);

		//field
		SDL_Rect rectField = {0, FIELD_Y, FIELD_WIDTH, FIELD_HEIGHT};

		// la boucle du jeu
		while((program_launched == SDL_TRUE) && (accueilOrGame == SDL_FALSE))
		{
			// limitation des fps
			limitFps(frameLimit);
			frameLimit = SDL_GetTicks() + FPS_LIMIT;

			while(SDL_PollEvent(&event))
			{
				switch(event.type)
				{
					case SDL_KEYDOWN:
						if(event.key.keysym.sym == SDLK_z)
							joueur1UP = 1;
						if(event.key.keysym.sym == SDLK_s)
							joueur1DOWN = 1;
						if(event.key.keysym.sym == SDLK_UP)
							joueur2UP = 1;
						if(event.key.keysym.sym == SDLK_DOWN)
							joueur2DOWN = 1;
						continue;

					case SDL_KEYUP:
						if(event.key.keysym.sym == SDLK_z)
							joueur1UP = 0;
						if(event.key.keysym.sym == SDLK_s)
							joueur1DOWN = 0;
						if(event.key.keysym.sym == SDLK_UP)
							joueur2UP = 0;
						if(event.key.keysym.sym == SDLK_DOWN)
							joueur2DOWN = 0;
						continue;

					case SDL_QUIT:
						program_launched = SDL_FALSE;
						break;

					default:
						break;
				}
			}
			SDL_RenderClearRect(&rectField, retour);

			if(score.change == 0)
			{
			moveBall(&ball);
			printBall(&ball, retour);
			printWall(&raquette1, retour);
			printWall(&raquette2, retour);

			SDL_RenderPresent(retour.r);

			moveRaquettes(&raquette1, joueur1UP, joueur1DOWN, &raquette2, joueur2UP, joueur2DOWN);

			collitionWall(&ball, &murG, &murH, &murD, &murB, &raquette1, &raquette2, joueur1UP, joueur1DOWN, &raquette2, joueur2UP, joueur2DOWN, &score);
			}
			else
			{
				printScore(retour, &score);
				ball = initBall();
				raquette1.x = WINDOW_WIDTH / 10;
				raquette1.y = (WINDOW_HEIGHT / 2) + (FIELD_Y / 2) - (RAQUETTE_HEIGHT / 2);
				raquette1.w = RAQUETTE_WIDTH;
				raquette1.h = RAQUETTE_HEIGHT;
			    raquette2.x = (WINDOW_WIDTH / 10) * 9;
			    raquette2.y = (WINDOW_HEIGHT / 2) + (FIELD_Y / 2) - (RAQUETTE_HEIGHT / 2);
			    raquette2.w =  RAQUETTE_WIDTH;
			    raquette2.h = RAQUETTE_HEIGHT;
				printBall(&ball, retour);
				printWall(&raquette1, retour);
				printWall(&raquette2, retour);

				SDL_RenderPresent(retour.r);

				SDL_Delay(2000);
			}
			if(score.fin == 1)
				accueilOrGame = SDL_TRUE;
		}
	}

	SDL_DestroyTexture(retour.t);
	SDL_DestroyRenderer(retour.r);
	SDL_DestroyWindow(retour.w);
	SDL_Quit();

	return EXIT_SUCCESS; // 0
}
