// Mateusz Maciejewski
// 15.02.2017
// utils.h

//contains snippet functions
//essentially wrapper functions

//setup nuklear from given SDL_Window
struct nk_context* g7_nk_sdl_setup(SDL_Window *win);

//reset nuklear context
struct nk_context* g7_nk_sdl_reset(SDL_Window *win);

//go fullscreen
void g7_SDL_go_fullscreen(SDL_Window *win);

//the sdl + nuklear + flags wrapper
typedef struct 
{
	SDL_Window *win;
	struct nk_context *ctx;
	int flags;
}
g7_stage;
