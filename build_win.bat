gcc src/end_game.c src/net_utils.c src/saves.c src/main.c src/main_menu.c src/load_dialog.c src/new_game_dialog.c src/gameplay.c src/utils.c src/engine.c src/parser.c src/connect_dialog.c -std=c99 -Wall -Wextra -Wshadow -Wconversion -lmingw32 -lSDL2main -lSDL2 -lSDL2_net -lopengl32 -lm -lGLU32 -o out/g7_w32.exe