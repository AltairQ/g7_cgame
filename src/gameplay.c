#include "g7_common.h"

float scale = 1.0f;
float x_offset = 200.0f;
bool host = false;
bool newmove = false;

bool enemy_chose = false;
bool iwon=false;
bool en_won=false;

void g7_draw_tile(int x, int y, g7_tile tile, bool focus)
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

	float xf = (float)x;
	float yf = (float)y;

	float x1, x2, y1, y2;
	x1 = xf*scale;
	x2 = (xf+1.0f)*scale;
	y1 = yf*scale;
	y2 = (yf+1.0f)*scale;
	

	glBegin(GL_QUADS);
	glVertex2f(x_offset + x1, y1);
	glVertex2f(x_offset + x2, y1);
	glVertex2f(x_offset + x2, y2);
	glVertex2f(x_offset + x1, y2);
	glEnd();
}

g7_vertex chosen_tile;

bool g7_is_accessible(g7_player_state player, g7_vertex tile)
{
	if (player.vel.x -game_state.max_delta <= tile.x - player.pos.x  && player.vel.x + game_state.max_delta >= tile.x - player.pos.x &&
	 player.vel.y -game_state.max_delta <= tile.y - player.pos.y  && player.vel.y + game_state.max_delta >= tile.y - player.pos.y)
	{
		return true;
	}
	else
	{
		return false;
	}

}


void g7_draw_map(g7_vertex mouse)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (int i = 0; i < game_state.map.size.x; ++i)
	{
		for (int ii = 0; ii < game_state.map.size.y; ++ii)
		{
			g7_vertex buf = screen_to_tile(mouse);
			g7_vertex me;
			me.x = i;
			me.y = ii;

			g7_draw_tile(i, ii, game_state.map.tab[i][ii],( g7_path_is_nonblocked(client_state.own_state->pos, me) && g7_is_accessible(*client_state.own_state, me) )|| (buf.x == i && buf.y == ii) || (i == chosen_tile.x && ii == chosen_tile.y ));			
		}
	}
}

void g7_draw_players()
{
	g7_draw_tile(game_state.playerA.pos.x , game_state.playerA.pos.y, pA, false);
	g7_draw_tile(game_state.playerB.pos.x , game_state.playerB.pos.y, pB, false);
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
			if(client_state.new_own_move == false && g7_is_accessible(*client_state.own_state, chosen_tile) && g7_path_is_nonblocked(client_state.own_state->pos, chosen_tile))
			{

				int vx, vy;
				vx = chosen_tile.x - client_state.own_state->pos.x;
				vy = chosen_tile.y - client_state.own_state->pos.y;
				client_state.own_state->vel.x = vx;
				client_state.own_state->vel.y = vy;				
				client_state.own_state->pos = chosen_tile;

				char buf[32]="";
				sprintf(buf, "M%c %d %d", client_state.own_state->prefix, vx, vy);
				strncpy(client_state.own_buf_cmd, buf, 32);
				net_send_buffer(buf, 32);
				// if(game_state.map.tab[])
				client_state.new_own_move = true;
				// g7_check_step();
			}
		}
	}


	if (event->type == SDL_USEREVENT)
		return;		
	

	nk_sdl_handle_event(event);	
	
}

int running = 1;

bool g7_has_moves(g7_player_state player)
{
	for (int ii = 0; ii < game_state.map.size.y; ++ii)
	{
		for (int i = 0; i < game_state.map.size.x; ++i)
		{
			g7_vertex btile;

			btile.x = i;
			btile.y = ii;

			if(g7_is_accessible(player, btile) && g7_path_is_nonblocked(player.pos, btile))
				return true;
		}
	}

	return false;
}

bool g7_on_finish(g7_player_state player)
{
	int x = player.pos.x;
	int y = player.pos.y;

	return (game_state.map.tab[x][y] == finish);
}

