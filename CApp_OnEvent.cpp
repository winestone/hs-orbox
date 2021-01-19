#include "CApp.h"

void CApp::OnEvent (SDL_Event* Event) {
    CEvent::OnEvent(Event);
}

void CApp::setup_menu_me_anim (int i) {
    menu_anim_flag = true;
    menu_anim_starty = menu_anim_y;
    menu_anim_endy = menu_button[i].y;
    menu_anim_time = 0;
    menu_anim_totaltime = abs(((double)(menu_anim_endy - menu_anim_starty))*MENU_BUTTON_ANIM_TIME_MULTIPLIER);
}

void CApp::OnMouseMove (int mX, int mY, int relX, int relY, bool Left, bool Right, bool Middle) {
    switch (window) {
        case WIND_MAINMENU:
            for (int i = 0;i < NUM_MENU_BUTTONS;i++) {
                if (((menu_button[i].x <= mX) && (mX <= (menu_button[i].x + MENU_BUTTON_WIDTH))) && 
                    ((menu_button[i].y <= mY) && (mY <= (menu_button[i].y + MENU_BUTTON_HEIGHT)))) {
                    if (menu_hover != i) {
                        if (menu_selected == -1) {
                            menu_button[menu_hover].state = MENU_BUTTON_STATE_NONE;
                            menu_button[i].state = MENU_BUTTON_STATE_HOVER;
                            setup_menu_me_anim(i);
                        }
                        menu_hover = i;
                    }
                }
            }
            break;
        case WIND_TUTORIAL:
            if (((tut_button.x <= mX) && (mX <= (tut_button.x + Text_MainMenuButton_W))) && 
                ((tut_button.y <= mY) && (mY <= (tut_button.y + Text_MainMenuButton_H))) && 
                (tut_button.state != MENU_BUTTON_STATE_SELECTED)) {
                tut_button.state = MENU_BUTTON_STATE_HOVER;
            } else {
                tut_button.state = MENU_BUTTON_STATE_NONE;
            }
            break;
        case WIND_CERTIFICATE:
            if (((cert_button.x <= mX) && (mX <= (cert_button.x + Text_MainMenuButton_W))) && 
                ((cert_button.y <= mY) && (mY <= (cert_button.y + Text_MainMenuButton_H))) && 
                (cert_button.state != MENU_BUTTON_STATE_SELECTED)) {
                cert_button.state = MENU_BUTTON_STATE_HOVER;
            } else {
                cert_button.state = MENU_BUTTON_STATE_NONE;
            }
            break;
    }
}

void CApp::OnLButtonDown (int mX, int mY) {
    switch (window) {
        case WIND_MAINMENU: {
            if (((menu_button[menu_hover].x <= mX) && (mX <= (menu_button[menu_hover].x + MENU_BUTTON_WIDTH))) && 
                ((menu_button[menu_hover].y <= mY) && (mY <= (menu_button[menu_hover].y + MENU_BUTTON_HEIGHT)))) {
                menu_selected = menu_hover;
                menu_button[menu_hover].state = MENU_BUTTON_STATE_SELECTED;
            }
        }
        break;
        case WIND_CERTIFICATE:
            if (cert_button.state == MENU_BUTTON_STATE_HOVER) {
                cert_button.state = MENU_BUTTON_STATE_SELECTED;
            }
            break;
        
        case WIND_TUTORIAL:
            if (tut_button.state == MENU_BUTTON_STATE_HOVER) {
                tut_button.state = MENU_BUTTON_STATE_SELECTED;
            }
        case WIND_GAME: {
            /*mX -= GRID_X;
            mY -= GRID_Y;
            int gridx = (int)(mX/GRID_CELLSIZE);
            int gridy = (int)(mY/GRID_CELLSIZE);
            grid[gridy][gridx] = (Tile_Assign)(((int)grid[gridy][gridx].assign_type) + 1);
            if (((int)grid[gridy][gridx].assign_type) >= TILE_TYPES) {
                grid[gridy][gridx] = (Tile_Assign)(((int)grid[gridy][gridx].assign_type) - TILE_TYPES);
            }
            level[currlvl][gridy][gridx] = grid[gridy][gridx];*/
        }
        break;
        
        default: {
        }
        break;
    }
}

