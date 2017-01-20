struct nk_context* G7_nk_sdl_setup(SDL_Window *win);
struct nk_context* G7_nk_sdl_reset(SDL_Window *win);
void G7_SDL_go_fullscreen(SDL_Window *win);

typedef struct 
{
	SDL_Window *win;
	struct nk_context *ctx;
	int flags;
}
G7_stage;
