// Mateusz Maciejewski
// 15.02.2017
// new_game_dialog.c

//handles the new game dialog

#include "g7_common.h"

//array in which we store maps' filenames
char *maps[256];
//map count (+1)
size_t map_count=1;


//main thread
int new_game_stageloop(g7_stage *stage)
{

	//using dirent search for all maps in ./maps/ directory
	{
		DIR *dir;
		struct dirent *ent;
		//open directory
		if ((dir = opendir ("./maps/")) != NULL) {

			while ((ent = readdir (dir)) != NULL && map_count < 256)
			{
				//ignore the . and .. special files
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

	//running flag
	int running = 1;
	struct nk_color background = nk_rgb(28,48,62);

	//here we store the choice
	int option_mapchoice = 0;
	int win_height, win_width;
	SDL_GetWindowSize(stage->win, &win_width, &win_height);

	SDL_Event evt;
	while (running && SDL_WaitEvent(&evt))
	{
		
		nk_input_begin(stage->ctx);

		do
		{
			if (evt.type == SDL_QUIT) running = 0;
			nk_sdl_handle_event(&evt);
		}
		while (SDL_PollEvent(&evt));

		nk_input_end(stage->ctx);

		if (nk_begin(stage->ctx, "Choose map", nk_rect(0, 0, (float)win_width, (float)win_height), //NK_WINDOW_MOVABLE| NK_WINDOW_SCALABLE|NK_WINDOW_MINIMIZABLE|
		   
			NK_WINDOW_TITLE))
		{
			nk_layout_row_dynamic(stage->ctx, (float)win_height - 100.0f, 1);

			//list all maps that we found
			nk_group_begin(stage->ctx, "", 0 );
			{
				nk_layout_row_dynamic(stage->ctx, 30, 1);
				for (size_t i = 1; i < map_count; ++i)
					//if clicked
					if(nk_button_label(stage->ctx, maps[i]))
					{
						//then set the choice
						option_mapchoice = (int)i;
					}

				//if we found no maps
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

	return 0;
}
