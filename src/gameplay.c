#include "g7_common.h"


void DrawCircle(float cx, float cy, float r, int num_segments)
{
	glColor4f(0.2f, 0.7f, 0.0f, 0.7f);
	glBegin(GL_LINE_LOOP);
	for(int ii = 0; ii < num_segments; ii++)
	{
		float theta = 2.0f * 3.1415926f * (float)(ii) / (float)(num_segments);//get the current angle

		float x = r * cosf(theta);//calculate the x component
		float y = r * sinf(theta);//calculate the y component

		glVertex2f(x + cx, y + cy);//output vertex

	}
	glEnd();
}

void g7_draw_tile(int y, int x, g7_tile tile, bool focus)
{
	switch(tile)
	{
		case blank:
			glColor4f(1.0f, 1.0f, 1.0f, 0.1f + ((focus)?0.3f:0.0f)  );
		break;

		case block:
			glColor4f(0.0f, 0.0f, 0.0f, 0.7f + ((focus)?0.3f:0.0f)  );
		break;

		case finish:
			glColor4f(0.0f, 1.0f, 0.0f, 0.7f + ((focus)?0.3f:0.0f)  );
		break;

		default:
			glColor4f(1.0f, 0.0f, 0.0f, 0.7f + ((focus)?0.3f:0.0f)  );
		break;
	}

	

	float x1, x2, y1, y2;
	x1 = x*50;
	x2 = (x+1)*50;
	y1 = y*50;
	y2 = (y+1)*50;
	float x_offset = 200;

	glBegin(GL_QUADS);
	glVertex2f(x_offset + x1, y1);
	glVertex2f(x_offset + x2, y1);
	glVertex2f(x_offset + x2, y2);
	glVertex2f(x_offset + x1, y2);
	glEnd();
}


g7_vertex chosen_tile;
g7_vertex him;

void g7_draw_map(g7_vertex mouse)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (int i = 0; i < game_state.map.size.y; ++i)
	{
		for (int ii = 0; ii < game_state.map.size.x; ++ii)
		{
			g7_vertex buf = screen_to_tile(mouse);
			g7_vertex buf2= screen_to_tile(him);

			g7_draw_tile(i, ii, game_state.map.tab[i][ii], (buf.x == ii && buf.y == i) || (ii == chosen_tile.x && i == chosen_tile.y ) || (ii == buf2.x && i == buf2.y ));			
		}
	}
}


void g7_event_filter(SDL_Event *event, g7_vertex *mouse)
{
	if(event->type == SDL_MOUSEBUTTONDOWN)
	{
		chosen_tile = screen_to_tile(*mouse);
	}

	if (event->type == SDL_USEREVENT)
	{
		return;		
	}

	nk_sdl_handle_event(event);	
	
}

bool bumpe;
static PipesPtr potoki;



Uint32 my_callbackfunc(Uint32 interval, void *param)
{

	g7_vertex mouse;
	SDL_GetMouseState(&mouse.x, &mouse.y);

	char tmp[1024];
	sprintf(tmp, "%d %d\n", mouse.x, mouse.y);
	sendStringToPipe(potoki, tmp);

	if(getStringFromPipe(potoki, tmp, 1024))
	{
		int xtmp, ytmp;
		if(sscanf(tmp, "%d %d ", &xtmp, &ytmp) == 2)
		{
			him.x = xtmp;
			him.y = ytmp;
		}

	}


	SDL_Event event;
	SDL_UserEvent userevent;

	/* In this example, our callback pushes an SDL_USEREVENT event
	into the queue, and causes our callback to be called again at the
	same interval: */

	userevent.type = SDL_USEREVENT;
	userevent.code = 0;
	userevent.data1 = NULL;
	userevent.data2 = NULL;

	event.type = SDL_USEREVENT;
	event.user = userevent;

	SDL_PushEvent(&event);
	// return(interval);

	puts("bumpe");
	return(interval);
}




int gameplay_stageloop(G7_stage *stage)
{
	// if((potoki=initPipes(stage->flags & G7_PARAM_HOST)) == NULL)
	// 	return -1;


	// SDL_AddTimer(1000, my_callbackfunc, NULL);

	//DOES NOT WORK!!!!!!!!!

	chosen_tile.x = -1;
	chosen_tile.y = -1;

	int win_width = 1024;
	int win_height = 768;
	if(stage->flags & G7_PARAM_FULLSCREEN)
		G7_SDL_go_fullscreen(stage->win);
	else
		SDL_SetWindowSize(stage->win, win_width, win_height);

	stage->ctx = G7_nk_sdl_reset(stage->win);
	SDL_GetWindowSize(stage->win, &win_width, &win_height);


	int running = 1;
	struct nk_color background = nk_rgb(28,48,62);

	float x = 340;
	float y = 120;
	float z = 100;

	g7_vertex mouse;
	mouse.x = 0;
	mouse.y = 0;

	SDL_Event evt;
	while (running && SDL_WaitEvent(&evt))
	{
		
		nk_input_begin(stage->ctx);

		do
		{
			if (evt.type == SDL_QUIT) return 0;
			g7_event_filter(&evt, &mouse);
			
		}
		while (SDL_PollEvent(&evt));

		nk_input_end(stage->ctx);
		SDL_GetMouseState(&mouse.x, &mouse.y);

		if (nk_begin(stage->ctx, "Menu", nk_rect(0, 0, 120, win_height), //NK_WINDOW_MOVABLE| NK_WINDOW_SCALABLE|NK_WINDOW_MINIMIZABLE|
		   
			NK_WINDOW_TITLE))
		{

			nk_layout_row_dynamic(stage->ctx, 30, 1);

			if (nk_button_label(stage->ctx, "Up"))
			{
				y-=10.0f;
				printf("y: %f\n", y );
			}
			if (nk_button_label(stage->ctx, "Left"))
			{
				x-=10.f;
				printf("x: %f\n", x );
			}
			if (nk_button_label(stage->ctx, "Rad+"))
			{
				z+=10.0f;
				printf("z: %f\n", z );
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

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(0, 0, win_width, win_height);
		glOrtho(0.0f, win_width, win_height, 0.0f, -1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// for (int i = 0; i < 10; ++i)
			// DrawCircle(x, y, z, 10000);

		g7_draw_map(mouse);
	
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
	return 0;
}