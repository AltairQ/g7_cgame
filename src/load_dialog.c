#include "g7_common.h"

char *maps[256];
size_t map_count=1;

int load_dialog_stageloop(G7_stage *stage)
{
	{
		DIR *dir;
		struct dirent *ent;
		if ((dir = opendir ("./out/maps/")) != NULL) {

			while ((ent = readdir (dir)) != NULL && map_count < 256)
			{
				if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0 )
					continue;

				maps[map_count] = malloc(strlen(ent->d_name) + 1);
				strcpy(maps[map_count], ent->d_name);
				map_count++;
				printf("%s\n", ent->d_name);
			}			
	
			closedir (dir);
		} else {
			/* could not open directory */
			perror ("");
			return -1;
		}

	}

	int running = 1;
	struct nk_color background = nk_rgb(28,48,62);
	static int property = 5;
	int option_fullscreen = 0;
	bool action_newgame = false;
	bool action_loadgame = false;

	int option_mapchoice = 0;
	int win_height, win_width;
	SDL_GetWindowSize(stage->win, &win_width, &win_height);

	SDL_Event evt;
	while (running && SDL_WaitEvent(&evt))
	{
		
		nk_input_begin(stage->ctx);

		do
		{
			if (evt.type == SDL_QUIT) return 0;
			nk_sdl_handle_event(&evt);
		}
		while (SDL_PollEvent(&evt));

		nk_input_end(stage->ctx);

		if (nk_begin(stage->ctx, "Choose map", nk_rect(0, 0, win_width, win_height), //NK_WINDOW_MOVABLE| NK_WINDOW_SCALABLE|NK_WINDOW_MINIMIZABLE|
		   
			NK_WINDOW_TITLE))
		{
			nk_layout_row_dynamic(stage->ctx, win_height - 100, 1);

			nk_group_begin(stage->ctx, "", 0 );
			{
				nk_layout_row_dynamic(stage->ctx, 30, 1);
				for (int i = 1; i < map_count; ++i)
					if(nk_button_label(stage->ctx, maps[i]))
					{
						option_mapchoice = i;
					}

				if (map_count == 1)
				{
					nk_label(stage->ctx, "(no maps found)", NK_TEXT_CENTERED);
				}

			}

			nk_group_end(stage->ctx);
			nk_layout_row_dynamic(stage->ctx, 30, 1);


			if (nk_button_label(stage->ctx, "Cancel"))
			{
				return 0;
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

		if (option_mapchoice != 0)		
			return option_mapchoice;		
		


	}
}