// Mateusz Maciejewski
// 15.02.2017
// main.c

//the real main file
//handles the SDL+OpenGL init
//changes the stages


//this is so the nuklear library is compiled
//as part of main.c file
#define NK_IMPLEMENTATION
#define NK_SDL_GL2_IMPLEMENTATION


#include "g7_common.h"

//the argc and argv arguments are mandatory
//because on windows the SDLmain library is mandatory
//and it needs this exact int main prototype
int main(int argc, char* argv[])
{
	/* Platform */
	SDL_Window *win;
	SDL_GLContext glContext;
	int win_width, win_height;



	/* SDL setup */
	//pretty much self-explanatory
	SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	win = SDL_CreateWindow("g7 game",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_ALLOW_HIGHDPI|SDL_WINDOW_RESIZABLE);
	//here the opengl context is created
	glContext = SDL_GL_CreateContext(win);

	SDL_GL_GetDrawableSize(win, &win_width, &win_height);



	/* GUI */
	struct nk_context *ctx = g7_nk_sdl_setup(win);

	//the main stage
	//this is used to pass SDL + nuklear to 
	//"stageloops" functions
	g7_stage game_stage;
	game_stage.win = win;
	game_stage.ctx = ctx;
	game_stage.flags = 0;

	//show the main menu
	game_stage.flags = main_menu_stageloop(&game_stage);

	assert(G7_PARAM_HOST || G7_PARAM_CLIENT);

	#ifdef DEBUG
	printf("exited stageloop\n");
	#endif

	//if the user chose to abort
	if (game_stage.flags == 0 )
		goto cleanup;


	//choice flag
	int map_choice=-3;

	//if newgame
	if (game_stage.flags & G7_PARAM_NEWGAME )
	{
		// printf("newgame\n");
		map_choice = new_game_stageloop(&game_stage);
		// printf("chosen map (%d): %s\n", map_choice, maps[map_choice] );
	}

	
	//if loadgame
	if (game_stage.flags & G7_PARAM_LOADGAME )
	{
		map_choice = load_dialog_stageloop(&game_stage);
		printf("chosen save (%d): %s\n", map_choice, maps[map_choice] );
	}

	//if something went wrong/user cancelled
	if (map_choice == 0 || map_choice == -1)
	{
		goto cleanup;
	}

	//if we have a choice
	if (map_choice > 0)
	{
		char buffname[256]="";

		//set the directory
		//if it's a new game
		if (game_stage.flags & G7_PARAM_NEWGAME)
		{
			//then it's a map
			strcpy(buffname, "./maps/");
		}

		//if we load a save
		if(game_stage.flags & G7_PARAM_LOADGAME)
		{
			//then it's a save
			strcpy(buffname, "./saves/");
		}

		//add the filename
		strcat(buffname, maps[map_choice]);

		// puts(buffname);

		//load gamestate from file
		g7_init_gamestate_from_file(buffname, game_stage.flags & G7_PARAM_LOADGAME);
		// printf("init_gamestate_from_file: %d \n", init_gamestate_from_file(buffname, game_stage.flags & G7_PARAM_LOADGAME));
	}

	//reset the nuklear
	game_stage.ctx = g7_nk_sdl_reset(game_stage.win);

	//connect to the peer
	if (connect_dialog_stageloop(&game_stage) == -1)
		goto cleanup;


	//game result
	int gameres;
	//play the game
	//if it wasn't aborted
	if((gameres = gameplay_stageloop(&game_stage)) > 0)
	{
		//show the result
		game_stage.flags = gameres;
		end_game_stageloop(&game_stage);
	}
	

//shut down libraries and stuff
//then exit
cleanup:
	nk_sdl_shutdown();
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}

