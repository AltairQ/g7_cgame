// Mateusz Maciejewski
// 15.02.2017
// engine.h

//types of tiles (enum)
typedef enum 
{
	blank =0,
	block,
	finish,
	startA,
	startB,
	pA,
	pB	
}
g7_tile;

//primitive type of a point
typedef struct 
{
	int x;
	int y;
}
g7_vertex;

//map struct
typedef struct
{
	//2d dynamic array of tiles
	g7_tile** tab;
	//size of the map
	g7_vertex size;
}
g7_map;

//player state struct
typedef struct
{
	//position on the map
	g7_vertex pos;
	//velocity
	g7_vertex vel;
	//one character prefix
	//('A' for player A, 'B' for player B)
	char prefix;	
}
g7_player_state;

//game state
typedef struct
{
	//the map
	g7_map map;
	//maximum velocity difference
	int max_delta;
	
	//win state
	//is player A the winner?
	bool a_win;
	//is player B the winner?
	bool b_win;
	//is it a draw?
	bool draw;

	//state of player A
	g7_player_state playerA;
	//state of player B
	g7_player_state playerB;
}
g7_game_state;

//state of a client
//contains many flags that should be global
typedef struct 
{
	//have we made a move in current turn?
	bool new_own_move;
	//has the enemy made a move in current turn?
	bool new_enemy_move;
	//pointer to our own state
	//saves us the trouble to check if we are the host
	//every time we want to check our own state
	g7_player_state *own_state;
	//same for the enemy
	g7_player_state *enemy_state;
	//buffer for enemy command
	//contains their last move
	char buf_cmd[32];
	//the same for us
	char own_buf_cmd[32];
	//display scaling
	float scale;
	//x_offset for gameplay UI
	float x_offset;
	//SDLNet socket
	TCPsocket socket;
	//are we the host?
	bool host;
}
g7_client_state;

//global game state
extern g7_game_state game_state;
//global client state
extern g7_client_state client_state;

//convert screen coordinates to tile coordinates
g7_vertex screen_to_tile(g7_vertex mouse);
//check if we can access one tile from another
bool g7_path_is_nonblocked(g7_vertex pos, g7_vertex dest);
//is the tile within player's reach
bool g7_is_accessible(g7_player_state player, g7_vertex tile);
//is the player on finish line?
bool g7_on_finish(g7_player_state player);
//any possible moves for the player?
bool g7_has_moves(g7_player_state player);

