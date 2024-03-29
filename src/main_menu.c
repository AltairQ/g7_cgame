// Mateusz Maciejewski
// 15.02.2017
// main_menu.h


//main menu dialog
//only purpose is to let the user
//choose the gamemode, then pass the flag

#include "g7_common.h"

int main_menu_stageloop(g7_stage *stage)
{

	//self-explanatory
	
	int running = 1;
	struct nk_color background = nk_rgb(28,48,62);
	int option_fullscreen = 0;
	int option_host = 0;
	bool action_newgame = false;
	bool action_loadgame = false;
	bool action_joingame = false;

	int win_width;
	int win_height;
	SDL_GetWindowSize(stage->win, &win_width, &win_height);

	SDL_Event evt;
	while (running && SDL_WaitEvent(&evt))
	{
		
		nk_input_begin(stage->ctx);

		do
		{
			if (evt.type == SDL_QUIT) running=0;
			nk_sdl_handle_event(&evt);
		}
		while (SDL_PollEvent(&evt));

		nk_input_end(stage->ctx);

		if (nk_begin(stage->ctx, "Main menu", nk_rect(0, 0, (float)win_width, (float)win_height), //NK_WINDOW_MOVABLE| NK_WINDOW_SCALABLE|NK_WINDOW_MINIMIZABLE|
		   
			0))
		{
			enum {EASY, HARD};
		

			nk_layout_row_dynamic(stage->ctx, 80, 1);
			nk_label(stage->ctx, "MAIN MENU", NK_TEXT_CENTERED);
			// if (nk_button_label(stage->ctx, "button"))
			nk_layout_row_dynamic(stage->ctx, 25, 1);

			if (nk_button_label(stage->ctx, "New Game"))
			{
				option_host = G7_PARAM_HOST;
				action_newgame = true;				
			}

			if (nk_button_label(stage->ctx, "Load Game"))
			{
				option_host = G7_PARAM_HOST;
				action_loadgame = true;				
			}

			if (nk_button_label(stage->ctx, "Join Game"))
			{
				action_joingame = true;
			}


			nk_layout_row_dynamic(stage->ctx, 230, 1);
			nk_group_begin(stage->ctx, "Game Settings:", NK_WINDOW_TITLE );

			nk_layout_row_dynamic(stage->ctx, 50, 1);
			if (nk_option_label(stage->ctx, "fullscreen", option_fullscreen == G7_PARAM_FULLSCREEN ))
			{
				option_fullscreen = G7_PARAM_FULLSCREEN;
			}
			else
			{
				option_fullscreen = 0;
			}


			nk_group_end(stage->ctx);
			nk_layout_row_dynamic(stage->ctx, 25, 1);

			if (nk_button_label(stage->ctx, "Exit"))
			{
				running = 0;
			}


		}
		nk_end(stage->ctx);

		//Drawing 
		
		{float bg[4];
		nk_color_fv(bg, background);
		SDL_GetWindowSize(stage->win, &win_width, &win_height);
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
		SDL_GL_SwapWindow(stage->win);
		}

		if(action_loadgame)
			return G7_PARAM_LOADGAME | option_fullscreen | option_host;

		if(action_newgame)
			return G7_PARAM_NEWGAME | option_fullscreen | option_host;

		if(action_joingame)
			return G7_PARAM_CLIENT | option_fullscreen;
	}
	
	return 0;
}