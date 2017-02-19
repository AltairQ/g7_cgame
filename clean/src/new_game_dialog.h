// Mateusz Maciejewski
// 15.02.2017
// new_game_dialog.h

//array with maps/saves
extern char *maps[256];
//number of found maps/saves
extern size_t map_count;
//main thread
int new_game_stageloop(g7_stage *stage);