void CApp::OnLButtonUp (int mX, int mY) {
    switch (window) {
        case WIND_MAINMENU: {
            bool menu_actually_selected = false;
            if (((menu_button[menu_selected].x <= mX) && (mX <= (menu_button[menu_selected].x + MENU_BUTTON_WIDTH))) && 
                ((menu_button[menu_selected].y <= mY) && (mY <= (menu_button[menu_selected].y + MENU_BUTTON_HEIGHT)))) {
                menu_actually_selected = true;
            }
            if (menu_actually_selected) {
                switch (menu_selected) {
                    case MENU_BUTTON_PLAY:
                        window = WIND_GAME;
                        ResetGame();
                        LoadLevel(currlvl);
                        break;
                    case MENU_BUTTON_TUTORIAL:
                        window = WIND_TUTORIAL;
                        ResetGame();
                        LoadLevel(0);
                        break;
                    case MENU_BUTTON_EXIT:
                        Running = false;
                        break;
                }
            }
            //fprintf(debug.file, "menu_selected: %d, menu_hover: %d\n", menu_selected, menu_hover); fflush(debug.file);
            if (menu_selected != -1) {
                if (menu_selected != menu_hover) {
                    menu_button[menu_selected].state = MENU_BUTTON_STATE_NONE;
                    menu_button[menu_hover].state = MENU_BUTTON_STATE_HOVER;
                    setup_menu_me_anim(menu_hover);
                } else {
                    menu_button[menu_selected].state = MENU_BUTTON_STATE_HOVER;
                }
            }
            menu_selected = -1;
        }
        break;
        
        case WIND_TUTORIAL:
            if (tut_button.state == MENU_BUTTON_STATE_SELECTED) {
                tut_button.state = MENU_BUTTON_STATE_NONE;
                if (((tut_button.x <= mX) && (mX <= (tut_button.x + Text_MainMenuButton_W))) && 
                    ((tut_button.y <= mY) && (mY <= (tut_button.y + Text_MainMenuButton_H)))) {
                    window = WIND_MAINMENU;
                }
            }
            break;
        case WIND_CERTIFICATE:
            if (cert_button.state == MENU_BUTTON_STATE_SELECTED) {
                cert_button.state = MENU_BUTTON_STATE_NONE;
                if (((cert_button.x <= mX) && (mX <= (cert_button.x + Text_MainMenuButton_W))) && 
                    ((cert_button.y <= mY) && (mY <= (cert_button.y + Text_MainMenuButton_H)))) {
                    window = WIND_MAINMENU;
                }
            }
            break;
    }
}

void CApp::OnRButtonDown (int mX, int mY) {
    switch (window) {
        case WIND_TUTORIAL:
        case WIND_GAME: {
            /*mX -= GRID_X;
            mY -= GRID_Y;
            int gridx = (int)(mX/GRID_CELLSIZE);
            int gridy = (int)(mY/GRID_CELLSIZE);
            grid[gridy][gridx] = (Tile_Assign)(((int)grid[gridy][gridx].assign_type) - 1);
            if (((int)grid[gridy][gridx].assign_type) < 0) {
                grid[gridy][gridx] = (Tile_Assign)(((int)grid[gridy][gridx].assign_type) + TILE_TYPES);
            }
            level[currlvl][gridy][gridx] = grid[gridy][gridx];*/
        }
        break;
        default: {
        }
        break;
    }
}

void CApp::OnRButtonUp (int mX, int mY) {
}

void CApp::OnKeyDown (SDL_Scancode scancode, SDL_Keycode sym, SDL_Keymod mod) {
    switch (window) {
        case WIND_TUTORIAL:
        case WIND_GAME: {
            switch (state) {
                case STATE_GAME_MOVE: case STATE_GAME_ANIM:
                    switch (scancode) {
                        case SDL_SCANCODE_UP:
                            nextdir = DIR_UP;
                            break;
                        case SDL_SCANCODE_DOWN:
                            nextdir = DIR_DOWN;
                            break;
                        case SDL_SCANCODE_LEFT:
                            nextdir = DIR_LEFT;
                            break;
                        case SDL_SCANCODE_RIGHT:
                            nextdir = DIR_RIGHT;
                            break;
                        
                        default:
                            break;
                    }
            }
        }
        break;
        default: {
        }
        break;
    }
}

