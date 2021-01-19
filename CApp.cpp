#include "CApp.h"

//Initialises class to known state, mainly pointers to nullptr
CApp::CApp () {
    Wind_Display = nullptr;
    Rend_Display = nullptr;
    
    Text_Background = nullptr;
    Text_MenuBackParticle = nullptr;
    Text_BackParticle = nullptr;
    
    Text_MenuText = nullptr;
    for (int j = 0;j < NUM_MENU_BUTTONS;j++) {
        for (int i = 0;i < 3;i++) {
            Text_MenuButton[i][j] = nullptr;
        }
    }
    
    for (int i = 0;i < 256;i++) {
        Text_Character[i] = nullptr;
    }
    
    Text_Tutorial_Background = nullptr;
    
    Text_Certificate = nullptr;
    for (int i = 0;i < 3;++i) {
        Text_MainMenuButton[i] = nullptr;
    }
    
    Text_Player = nullptr;
    
    for (int i = 0;i < TILE_TYPES;i++) {
        Text_Tile[i] = nullptr;
    }
    
    settings = nullptr;
    
    Running = true;
}

//Provide standard copy operator
CApp::tile_t& CApp::tile_t::operator= (const CApp::tile_t other) {
    memcpy(this, &other, sizeof(CApp::tile_t));
}

//Assignment from a Tile_Assign type
CApp::tile_t& CApp::tile_t::operator= (const Tile_Assign other) {
    assign_type = other;
    switch (other) {
        case Tile_Assign::EMPTY: default:
            (*this).type = Tile::EMPTY;
            break;
        case Tile_Assign::START:
            (*this).type = Tile::START;
            break;
        case Tile_Assign::DESTINATION:
            (*this).type = Tile::DESTINATION;
            break;
        case Tile_Assign::BLOCK:
            (*this).type = Tile::BLOCK;
            break;
        case Tile_Assign::REFLECTOR_NE:
            (*this).type = Tile::REFLECTOR;
            srcdir[0] = DIR_DOWN;  destdir[0] = DIR_RIGHT;
            srcdir[1] = DIR_LEFT;  destdir[1] = DIR_UP;
            break;
        case Tile_Assign::REFLECTOR_SE:
            (*this).type = Tile::REFLECTOR;
            srcdir[0] = DIR_UP;    destdir[0] = DIR_RIGHT;
            srcdir[1] = DIR_LEFT;  destdir[1] = DIR_DOWN;
            break;
        case Tile_Assign::REFLECTOR_SW:
            (*this).type = Tile::REFLECTOR;
            srcdir[0] = DIR_UP;    destdir[0] = DIR_LEFT;
            srcdir[1] = DIR_RIGHT; destdir[1] = DIR_DOWN;
            break;
        case Tile_Assign::REFLECTOR_NW:
            (*this).type = Tile::REFLECTOR;
            srcdir[0] = DIR_DOWN;  destdir[0] = DIR_LEFT;
            srcdir[1] = DIR_RIGHT; destdir[1] = DIR_UP;
            break;
        case Tile_Assign::TELEPORTER1:
            (*this).type = Tile::TELEPORTER;
            id = 0;
            break;
        case Tile_Assign::TELEPORTER2:
            (*this).type = Tile::TELEPORTER;
            id = 1;
            break;
    }
}

int CApp::OnExecute () {
    //Initialise
    if(OnInit() == false) {
        //Assuming that Debug initialised :P
        debug.PInd(); fprintf(debug.file, "Initialisation failed.\n");
        OnCleanup();
        return -1;
    }
    
    //Holds events from SDL_PollEvent
    SDL_Event Event;
    
    //Loop while Running
    while (Running) {
        //Get current time
        Time = SDL_GetTicks();
        //If 20 milliseconds have passed since last run
        if (((PrevRunTime+10) <= Time) || (PrevTime > Time)) {
            //The last time we ran was now
            PrevRunTime = Time;
            //Handle events, loop and rendering
            while (SDL_PollEvent(&Event)) {
                OnEvent(&Event);
            }
            OnLoop();
            OnRender();
        } else {
            //Otherwise, delay the remaining time
            SDL_Delay(PrevRunTime + 10 - Time);
        }
        //Last time the loop ran
        PrevTime = Time;
    }
    
    //Cleanup
    OnCleanup();
    
    //Tell the OS all went ok
    return 0;
}

//Program start point
#undef main
int main (int argc, char* args[]) {
    //Create an instance of the app
    CApp app;
    
    //Run it and return it's return value
    return app.OnExecute();
}
