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

		case pB:
			glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
		break;

		case pA:
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
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

void g7_draw_map(g7_vertex mouse)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (int i = 0; i < game_state.map.size.y; ++i)
	{
		for (int ii = 0; ii < game_state.map.size.x; ++ii)
		{
			g7_vertex buf = screen_to_tile(mouse);

			g7_draw_tile(i, ii, game_state.map.tab[i][ii], (buf.x == ii && buf.y == i) || (ii == chosen_tile.x && i == chosen_tile.y ));			
		}
	}
}

void g7_draw_players()
{
	g7_draw_tile(game_state.playerA.pos.y , game_state.playerA.pos.x, pA, false);
	g7_draw_tile(game_state.playerB.pos.y , game_state.playerB.pos.x, pB, false);
}


void g7_event_filter(SDL_Event *event, g7_vertex *mouse)
{
	if(event->type == SDL_MOUSEBUTTONDOWN)
	{
		chosen_tile = screen_to_tile(*mouse);
		if (chosen_tile.x >= game_state.map.size.x || chosen_tile.y >= game_state.map.size.y
			|| chosen_tile.x < 0 || chosen_tile.y < 0)
		{
			chosen_tile.x = -1;
			chosen_tile.y = -1;
		}
		else
		{
			game_state.playerA.pos = chosen_tile;
		}
	}



	if (event->type == SDL_USEREVENT)
	{
		return;		
	}

	nk_sdl_handle_event(event);	
	
}

bool bumpe;


Uint32 net_callback(Uint32 interval, void *param)
{
	char message[1024];

	TCPsocket *client = ((TCPsocket*)param);

	int len;
	int result;

	if(chosen_tile.x != -1)
	{
		sprintf(message, "%d %d ",chosen_tile.x, chosen_tile.y );
	}
	else
	{
		sprintf(message, "KEEP ALIVE");
	}	

	len = strlen(message)+1;


	result=SDLNet_TCP_Send(*client,message,len); /* add 1 for the NULL */
	if(result<len)
		printf("SDLNet_TCP_Send: %s\n",SDLNet_GetError());


	len = SDLNet_TCP_Recv(*client, message, 1024);

	if(!len)
	{
		printf("SDLNet_TCP_Recv: %s\n",SDLNet_GetError());
		return 0;
	}

	printf("Received: %.*s\n",len,message);

	int bx, by;
	if(sscanf(message, "%d %d ", &bx, &by) == 2)
	{
		game_state.playerB.pos.x = bx;
		game_state.playerB.pos.y = by;
		printf("Set player to %d %d\n", bx, by );
	}


	SDL_Event event;
	SDL_UserEvent userevent;

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
	SDLNet_Init();

	TCPsocket client;
	TCPsocket server;
	IPaddress ip;

	if(stage->flags & G7_PARAM_HOST)
	{
		SDLNet_ResolveHost(&ip, NULL, 9999);
		server = SDLNet_TCP_Open(&ip);
		puts("Waiting for a client to connect...");

		while(!(client = SDLNet_TCP_Accept(server)))
		{
			SDL_Delay(100);			
		}
			
	}
	else
	{
		SDLNet_ResolveHost(&ip,"localhost",9999);
		client = SDLNet_TCP_Open(&ip);		
	}

	// SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "G7", "Cośtam", NULL);

	SDL_AddTimer(100, net_callback, &client);

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


		g7_draw_map(mouse);
		g7_draw_players();
	
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