void CApp::OnKeyUp (SDL_Scancode scancode, SDL_Keycode sym, SDL_Keymod mod) {
    switch (window) {
        case WIND_MAINMENU: {
            switch (scancode) {
                case SDL_SCANCODE_ESCAPE:
                    Running = false; //Exit next loop
                    break;
            }
        }
        break;
        
        case WIND_TUTORIAL:
        case WIND_GAME: {
            switch (scancode) {
                case SDL_SCANCODE_R:
                    ResetGame();
                    LoadLevel(currlvl);
                    break;
                
                case SDL_SCANCODE_ESCAPE:
                    window = WIND_MAINMENU;
                    break;
                
                default:
                    break;
            }
        }
        break;
        default: {
        }
        break;
    }
}

bool CApp::playerMove (int direction) {
    int dir;
    if ((direction == DIR_UP) || (direction == DIR_LEFT)) {
        dir = -1;
    } else {
        dir = 1;
    }
    //fprintf(debug.file, "direction: %d, ", direction);
    anim_startx = playerx;
    anim_starty = playery;
    
    int* staticDir;
    int* dynamicDir;
    if ((direction == DIR_UP) || (direction == DIR_DOWN)) {
        staticDir = &playerx;
        dynamicDir = &playery;
    } else {
        staticDir = &playery;
        dynamicDir = &playerx;
    }
    
    int LIMIT;
    if ((direction == DIR_UP) || (direction == DIR_DOWN)) {
        LIMIT = GRID_HEIGHT;
    } else {
        LIMIT = GRID_WIDTH;
    }
    
    bool condition = false;
    (*dynamicDir) += dir;
    for (;((0 <= (*dynamicDir)) && ((*dynamicDir) < LIMIT));(*dynamicDir) += dir) {
        tile_t* gridcell;
        if ((direction == DIR_UP) || (direction == DIR_DOWN)) {
            gridcell = &grid[(*dynamicDir)][(*staticDir)];
        } else {
            gridcell = &grid[(*staticDir)][(*dynamicDir)];
        }
        
        condition = false;
        switch (gridcell->type) {
            case Tile::DESTINATION:
                (*dynamicDir) += dir;
                anim_win_flag = true;
                condition = true;
                fprintf(debug.file, "stuffy stuff\n"); fflush(debug.file);
                break;
            case Tile::BLOCK:
                condition = true;
                break;
            case Tile::REFLECTOR: {
                //fprintf(debug.file, "TILE_REFLECTOR_%d, direction: %d, ", gridcell, direction);
                (*dynamicDir) += dir;
                condition = true;
                for (int i = 0;i < 2;i++) {
                    if (direction == gridcell->srcdir[i]) {
                        anim_reflector_data = gridcell->destdir[i];
                        //fprintf(debug.file, "set, ");
                        break;
                    }
                }
                //If direction was not a source direction
                if (anim_reflector_data == -1) {
                    (*dynamicDir) -= dir;
                }
                //fprintf(debug.file, "anim_reflector_data: %d\n", anim_reflector_data);
            }
            break;
            case Tile::TELEPORTER: {
                condition = true;
                (*dynamicDir) += dir;
                anim_teleporter_flag = true;
                anim_teleporter_data = direction;
            }
            break;
            
            default:
                break;
        }
        
        if (condition) {
            break;
        }
    }
    
    (*dynamicDir) -= dir;
    
    if (!condition) {
        (*dynamicDir) += 10*dir;
        anim_restart_flag = true;
    }
    
    anim_endx = playerx;
    anim_endy = playery;
    playerx = anim_startx;
    playery = anim_starty;
    int xdiff = abs(anim_endx - anim_startx);
    int ydiff = abs(anim_endy - anim_starty);
    anim_totaltime = ((xdiff > ydiff) ? xdiff : ydiff) * ANIM_MOVE_TIME_MULTIPLIER;
    //fprintf(debug.file, "playerx: %d, playery: %d, anim_startx: %d, anim_starty: %d, anim_endx: %d, anim_endy: %d\n",
    //    playerx, playery, anim_startx, anim_starty, anim_endx, anim_endy);
    //fprintf(debug.file, "xdiff: %d, ydiff: %d, anim_totaltime: %d\n", xdiff, ydiff, anim_totaltime);
    
    //reset
    //anim_totaltime = ((xdiff > ydiff) ? xdiff : ydiff) * 2;
    
    return condition;
}
