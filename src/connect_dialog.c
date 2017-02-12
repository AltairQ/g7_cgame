#include "g7_common.h"

bool waiting = true;
char dots_tab[]=".....";

Uint32 net_wait(Uint32 interval, void *param)
{
	if (!waiting)
		return 0;

	static int dots = 0;

	dots_tab[dots]='.';
	dots_tab[++dots]=0;

	dots %=4;


	if (client_state.host)
	{
		TCPsocket client;
		TCPsocket server = *((TCPsocket*)param);

		if(client = SDLNet_TCP_Accept(server))
		{
			waiting = false;
			client_state.socket = client;					
		}

		
	}

	if (!client_state.host)
	{
		IPaddress ip = *((IPaddress*)param);
		TCPsocket client;

		if(client = SDLNet_TCP_Open(&ip))
		{
			waiting = false;
			client_state.socket = client;
		}
		
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

	

	return interval;



}



int connect_dialog_stageloop(G7_stage *stage)
{
	SDLNet_Init();

	TCPsocket client;
	TCPsocket server;
	IPaddress ip;

	game_state.playerA.prefix = 'A';
	game_state.playerB.prefix = 'B';

	if(stage->flags & G7_PARAM_HOST)
	{
		client_state.host = true;
		client_state.own_state = &(game_state.playerA);
		client_state.enemy_state = &(game_state.playerB);

		SDLNet_ResolveHost(&ip, NULL, 9999);
		server = SDLNet_TCP_Open(&ip);
		puts("Waiting for a client to connect...");

		SDL_AddTimer(300, net_wait, &server);
					
	}
	else
	{
		client_state.own_state = &(game_state.playerB);
		client_state.enemy_state = &(game_state.playerA);
		client_state.host = false;

		SDLNet_ResolveHost(&ip,"localhost",9999);

		SDL_AddTimer(300, net_wait, &ip);


	}

	client_state.new_own_move = false;
	client_state.new_enemy_move = false;
	client_state.display_scale = 1.0f;
	client_state.last_move = 0;



	int running = 1;


	struct nk_color background = nk_rgb(28,48,62);

	int win_width;
	int win_height;
	SDL_GetWindowSize(stage->win, &win_width, &win_height);

	SDL_Event evt;
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
			// if (nk_button_label(stage->ctx, "button"))
			nk_label(stage->ctx, dots_tab, NK_TEXT_CENTERED);


			if (nk_button_label(stage->ctx, "Cancel"))
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
	waiting = false;
	return 0;	
}
