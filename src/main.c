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
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	win = SDL_CreateWindow("g7 game",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_ALLOW_HIGHDPI);
	glContext = SDL_GL_CreateContext(win);
	// SDL_GetWindowSize(win, &win_width, &win_height);
	SDL_GL_GetDrawableSize(win, &win_width, &win_height);

	//TODO
	//SDL_SetWindowIcon

	//if fullscreen
	// SDL_DisplayMode modein;
	// SDL_GetDesktopDisplayMode(0, &modein);
	// SDL_SetWindowSize(win, modein.w, modein.h);
	// SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN);

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


	int game_params = main_menu_stageloop(win, ctx, win_width, win_height);

	#ifdef DEBUG
	printf("exited stageloop\n");
	#endif

	if (game_params == 0 )
		goto cleanup;



	if (game_params & G7_PARAM_FULLSCREEN)
	{
		printf("fullscreen\n");
	}

	if (game_params & G7_PARAM_NEWGAME )
	{
		printf("newgame\n");
	}

	if (game_params & G7_PARAM_LOADGAME )
	{
		printf("loadgame\n");
		int map_choice = load_dialog_stageloop(win, ctx, win_width, win_height);
		printf("chosen map: %s\n", maps[map_choice] );
	}

	gameplay_stageloop(win, ctx, win_width, win_height);
	

cleanup:
	nk_sdl_shutdown();
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}