void server_check()
{
	if(!running)
		return;

	if (enemy_chose && client_state.new_own_move)
	{
		enemy_chose = false;
		client_state.new_own_move = false;
		g7_append_to_save(client_state.buf_cmd);
		g7_append_to_save(client_state.own_buf_cmd);

		g7_command_parse(client_state.buf_cmd);
		char buf[32]="S";
		net_send_buffer(buf, 32);
		g7_append_to_save(buf);

		int a_win = 0;
		int b_win = 0;

		if(!g7_has_moves(game_state.playerA))
			a_win = -1;

		if(!g7_has_moves(game_state.playerB))
			b_win = -1;

		if(g7_on_finish(game_state.playerA))
			a_win = 1;

		if(g7_on_finish(game_state.playerB))
			b_win = 1;

		if (a_win * b_win == 1)
		{
			game_state.draw = true;
			strcpy(buf,"R");
		}

		if (a_win > b_win)
		{
			game_state.a_win = true;
			strcpy(buf,"WA");			
		}

		if (b_win > a_win)
		{
			game_state.b_win = true;
			strcpy(buf,"WB");
		}

		if (a_win == 0 && b_win == 0)
		{
			strcpy(buf,"K");			
		}

		net_send_buffer(buf, 32);
		g7_append_to_save(buf);

	}

}



Uint32 net_callback(Uint32 interval, void *param)
{
	char message[32];

	TCPsocket *client = &client_state.socket;

	int len = 32;

	if(!running)
	{
		SDLNet_TCP_Close(*client);
		return 0;
	}



	len = SDLNet_TCP_Recv(*client, message, 32);

	if(!len)
	{
		printf("SDLNet_TCP_Recv: %s\n",SDLNet_GetError());
		return 0;
	}

	bool save_command = false;

	switch(message[0])
	{
		case 'K':
		//keep-alive, we can easily ignore it
		break;

		case 'Q':
		//peer quitting, gotta quit too
		running=0;
		//TODO
		break;

		case 'M':
		//move info, let's update
		enemy_chose = true;
		strncpy(client_state.buf_cmd, message, 32);
		save_command = false;
		break;
		// game_state.current_move++;
		case 'P':
		case 'W':
		case 'R':
		//position info, let's update
		g7_command_parse(message);
		save_command = true;

		break;

		case 'S':
		//make a step		
			enemy_chose = false;
			client_state.new_own_move = false;
			g7_command_parse(client_state.buf_cmd);
			save_command=true;
		break;


		default:
		//dunno
		break;

	}

	if (save_command && client_state.host)
		g7_append_to_save(message);		
	


	SDL_Event event;
	SDL_UserEvent userevent;

	userevent.type = SDL_USEREVENT;
	userevent.code = 0;
	userevent.data1 = NULL;
	userevent.data2 = NULL;

	event.type = SDL_USEREVENT;
	event.user = userevent;

	SDL_PushEvent(&event);

	if(running)
	return(interval);

	return 0;
}

int gameplay_stageloop(G7_stage *stage)
{
	SDL_AddTimer(100, net_callback, NULL);

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


	struct nk_color background = nk_rgb(28,48,62);

	g7_vertex mouse;
	mouse.x = 0;
	mouse.y = 0;

	SDL_Event evt;
	while (running && SDL_WaitEvent(&evt))
	{

		if(stage->flags & G7_PARAM_HOST)
			server_check();
		
		nk_input_begin(stage->ctx);

		do
		{
			if (evt.type == SDL_QUIT) running = 0;
			g7_event_filter(&evt, &mouse);
			
		}
		while (SDL_PollEvent(&evt));

		nk_input_end(stage->ctx);
		SDL_GetMouseState(&mouse.x, &mouse.y);

		if (nk_begin(stage->ctx, "Menu", nk_rect(0, 0, x_offset - 50.0f, (float)win_height), //NK_WINDOW_MOVABLE| NK_WINDOW_SCALABLE|NK_WINDOW_MINIMIZABLE|
		  
			NK_WINDOW_TITLE | NK_WINDOW_BORDER))
		{

			nk_layout_row_dynamic(stage->ctx, 30, 1);

			if(!client_state.new_own_move)
			{
				nk_label(stage->ctx, "Your move", NK_TEXT_CENTERED);
			}
			else
			{
				nk_label(stage->ctx, "Waiting...", NK_TEXT_CENTERED);
			}


			if (nk_button_label(stage->ctx, "Exit"))
			{
				running = 0;
			}


		}
		nk_end(stage->ctx);

		//Drawing 
		{
		float bg[4];
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

		if (game_state.draw || game_state.a_win || game_state.b_win)
		{
			running = 0;
		}


	}

	net_atexit(client_state.own_state->prefix);
	g7_close_save();

	int retcode = 0;

	if (game_state.draw)
	{
		retcode = 3;		
	}

	if ((client_state.host && game_state.a_win) || (!client_state.host && game_state.b_win))
	{
		retcode = 2;
	}

	if ((!client_state.host && game_state.a_win) || (client_state.host && game_state.b_win))
	{
		retcode = 1;
	}

	return retcode;
}