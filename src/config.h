#ifndef _config_h_
#define _config_h_

// app parameters
#define DEBUG 0
#define FULLSCREEN 0
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define VSYNC 1
#define SCROLL_THRESHOLD 0.1
#define MAX_MESSAGES 4
#define DAY_LENGTH 600
#define INVERT_MOUSE 0
#define KONSTRUCTS_TEXT_MESSAGE_SIZE 1024
#define KONSTRUCTS_PATH_SIZE 64

// rendering options
#define SHOW_LIGHTS 1
#define SHOW_CROSSHAIRS 1

// key bindings
#define KONSTRUCTS_KEY_FORWARD 'W'
#define KONSTRUCTS_KEY_BACKWARD 'S'
#define KONSTRUCTS_KEY_LEFT 'A'
#define KONSTRUCTS_KEY_RIGHT 'D'
#define KONSTRUCTS_KEY_JUMP GLFW_KEY_SPACE
#define KONSTRUCTS_KEY_FLY GLFW_KEY_TAB
#define KONSTRUCTS_KEY_OBSERVE 'O'
#define KONSTRUCTS_KEY_OBSERVE_INSET 'P'
#define KONSTRUCTS_KEY_INVENTORY_TOGGLE 'E'
#define KONSTRUCTS_KEY_ZOOM GLFW_KEY_LEFT_SHIFT
#define KONSTRUCTS_KEY_ORTHO 'F'
#define KONSTRUCTS_KEY_CHAT 't'
#define KONSTRUCTS_KEY_COMMAND '/'
#define KONSTRUCTS_KEY_SIGN '`'
#define KONSTRUCTS_KEY_DEBUG_SCREEN 292 // F3

// inventory
#define INVENTORY_SLOTS 9
#define INVENTORY_ROWS 4
#define MAX_SLOT_SIZE 64
#define INVENTORY_ITEM_SIZE 48
#define INVENTORY_FONT_SIZE 20

// advanced parameters
#define CREATE_CHUNK_RADIUS 10
#define RENDER_CHUNK_RADIUS 10
#define RENDER_SIGN_RADIUS 4
#define DELETE_CHUNK_RADIUS 14
#define CHUNK_SIZE 32
#define COMMIT_INTERVAL 5
#define MAX_BLOCK_HEIGHT 4096

#endif
