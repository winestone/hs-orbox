#ifndef _CAPP_H_
    #define _CAPP_H_

#include <chrono>
#include <future>
#include <mutex>
#include <thread>

#include <SDL2/SDL.h>
#include <cstdio>
#include <cstdlib>
#define _USE_MATH_DEFINES
#include <cmath>
#include <cstring>
#include <ctime>
#include <cstdarg>
#include <windows.h>

#include "CDebug.h"
#include "CEvent.h"
#include "CRender.h"

//Window width and height
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 600;

//Menu button stuff
const int NUM_MENU_BUTTONS = 3;
const int MENU_BUTTON_X = 550;
const int MENU_BUTTON_Y = 300;
const int MENU_BUTTON_WIDTH = 200;
const int MENU_BUTTON_HEIGHT = 40;
const int MENU_BUTTON_BORDER = 40;

//Menu button animation stuff
const int MENU_BUTTON_ANIM_X = 500;
const int MENU_BUTTON_ANIM_Y = MENU_BUTTON_Y;
const double MENU_BUTTON_ANIM_TIME_MULTIPLIER = 0.2;

//Menu title stuff
const int MENU_BACK_X = 350;
const int MENU_BACK_Y = 100;
const int MENU_TEXT_X = 500;
const int MENU_TEXT_Y = 100;
const double MENU_BACK_POS_RATIO = 6.0;
const double MENU_BACK_POS_HEIGHT_MIN = 0.0;
const double MENU_BACK_POS_HEIGHT_MAX = 80.0;
const double MENU_BACK_POS_HEIGHTEDGE_VARY = 2.0;
const double MENU_BACK_CELL_RATIO = 6.0;
const double MENU_BACK_CELL_HEIGHT_MIN = 4.0;
const double MENU_BACK_CELL_HEIGHT_MAX = 8.0;
//const double BACK_CELL_SPEED_MIN = 0.00005;
//const double BACK_CELL_SPEED_DIFF = 0.002;
const double MENU_BACK_CELL_SPEED_DIST_MIN = 0.5;
const double MENU_BACK_CELL_SPEED_DIST_MAX = 1.1;
const double MENU_BACK_CELL_SPEED_VARY = 0.002;
const int MENU_BACK_CELL_NUM = 100;

//In game background animation stuff
const double BACK_POS_RATIO = 6.0;
const double BACK_POS_HEIGHT_MIN = 10.0;
const double BACK_POS_HEIGHT_MAX = 400.0;
const double BACK_CELL_RATIO = 6.0;
const double BACK_CELL_HEIGHT_MIN = 4.0;
const double BACK_CELL_HEIGHT_MAX = 8.0;
//const double BACK_CELL_SPEED_MIN = 0.00005;
//const double BACK_CELL_SPEED_DIFF = 0.002;
const double BACK_CELL_SPEED_DIST_MIN = 0.3;
const double BACK_CELL_SPEED_DIST_MAX = 0.6;
const double BACK_CELL_SPEED_VARY = 0.001;  
const int BACK_CELL_NUM = 400;

//Tutorial stuff
const int TUT_ANIM_FADE_TOTATIME = 40;

//Grid stuff
const int GRID_X = 40;
const int GRID_Y = 20;
const int GRID_WIDTH = 28;
const int GRID_HEIGHT = 14;
const int GRID_CELLSIZE = 40;

//How many levels there are
const int LEVELS = 10;

//Animation stuffs
const int ANIM_MOVE_TIME_MULTIPLIER = 4;
const int ANIM_TOTALTIME = 100;

//Tile_Assign values, mainly used for inputing from strings and stuff
const int TILE_TYPES = 10;
enum class Tile_Assign {
    EMPTY = 0,
    START = 1,
    DESTINATION = 2,
    BLOCK = 3,
    REFLECTOR_NE = 4,
    REFLECTOR_SE = 5,
    REFLECTOR_SW = 6,
    REFLECTOR_NW = 7,
    TELEPORTER1 = 8,
    TELEPORTER2 = 9
};

