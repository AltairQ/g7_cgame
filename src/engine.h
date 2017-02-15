//types of tiles
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

typedef struct 
{
	int x;
	int y;
}
g7_vertex;

typedef struct
{
	g7_vertex *tab;
	size_t count;
}
g7_v_list;

typedef struct
{
	g7_tile** tab;
	g7_vertex size;
}
g7_map;

typedef struct
{
	g7_vertex pos;
	g7_vertex vel;
	char prefix;	
}
g7_player_state;

typedef struct
{
	g7_map map;
	g7_player_state playerA;
	g7_player_state playerB;
}
g7_game_state;

typedef struct 
{
	bool new_own_move;
	bool new_enemy_move;
	g7_player_state *own_state;
	g7_player_state *enemy_state;
	char buf_cmd[32];
	int last_move;
	float display_scale;
	TCPsocket socket;
	bool host;
}
g7_client_state;

extern g7_game_state game_state;
extern g7_client_state client_state;

g7_vertex screen_to_tile(g7_vertex mouse);
bool g7_path_is_nonblocked(g7_vertex pos, g7_vertex dest);

