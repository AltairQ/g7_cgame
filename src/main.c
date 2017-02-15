#define NK_IMPLEMENTATION
#define NK_SDL_GL2_IMPLEMENTATION


#include "g7_common.h"

int main(int argc, char* argv[])
{
	/* Platform */
	SDL_Window *win;
	SDL_GLContext glContext;
	int win_width, win_height;

	/* GUI */
	struct nk_context *ctx;

	/* SDL setup */
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
	glContext = SDL_GL_CreateContext(win);
	// SDL_GetWindowSize(win, &win_width, &win_height);
	SDL_GL_GetDrawableSize(win, &win_width, &win_height);

	//TODO
	//SDL_SetWindowIcon



	ctx = nk_sdl_init(win);

	{	
	struct nk_font_atlas *atlas;

	nk_sdl_font_stash_begin(&atlas);
	// struct nk_font *rr = nk_font_atlas_add_from_file(atlas, "./font/Road_Rage.ttf", 50, 0);
	struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "./font/Droid_Sans_Mono.ttf", 23, 0);	
	nk_sdl_font_stash_end();
	// nk_style_load_all_cursors(ctx, atlas->cursors);
	nk_style_set_font(ctx, &droid->handle);
	}

	G7_stage game_stage;
	game_stage.win = win;
	game_stage.ctx = ctx;
	game_stage.flags = 0;

	game_stage.flags = main_menu_stageloop(&game_stage);

	assert(G7_PARAM_HOST || G7_PARAM_CLIENT);

	#ifdef DEBUG
	printf("exited stageloop\n");
	#endif

	if (game_stage.flags == 0 )
		goto cleanup;


	#ifdef DEBUG

	if (game_stage.flags & G7_PARAM_FULLSCREEN)
	{
		printf("fullscreen\n");
	}

	#endif

	int map_choice=-3;

	if (game_stage.flags & G7_PARAM_NEWGAME )
	{
		printf("newgame\n");
		map_choice = new_game_stageloop(&game_stage);
		printf("chosen map (%d): %s\n", map_choice, maps[map_choice] );
	}

	

	if (game_stage.flags & G7_PARAM_LOADGAME )
	{
		map_choice = load_dialog_stageloop(&game_stage);
		printf("chosen save (%d): %s\n", map_choice, maps[map_choice] );
	}

	if (map_choice == 0 || map_choice == -1)
	{
		goto cleanup;
	}

	if (map_choice > 0)
	{
		char buffname[256]="";

		if (game_stage.flags & G7_PARAM_NEWGAME)
		{
			strcat(buffname, "maps/");
		}

		if(game_stage.flags & G7_PARAM_LOADGAME)
		{
			strcat(buffname, "saves/");
		}

		strcat(buffname, maps[map_choice]);

		puts(buffname);

		printf("init_gamestate_from_file: %d \n", init_gamestate_from_file(buffname, game_stage.flags & G7_PARAM_LOADGAME));
	}

	#ifdef DEBUG
	for (int i = 0; i < game_state.map.size.y; ++i)
	{
		for (int ii = 0; ii < game_state.map.size.x; ++ii)
		{
			putchar(game_state.map.tab[i][ii] + '0');
		}
		puts("");
	}
	#endif


	game_stage.ctx = G7_nk_sdl_reset(game_stage.win);

	if (connect_dialog_stageloop(&game_stage) == -1)
		goto cleanup;



	gameplay_stageloop(&game_stage);
	

cleanup:
	nk_sdl_shutdown();
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}

