// Mateusz Maciejewski
// 15.02.2017
// connect_dialog.c

#include "g7_common.h"

//waiting flag
bool waiting = true;
//used for the waiting animation
char dots_tab[]=".....";

//send map to client
bool net_send_map()
{
	char buf[1024]="";
	char buf2[1024]="";
	//send map size
	sprintf(buf, "X %d %d", game_state.map.size.x, game_state.map.size.y);
	sprintf(buf2, "%-15s", buf);


	net_send_buffer(buf2, strlen(buf2)+1);

	//send individual tiles
	for (int ii = 0; ii < game_state.map.size.y; ++ii)	
		for (int i = 0; i < game_state.map.size.x; ++i)		
			net_send_byte((char)game_state.map.tab[i][ii] + '0');

	//send player A position
	sprintf(buf, "PA %d %d", game_state.playerA.pos.x, game_state.playerA.pos.y);
	sprintf(buf2, "%-31s", buf);
	net_send_buffer(buf2, strlen(buf2)+1);

	//send player B position
	sprintf(buf, "PB %d %d", game_state.playerB.pos.x, game_state.playerB.pos.y);
	sprintf(buf2, "%-31s", buf);
	net_send_buffer(buf2, strlen(buf2)+1);

	//send player A velocity
	sprintf(buf, "VA %d %d", game_state.playerA.vel.x, game_state.playerA.vel.y);
	sprintf(buf2, "%-31s", buf);
	net_send_buffer(buf2, strlen(buf2)+1);

	//send player B velocity
	sprintf(buf, "VB %d %d", game_state.playerB.vel.x, game_state.playerB.vel.y);
	sprintf(buf2, "%-31s", buf);
	net_send_buffer(buf2, strlen(buf2)+1);

	//set the maximum velocity difference
	sprintf(buf, "D %d", game_state.max_delta);
	sprintf(buf2, "%-31s", buf);
	net_send_buffer(buf2, strlen(buf2)+1);

	return true;

}

//receive map from server
bool net_receive_map()
{
	char buf[1024];
	net_receive_buffer(buf, 16);

	//read map size
	sscanf(buf, "X %d %d", &game_state.map.size.x, &game_state.map.size.y);

	//allocate map in memory
	g7_allocate_map(&game_state.map);

	//read each individual tile
	for (int ii = 0; ii < game_state.map.size.y; ++ii)
	{
		for (int i = 0; i < game_state.map.size.x; ++i)
		{
			char bbyte;
			net_receive_byte(&bbyte);
			game_state.map.tab[i][ii] = bbyte - '0';
		}
	}

	//read 5 commands
	//position x2, velocity x2, vel. diff.
	for (int i = 0; i < 5; ++i)
	{
		net_receive_buffer(buf, 32);
		g7_command_parse(buf);
	}

	return true;
}

//SDL_Timer function checking if the peer is ready
Uint32 net_wait(Uint32 interval, void *param)
{
	//if we already have a connection
	//then abort
	if (!waiting)
		return 0;

	//animation gimmicks
	static int dots = 0;

	dots_tab[dots]='.';
	dots_tab[++dots]=0;

	dots %=4;
	//end of animation


	//check if client appeared
	if (client_state.host)
	{
		TCPsocket client;
		TCPsocket server = *((TCPsocket*)param);

		//if yes
		if((client = SDLNet_TCP_Accept(server)) != NULL)
		{		
			//then send map	
			client_state.socket = client;
			net_send_map();
			waiting = false;					
		}

		
	}


	//check if server appeared
	if (!client_state.host)
	{
		IPaddress ip = *((IPaddress*)param);
		TCPsocket client;

		//if yes
		if((client = SDLNet_TCP_Open(&ip)) != NULL)
		{			
			//then receive map
			client_state.socket = client;
			net_receive_map();
			waiting = false;
		}
		
	}

	//send an event to wake up the main thread
	SDL_Event event;
	SDL_UserEvent userevent;

	userevent.type = SDL_USEREVENT;
	userevent.code = 0;
	userevent.data1 = NULL;
	userevent.data2 = NULL;

	event.type = SDL_USEREVENT;
	event.user = userevent;

	SDL_PushEvent(&event);	

	//wake me up in (interval) ms
	return interval;
}


//main thread
int connect_dialog_stageloop(g7_stage *stage)
{
	//init SDLNet lib
	SDLNet_Init();

	//flag if user cancelled the wait
	bool canceled = false;

	IPaddress ip;

	//some settings
	game_state.playerA.prefix = 'A';
	game_state.playerB.prefix = 'B';
	game_state.max_delta = 5;

	//if we are the host
	if(stage->flags & G7_PARAM_HOST)
	{
		client_state.host = true;
		client_state.own_state = &(game_state.playerA);
		client_state.enemy_state = &(game_state.playerB);

		SDLNet_ResolveHost(&ip, NULL, 9999);
		TCPsocket server = SDLNet_TCP_Open(&ip);
		puts("Waiting for a client to connect...");

		SDL_AddTimer(300, net_wait, &server);
					
	}
	else
	//if we are the client
	{
		client_state.own_state = &(game_state.playerB);
		client_state.enemy_state = &(game_state.playerA);
		client_state.host = false;

		SDLNet_ResolveHost(&ip,"localhost",9999);

		SDL_AddTimer(300, net_wait, &ip);

	}

	//some more setup
	client_state.x_offset=200;
	client_state.new_own_move = false;
	client_state.new_enemy_move = false;
	client_state.scale = 1.0f;


	//running flag
	int running = 1;

	struct nk_color background = nk_rgb(28,48,62);

	int win_width;
	int win_height;
	SDL_GetWindowSize(stage->win, &win_width, &win_height);

	SDL_Event evt;
	//while waiting
	while (waiting && running && SDL_WaitEvent(&evt))
	{
		
		nk_input_begin(stage->ctx);

		do
		{
			if (evt.type == SDL_QUIT) return 0;
			nk_sdl_handle_event(&evt);
		}
		while (SDL_PollEvent(&evt));

		nk_input_end(stage->ctx);

		if (nk_begin(stage->ctx, "Main menu", nk_rect(0, 0, (float)win_width, (float)win_height), //NK_WINDOW_MOVABLE| NK_WINDOW_SCALABLE|NK_WINDOW_MINIMIZABLE|
		   
			0))
		{
		

			nk_layout_row_dynamic(stage->ctx, 80, 1);

			if(stage->flags & G7_PARAM_HOST)
				nk_label(stage->ctx, "Waiting for the client", NK_TEXT_CENTERED);
			else
				nk_label(stage->ctx, "Waiting for the host", NK_TEXT_CENTERED);

			nk_label(stage->ctx, dots_tab, NK_TEXT_CENTERED);


			if (nk_button_label(stage->ctx, "Cancel"))
			{
				canceled = true;
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
	waiting = false;

	//if we want to abort
	if(canceled)
		return -1;

	//if we found the peer
	return	0;
}
