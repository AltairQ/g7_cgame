
typedef enum 
{
	blank =0,
	block,
	finish,
	startA,
	startB	
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
}
g7_player_state;

typedef struct
{
	g7_map map;
	g7_player_state playerA;
	g7_player_state playerB;
}
g7_game_state;

extern g7_game_state game_state;

g7_vertex screen_to_tile(g7_vertex mouse);
