// Mateusz Maciejewski
// 15.02.2017
// parser.h

//parse command from string
void g7_command_parse(char* cmd);

//read gamestate from file
bool g7_init_gamestate_from_file(char* fname, bool issave);

//allocate map in memory
bool g7_allocate_map(g7_map* in_map);
