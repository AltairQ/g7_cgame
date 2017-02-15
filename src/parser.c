#include "g7_common.h"

bool g7_allocate_map(g7_map* in_map)
{

	(*in_map).tab = malloc(sizeof(g7_tile*) * (*in_map).size.x);

	if ((*in_map).tab == NULL)
	{
		return false;
	}

	for (int i = 0; i < (*in_map).size.x; ++i)
	{
		(*in_map).tab[i] = malloc(sizeof(g7_tile) * (*in_map).size.y);

		if((*in_map).tab[i] == NULL)
		{
			return false;
		}
	}

	return true;

}


bool g7_read_state_from_file(FILE* map)
{
	int ret_code = 0;
	g7_map tmp_map;

	fscanf(map, "X %d %d ", &(tmp_map.size.x), &(tmp_map.size.y));
	if(tmp_map.size.x <= 0 || tmp_map.size.y <=0 || tmp_map.size.x > (1<<30) || tmp_map.size.y > (1<<30) )
	{
		ret_code = -2;
		goto cleanup;
	}

	if (!g7_allocate_map(&tmp_map))
	{
		ret_code = -3;
		goto cleanup;
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

	char* linebuf = malloc(sizeof(char)*1024);

	while(fgets(linebuf, 1024, map) != NULL)
		g7_command_parse(linebuf);
	
	free(linebuf);


	cleanup:
	fclose(map);

	return ret_code == 0;

}

bool init_gamestate_from_file(char* fname, bool issave)
{
	FILE *sfile;
	sfile = fopen(fname, "r+");

	if (sfile == NULL)
		return false;

	if (!g7_read_state_from_file(sfile))
		return false;

	fclose(sfile);	

	if (issave)
	{
		g7_current_save_file = fopen(fname, "at");
	}
	else
	{
		g7_create_save_file();
		g7_write_map_to_save();
	}

	return true;
}


void g7_command_parse(char* cmd)
{
	int x, y;

	switch(cmd[0])
	{

	case 'M':
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

	break;

	case 'P':
	
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

	break;

	case 'V':
		if (cmd[1] == 'A')
		{			
			if(sscanf(cmd+3, "%d %d", &x, &y) == 2 )
			{
				game_state.playerA.vel.x = x;
				game_state.playerA.vel.y = y;
			}
		}

		if (cmd[1] == 'B')
		{			
			if(sscanf(cmd+3, "%d %d", &x, &y) == 2 )
			{
				game_state.playerB.vel.x = x;
				game_state.playerB.vel.y = y;
			}
		}
	break;


	default:
	break;
	}

}