//Tile values, requires extra data to determine exact tile
enum class Tile {
    EMPTY = 0,
    START = 1,
    DESTINATION = 2,
    BLOCK = 3,
    REFLECTOR = 4,
    TELEPORTER = 5
};

//Game state stuff, waiting for a move or animating
enum {
    STATE_GAME_MOVE,
    STATE_GAME_ANIM
};

//Which window, loading screen, main menu, game or tutorial
enum {
    WIND_LOADING,
    WIND_MAINMENU,
    WIND_GAME,
    WIND_TUTORIAL,
    WIND_CERTIFICATE
};

//Which direction
enum {
    DIR_UP = 0,
    DIR_RIGHT = 1,
    DIR_DOWN = 2,
    DIR_LEFT = 3
};

//The application, inherits from CEvent to support events
class CApp : public CEvent {
    private:
        //Whether we are running or not
        bool            Running;
        
        //The window and it's renderer and surface
        SDL_Window*     Wind_Display;
        SDL_Surface*    Surf_Display;
        SDL_Renderer*   Rend_Display;
    
    private:
        //Background texture
        SDL_Texture*    Text_Background;
        //Menu particle
        SDL_Texture*    Text_MenuBackParticle;
        //Background particle
        SDL_Texture*    Text_BackParticle;
        
        //Menu title
        SDL_Texture*    Text_MenuText;
        //Menu buttons
        SDL_Texture*    Text_MenuButton[NUM_MENU_BUTTONS][3];
        
        //ASCII Characters
        SDL_Texture*    Text_Character[256];
        
        //Tutorial Text
        SDL_Texture*    Text_Tutorial_Background;
        
        //Certificate texture
        SDL_Texture*    Text_Certificate;
        //Certificate button
        SDL_Texture*    Text_MainMenuButton[3];
        int Text_MainMenuButton_W, Text_MainMenuButton_H;
        
        //Player texture
        SDL_Texture*    Text_Player;
        //Tile textures
        SDL_Texture*    Text_Tile[TILE_TYPES];
    
    private:
        //Debug instance
        CDebug debug;
        
        //Screen debug buffer and position in string
        char screenDebug[1<<16];
        int screenDebugPos;
        
        //Unused
        FILE* settings;
        
        //Which window and state are we in
        int window;
        int state;
        
        //For timing the main loop
        Uint32 Time, PrevTime, PrevRunTime;
        
        //Helps store buttons
        enum {
            MENU_BUTTON_PLAY,
            MENU_BUTTON_TUTORIAL,
            MENU_BUTTON_EXIT
        };
        //Helps store button states
        enum {
            MENU_BUTTON_STATE_NONE = 0,
            MENU_BUTTON_STATE_HOVER = 1,
            MENU_BUTTON_STATE_SELECTED = 2
        };
        //Stores stuff for each button
        struct button_struct {
            int x, y;
            int state;
        };
        //The buttons
        button_struct menu_button[NUM_MENU_BUTTONS];
        //What is hovered over/selected
        int menu_hover, menu_selected;
        int menu_me_hover, menu_me_selected;
        //Animating or not
        bool menu_anim_flag;
        //Starting y, current y and destination y
        int menu_anim_starty, menu_anim_y, menu_anim_endy;
        //Time and total time for animation
        int menu_anim_time, menu_anim_totaltime;
        
        //Tutorial button data
        button_struct tut_button;
        //Certificate button data
        button_struct cert_button;
        
        //The tile class
        class tile_t {
            public:
                //Type of tile
                Tile type;
                //Tile_Assign value of tile
                Tile_Assign assign_type;
                
                //Some data
                int data;
                int side[4];
                
                //reflectors
                int srcdir[2];
                int destdir[2];
                
                //teleporters
                int id;
                int srcx, srcy;
                int destx, desty;
                
