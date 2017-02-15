#include "g7_common.h"

int end_game_stageloop(G7_stage *stage)
{
	
	int running = 1;

	struct nk_color background = nk_rgb(28,48,62);

	int win_width;
	int win_height;
	SDL_SetWindowSize(stage->win, 300, 200);
	stage->ctx = G7_nk_sdl_reset(stage->win);
	SDL_GetWindowSize(stage->win, &win_width, &win_height);



	SDL_Event evt;
	while (running && SDL_WaitEvent(&evt))
	{
		
		nk_input_begin(stage->ctx);

		do
		{
			if (evt.type == SDL_QUIT) running= 0;
			nk_sdl_handle_event(&evt);
		}
		while (SDL_PollEvent(&evt));

		nk_input_end(stage->ctx);

		if (nk_begin(stage->ctx, "End game", nk_rect(0, 0, (float)win_width, (float)win_height), //NK_WINDOW_MOVABLE| NK_WINDOW_SCALABLE|NK_WINDOW_MINIMIZABLE|
		   
			0))
		{
		

			nk_layout_row_dynamic(stage->ctx, 80, 1);

			switch(stage->flags)
			{
				case 1:
					nk_label(stage->ctx, "You lost!", NK_TEXT_CENTERED);
				break;

				case 2:
					nk_label(stage->ctx, "You're winner!", NK_TEXT_CENTERED);
				break;

				case 3:
					nk_label(stage->ctx, "Draw!", NK_TEXT_CENTERED);
				break;
			}


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

	}

	return	0;
}