// Mateusz Maciejewski
// 15.02.2017
// gameplay.c

#include "g7_common.h"

//draw tile on screen
void g7_draw_tile(int x, int y, g7_tile tile, bool focus)
{
	//set colour and alpha according to tile type
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

	//cast the screen coordinates
	float xf = (float)x;
	float yf = (float)y;

	//calculate on-screen coordinates from x,y
	//just some multiplication
	float x1, x2, y1, y2;
	x1 = xf*client_state.scale;
	x2 = (xf+1.0f)*client_state.scale;
	y1 = yf*client_state.scale;
	y2 = (yf+1.0f)*client_state.scale;
	

	//draw tile
	glBegin(GL_QUADS);
	glVertex2f(client_state.x_offset + x1, y1);
	glVertex2f(client_state.x_offset + x2, y1);
	glVertex2f(client_state.x_offset + x2, y2);
	glVertex2f(client_state.x_offset + x1, y2);
	glEnd();
}

//global state of chosen tile
g7_vertex chosen_tile;

//draw the whole map
void g7_draw_map(g7_vertex mouse)
{
	//use transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//for each individual tile
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

//draw players on screen
void g7_draw_players()
{
	g7_draw_tile(game_state.playerA.pos.x , game_state.playerA.pos.y, pA, false);
	g7_draw_tile(game_state.playerB.pos.x , game_state.playerB.pos.y, pB, false);
}

//filtering events
void g7_event_filter(SDL_Event *event, g7_vertex *mouse)
{
	//if we receive a click
	if(event->type == SDL_MOUSEBUTTONDOWN)
	{
		//get the coordinates

		chosen_tile = screen_to_tile(*mouse);
		//if we are off-map
		if (chosen_tile.x >= game_state.map.size.x || chosen_tile.y >= game_state.map.size.y
			|| chosen_tile.x < 0 || chosen_tile.y < 0)
		{
			//set to -1, -1
			chosen_tile.x = -1;
			chosen_tile.y = -1;
		}
		else
		{
			//if it's a valid choice
			//> it's our turn
			//> the tile is accessible and within reach
			if(client_state.new_own_move == false && g7_is_accessible(*client_state.own_state, chosen_tile) && g7_path_is_nonblocked(client_state.own_state->pos, chosen_tile))
			{
				//do it
				int vx, vy;
				vx = chosen_tile.x - client_state.own_state->pos.x;
				vy = chosen_tile.y - client_state.own_state->pos.y;
				client_state.own_state->vel.x = vx;
				client_state.own_state->vel.y = vy;				
				client_state.own_state->pos = chosen_tile;

				//send move info
				char buf[32]="";
				sprintf(buf, "M%c %d %d", client_state.own_state->prefix, vx, vy);
				//SERVER ONLY
				//save our movement in buffer
				strncpy(client_state.own_buf_cmd, buf, 32);
				//send move info
				net_send_buffer(buf, 32);
				
				client_state.new_own_move = true;
				
			}
		}
	}


	//if it's an userevent then it's just a wake-up
	//from the net thread
	if (event->type == SDL_USEREVENT)
		return;		
	
	//send the event to nuklear
	nk_sdl_handle_event(event);	
	
}

//running flag
int running = 1;

//check game step
//check if the game should proceed (next turn)
//also check for game end
void server_check()
{
	if(!running)
		return;

	//if both players chose
	if (client_state.new_enemy_move && client_state.new_own_move)
	{
		//reset state
		client_state.new_enemy_move = false;
		client_state.new_own_move = false;
		//save commands
		g7_append_to_save(client_state.buf_cmd);
		g7_append_to_save(client_state.own_buf_cmd);

		//execute the client command
		g7_command_parse(client_state.buf_cmd);
		//send the S command
		char buf[32]="S";
		net_send_buffer(buf, 32);
		//save it
		g7_append_to_save(buf);

		//did a win?
		int a_win = 0;
		//did b win?
		int b_win = 0;

		if(!g7_has_moves(game_state.playerA))
			a_win = -1;

		if(!g7_has_moves(game_state.playerB))
			b_win = -1;

		if(g7_on_finish(game_state.playerA))
			a_win = 1;

		if(g7_on_finish(game_state.playerB))
			b_win = 1;

		//draw
		if (a_win * b_win == 1)
		{
			game_state.draw = true;
			strcpy(buf,"R");
		}

		//A won
		if (a_win > b_win)
		{
			game_state.a_win = true;
			strcpy(buf,"WA");			
		}

		//B won
		if (b_win > a_win)
		{
			game_state.b_win = true;
			strcpy(buf,"WB");
		}

		//nothing happened
		if (a_win == 0 && b_win == 0)
		{
			strcpy(buf,"K");			
		}

		//send command
		net_send_buffer(buf, 32);
		g7_append_to_save(buf);

	}

}


//the net_receive function
//it's running in a separate thread as a SDL_Timer
//checks for new data from peer
Uint32 net_callback(Uint32 interval, void *param)
{
	char message[32];

	TCPsocket *client = &client_state.socket;

	int len = 32;

	//if we are shutting down
	if(!running)
	{
		SDLNet_TCP_Close(*client);
		return 0;
	}



	//try to receive data
	len = SDLNet_TCP_Recv(*client, message, 32);

	if(!len)
	{
		printf("SDLNet_TCP_Recv: %s\n",SDLNet_GetError());
		return 0;
	}

	//should we save the command to save file?
	bool save_command = false;

	//process the data
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
			client_state.new_enemy_move = true;
			strncpy(client_state.buf_cmd, message, 32);
			save_command = false;
		break;
		// positon or win info
		case 'P':
		case 'W':
		case 'R':
			g7_command_parse(message);
			save_command = true;
		break;

		case 'S':
		//make a step		
			client_state.new_enemy_move = false;
			client_state.new_own_move = false;
			g7_command_parse(client_state.buf_cmd);
			save_command=true;
		break;


		default:
		//ignore it
		break;

	}

	if (save_command && client_state.host)
		g7_append_to_save(message);		
	


	//generate an user event to wake up the main thread
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

//main thread
int gameplay_stageloop(g7_stage *stage)
{
	//register the timer
	SDL_AddTimer(100, net_callback, NULL);

	//the rest is self-explanatory
	//regular stageloop

	chosen_tile.x = -1;
	chosen_tile.y = -1;

	int win_width = 1024;
	int win_height = 768;


	if(stage->flags & G7_PARAM_FULLSCREEN)
		g7_SDL_go_fullscreen(stage->win);
	else
		SDL_SetWindowSize(stage->win, win_width, win_height);

	stage->ctx = g7_nk_sdl_reset(stage->win);
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

		if (nk_begin(stage->ctx, "Menu", nk_rect(0, 0, client_state.x_offset - 50.0f, (float)win_height), //NK_WINDOW_MOVABLE| NK_WINDOW_SCALABLE|NK_WINDOW_MINIMIZABLE|
		  
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
		client_state.scale = MIN((float)(win_width- (int)client_state.x_offset)/(float)(game_state.map.size.x),
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