                //Unused?
                int dx, dy;
            public:
                //standard copy operator
                tile_t& operator= (const tile_t other);
                //assigning an int via Tile_Assign
                tile_t& operator= (const Tile_Assign other);
        };
        
        //The current level
        int currlvl;
        //Next direction player want's to go in
        int nextdir;
        //Player x and y
        int playerx, playery;
        //The level data
        tile_t level[LEVELS][GRID_HEIGHT][GRID_WIDTH];
        //Current grid
        tile_t grid[GRID_HEIGHT][GRID_WIDTH];
        
        //Player animation x and y
        int animx, animy;
        //Start and destination x of player
        int anim_startx, anim_starty;
        int anim_endx, anim_endy;
        //Current time and total time to animate
        int anim_time, anim_totaltime;
        //If the player fell out of the map
        bool anim_restart_flag;
        //If the player has won
        bool anim_win_flag;
        //If the player has hit a reflector and which way to go
        int anim_reflector_data; //-1 is false, 0-3 specifies direction as DIR_ enum
        //If the player has teleported
        bool anim_teleporter_flag;
        //Which way to go if player has been teleported
        int anim_teleporter_data;
        
        //Tutorial
        int tut_anim_fade_time;
        
        //Animation particles
        struct anim_particle_t {
            double radius;      //Distance from middle
            double constangle;  //Angle tilt around x axis
            double angle;       //Angle tilt around y axis
            double speed;       //Speed in pixels per tick
            int w, h;           //width and height of pixel
        };
        //Arrays of animation particles for the menu and backgrount particles
        anim_particle_t anim_particle[BACK_CELL_NUM];
        anim_particle_t menu_anim_particle[MENU_BACK_CELL_NUM];
    
    public:
        //Constructor
        CApp();
        //To call to run
        int OnExecute();
    
    private:
        //Initialises class
        bool OnInit();
        
            //Function which does initialisation in another thread while main thread wait's, accept events, loops and renders
            bool InitThread(std::mutex& callerMutex);
            //Set's up a level, given the index, using the string
            void SetupLevel(int index, const char* string);
            //Set's up all the levels
            void SetupLevels();
            //Load's the level specified by index onto the screen
            void LoadLevel(int index);
            //Reset's the same level
            void ResetGame();
            //Get's a line from a file, not sure if used
            void getLine(FILE* file, char* string);
            //Flips a surface vertically, turns y = x into y = -x
            bool flipSurfaceVertical(SDL_Surface* Surf_Src);
            //Print's what is in the string onto the screen at x and y
            void screenPrint(int x, int y, const char* string);
            //Does same thing except wraps the text
            void screenPrintWrap(int x, int y, int targetw, const char* string);
        
        //CEvent event caller
        void OnEvent(SDL_Event* Event);
        
            //Called when mouse moves
            void OnMouseMove(int mX, int mY, int relX, int relY, bool Left,bool Right,bool Middle);
            //Setup's the menu animation given a destination button
            void setup_menu_me_anim(int i);
            //When left mouse button is pressed
            void OnLButtonDown(int mX, int mY);
            //When left mouse button is depressed
            void OnLButtonUp(int mX, int mY);
            //When right mouse button is pressed
            void OnRButtonDown(int mX, int mY);
            //When right mouse button is depressed
            void OnRButtonUp(int mX, int mY);
            //When a keyboard key is pressed
            void OnKeyDown(SDL_Scancode scancode, SDL_Keycode sym, SDL_Keymod mod);
            //When a keyboard key is depressed
            void OnKeyUp(SDL_Scancode scancode, SDL_Keycode sym, SDL_Keymod mod);
            
            //Moves a player in specified direction and set's up animation
            bool playerMove(int direction);
        
        //Called every loop
        void OnLoop();
        
        //Renders stuff to screen
        void OnRender();
        
        //Clean's up textures
        void OnCleanup();
};

#endif
