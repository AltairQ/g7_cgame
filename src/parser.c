#include "g7_common.h"

int load_map(int choice)
{
	char filename_buf[1024] = "maps/";

	strcat(filename_buf, maps[choice]);

	FILE *map;
	map = fopen(filename_buf, "rt");

	int ret_code = 0;

	if(map == NULL)
		return -1;

	g7_map tmp_map;

	fscanf(map, "%d %d ", &(tmp_map.size.x), &(tmp_map.size.y));
	if(tmp_map.size.x <= 0 || tmp_map.size.y <=0 || tmp_map.size.x > (1<<30) || tmp_map.size.y > (1<<30) )
	{
		ret_code = -2;
		goto cleanup;
	}

	tmp_map.tab = malloc(sizeof(g7_tile*) * tmp_map.size.x);

	if (tmp_map.tab == NULL)
	{
		ret_code = -3;
		goto cleanup;
	}

	for (int i = 0; i < tmp_map.size.x; ++i)
	{
		tmp_map.tab[i] = malloc(sizeof(g7_tile) * tmp_map.size.y);
		if(tmp_map.tab[i] == NULL)
		{
			ret_code = -3;
			goto cleanup;
		}
	}


	for (int ii = 0; ii < tmp_map.size.y; ++ii)
	{
		for (int i = 0; i < tmp_map.size.x; ++i)
		{
			char buf;
			fscanf(map, "%c ", &buf);
			tmp_map.tab[i][ii] = buf - '0';
		}
	}


	game_state.map = tmp_map;
	game_state.current_move = 1;


	cleanup:
	fclose(map);
	return ret_code;

}

void g7_command_parse(char* cmd)
{
	int x, y;

	if (cmd[0] == 'M')
	{
		if (cmd[1] == 'A')
		{			
			if(sscanf(cmd+3, "%d %d", &x, &y) == 2 )
			{
				game_state.playerA.vel.x = x;
				game_state.playerA.vel.y = y;

				game_state.playerA.pos.x += x;
				game_state.playerA.pos.y += y;
			}
		}

		if (cmd[1] == 'B')
		{			
			if(sscanf(cmd+3, "%d %d", &x, &y) == 2 )
			{
				game_state.playerB.vel.x = x;
				game_state.playerB.vel.y = y;

				game_state.playerB.pos.x += x;
				game_state.playerB.pos.y += y;
			}
		}

	}

	if (cmd[0] == 'P')
	{
		if (cmd[1] == 'A')
		{			
			if(sscanf(cmd+3, "%d %d", &x, &y) == 2 )
			{
				game_state.playerA.pos.x = x;
				game_state.playerA.pos.y = y;
			}
		}

		if (cmd[1] == 'B')
		{			
			if(sscanf(cmd+3, "%d %d", &x, &y) == 2 )
			{
				game_state.playerB.pos.x = x;
				game_state.playerB.pos.y = y;
			}
		}
	}


}