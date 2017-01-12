#include "g7_common.h"

int main_menu_stageloop(SDL_Window *win, struct nk_context *ctx, int win_width, int win_height)
{
	int running = 1;
	struct nk_color background = nk_rgb(28,48,62);
	static int property = 20;
	int option_fullscreen = 0;
	bool action_newgame = false;
	bool action_loadgame = false;

	SDL_Event evt;
	while (running && SDL_WaitEvent(&evt))
	{
		
		nk_input_begin(ctx);

		do
		{
			if (evt.type == SDL_QUIT) return 0;
			nk_sdl_handle_event(&evt);
		}
		while (SDL_PollEvent(&evt));

		nk_input_end(ctx);

		if (nk_begin(ctx, "Main menu", nk_rect(0, 0, win_width, win_height), //NK_WINDOW_MOVABLE| NK_WINDOW_SCALABLE|NK_WINDOW_MINIMIZABLE|
		   
			0))
		{
			enum {EASY, HARD};
			static int op = EASY;
			

			nk_layout_row_dynamic(ctx, 80, 1);
			nk_label(ctx, "MAIN MENU", NK_TEXT_CENTERED);
			// if (nk_button_label(ctx, "button"))
			// 	fprintf(stdout, "button pressed %d\n", property);
			nk_layout_row_dynamic(ctx, 25, 1);

			if (nk_button_label(ctx, "New Game"))
			{
				action_newgame = true;				
			}

			if (nk_button_label(ctx, "Load Game"))
			{
				action_loadgame = true;				
			}


			nk_layout_row_dynamic(ctx, 230, 1);
			nk_group_begin(ctx, "Game Settings", NK_WINDOW_TITLE );

			nk_layout_row_dynamic(ctx, 50, 1);
			if (nk_option_label(ctx, "fullscreen", option_fullscreen == G7_PARAM_FULLSCREEN ))
			{
				option_fullscreen = G7_PARAM_FULLSCREEN;
			}
			else
			{
				option_fullscreen = 0;
			}

			if (nk_option_label(ctx, "hax", false))
			{

			}

			if (nk_option_label(ctx, "whatever", false))
			{

			}



			nk_group_end(ctx);
			nk_layout_row_dynamic(ctx, 25, 1);

			if (nk_button_label(ctx, "Exit"))
			{
				return 0;
			}


		}
		nk_end(ctx);

		//Drawing 
		
		{float bg[4];
		nk_color_fv(bg, background);
		SDL_GetWindowSize(win, &win_width, &win_height);
		glViewport(0, 0, win_width, win_height);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glClearColor(bg[0], bg[1], bg[2], bg[3]);
		//MAY BREAK
		//blending
		//scissor
		//face culling
		//depth test
		//viewport
		nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);
		SDL_GL_SwapWindow(win);
		}

		if(action_loadgame)
			return G7_PARAM_LOADGAME | option_fullscreen;

		if(action_newgame)
			return G7_PARAM_NEWGAME | option_fullscreen;

	}
}