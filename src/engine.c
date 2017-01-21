#include "g7_common.h"

g7_game_state game_state;
//global game state

g7_vertex screen_to_tile(g7_vertex mouse)
{
	g7_vertex ret;

	ret.x = (int)floor((float)(mouse.x - 200)/50.0f);
	ret.y = (int)floor((float)mouse.y/50.0f);

	return ret;

}


