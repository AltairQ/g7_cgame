// Mateusz Maciejewski
// 15.02.2017
// engine.h

#include "g7_common.h"

//contains snippet functions
//essentially wrapper functions



//setup nuklear from given SDL_Window
struct nk_context* g7_nk_sdl_setup(SDL_Window *win)
{
	struct nk_context *ctx  = nk_sdl_init(win);
	{	
	struct nk_font_atlas *atlas;

	nk_sdl_font_stash_begin(&atlas);
	// struct nk_font *rr = nk_font_atlas_add_from_file(atlas, "./font/Road_Rage.ttf", 50, 0);
	struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "./font/Droid_Sans_Mono.ttf", 23, 0);	
	nk_sdl_font_stash_end();
	// nk_style_load_all_cursors(ctx, atlas->cursors);
	nk_style_set_font(ctx, &droid->handle);
	}

	return ctx;
}

//reset nuklear context
struct nk_context* g7_nk_sdl_reset(SDL_Window *win)
{
	nk_sdl_shutdown();
	return g7_nk_sdl_setup(win);

}

//go fullscreen
void g7_SDL_go_fullscreen(SDL_Window *win)
{
	SDL_DisplayMode modein;
	SDL_GetDesktopDisplayMode(0, &modein);
	SDL_SetWindowSize(win, modein.w, modein.h);
	SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN);
}
