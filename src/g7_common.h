#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include <dirent.h>

#include "external_wrapper.h"

#define G7_PARAM_NEWGAME 1
#define G7_PARAM_LOADGAME 2
#define G7_PARAM_FULLSCREEN 4
#define G7_PARAM_HOST 8

#include "fifo/fifo.h"

#include "utils.h"

#include "main_menu.h"
#include "load_dialog.h"
#include "gameplay.h"
#include "parser.h"
#include "engine.h"

