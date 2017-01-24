#include "g7_common.h"

float scale = 1.0f;
float x_offset = 200.0f;
bool host = false;
bool newmove = false;

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
	x1 = x*scale;
	x2 = (x+1)*scale;
	y1 = y*scale;
	y2 = (y+1)*scale;
	

	glBegin(GL_QUADS);
	glVertex2f(x_offset + x1, y1);
	glVertex2f(x_offset + x2, y1);
	glVertex2f(x_offset + x2, y2);
	glVertex2f(x_offset + x1, y2);
	glEnd();
}

g7_vertex chosen_tile;

bool g7_is_accessible(g7_vertex tile)
{
	if(host)
	{
		if (game_state.playerA.vel.x -2 <= tile.x - game_state.playerA.pos.x  && game_state.playerA.vel.x + 2 >= tile.x - game_state.playerA.pos.x &&
		  game_state.playerA.vel.y -2 <= tile.y - game_state.playerA.pos.y  && game_state.playerA.vel.y + 2 >= tile.y - game_state.playerA.pos.y)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (game_state.playerB.vel.x -2 <= tile.x - game_state.playerB.pos.x  && game_state.playerB.vel.x + 2 >= tile.x - game_state.playerB.pos.x &&
		  game_state.playerB.vel.y -2 <= tile.y - game_state.playerB.pos.y  && game_state.playerB.vel.y + 2 >= tile.y - game_state.playerB.pos.y)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}


void g7_draw_map(g7_vertex mouse)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (int i = 0; i < game_state.map.size.y; ++i)
	{
		for (int ii = 0; ii < game_state.map.size.x; ++ii)
		{
			g7_vertex buf = screen_to_tile(mouse);
			g7_vertex me;
			me.x = ii;
			me.y = i;

			g7_draw_tile(i, ii, game_state.map.tab[i][ii], g7_is_accessible(me) || (buf.x == ii && buf.y == i) || (ii == chosen_tile.x && i == chosen_tile.y ));			
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
			if( game_state.map.tab[chosen_tile.y][chosen_tile.x] != block && g7_is_accessible(chosen_tile))
			{

				if(host)
				{
					game_state.playerA.vel.x = chosen_tile.x - game_state.playerA.pos.x;
					game_state.playerA.vel.y = chosen_tile.y - game_state.playerA.pos.y;
					game_state.playerA.pos = chosen_tile;
				}
				else
				{
					game_state.playerB.vel.x = chosen_tile.x - game_state.playerB.pos.x;
					game_state.playerB.vel.y = chosen_tile.y - game_state.playerB.pos.y;
					game_state.playerB.pos = chosen_tile;
				}
	
				newmove = true;
			}
		}
	}


	if (event->type == SDL_USEREVENT)
	{
		return;		
	}

	nk_sdl_handle_event(event);	
	
}



Uint32 net_callback(Uint32 interval, void *param)
{
	char message[1024];

	TCPsocket *client = ((TCPsocket*)param);

	int len;
	int result;

	if(chosen_tile.x != -1 && newmove)
	{
		if(host)
			sprintf(message, "MA %d %d ", game_state.playerA.vel.x, game_state.playerA.vel.y );
		else
			sprintf(message, "MB %d %d ", game_state.playerB.vel.x, game_state.playerB.vel.y );

		newmove = false;
	}
	else
	{
		sprintf(message, "K"); // keep-alive
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

	switch(message[0])
	{
		case 'K':
		//keep-alive, we can easily ignore it
		break;

		case 'Q':
		//peer quitting, gotta quit too
		break;

		case 'M':
		//move info, let's update
		case 'P':
		//position info, let's update

		g7_command_parse(message);

		break;

		case 'W':
		//someone won, game ends
		break;

		default:
		//dunno
		break;

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

	// puts("bumpe");
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
		host = true;
			
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

		if (nk_begin(stage->ctx, "Menu", nk_rect(0, 0, (int)x_offset - 50, win_height), //NK_WINDOW_MOVABLE| NK_WINDOW_SCALABLE|NK_WINDOW_MINIMIZABLE|
		   
			NK_WINDOW_TITLE | NK_WINDOW_BORDER))
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
		scale = MIN((float)(win_width- (int)x_offset)/(float)(game_state.map.size.x),
		(float)(win_height)/(float)(game_state.map.size.y) );

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