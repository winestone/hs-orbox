#include "CApp.h"

inline void drawRect (SDL_Renderer* Renderer, int X, int Y, int W, int H, int R, int G, int B, int A) {
    SDL_Rect Rectangle = {X, Y, W, H};
    SDL_SetRenderDrawColor(Renderer, R, G, B, A);
    SDL_RenderDrawRect(Renderer, &Rectangle);
}

inline void draw(SDL_Renderer* Renderer, SDL_Texture* Texture, int X, int Y) {
    CRender::OnDrawTexture(Renderer, Texture, X, Y);
}

inline void draw(SDL_Renderer* Renderer, SDL_Texture* Texture, int X, int Y, int W, int H) {
    CRender::OnDrawTexture(Renderer, Texture, X, Y, W, H);
}

inline void draw(SDL_Renderer* Renderer, SDL_Texture* Texture, int X, int Y, int W, int H, int X2, int Y2, int W2, int H2) {
    CRender::OnDrawTexture(Renderer, Texture, X, Y, W, H, X2, Y2, W2, H2);
}

inline void drawHealthBar (SDL_Renderer* Renderer, SDL_Texture* Text_HealthBack, SDL_Texture* Text_HealthFront, int X, int Y, int hp, int maxhp) {
    draw(Renderer, Text_HealthBack, X, Y);
    int w, h; SDL_QueryTexture(Text_HealthFront, NULL, NULL, &w, &h);
    draw(Renderer, Text_HealthFront,
        X, Y, (int)floor( ((double)hp) / ((double)maxhp) * ((double)w) + 0.5 ), h,
        0, 0, (int)floor( ((double)hp) / ((double)maxhp) * ((double)w) + 0.5 ), h);
}

void CApp::OnRender () {
    
    //Clear screen to black
    //SDL_SetRenderDrawColor(Rend_Display, 0, 0, 0, 0);
    //SDL_RenderClear(Rend_Display);
    
    switch (window) {
        case WIND_LOADING: {
            SDL_SetRenderDrawColor(Rend_Display, 255, 255, 255, 255);
            SDL_RenderClear(Rend_Display);
            screenPrint(100, 100, "Loading");
        }
        break;
        
        case WIND_MAINMENU: {
            draw(Rend_Display, Text_Background, 0, 0);
            
            for (int i = 0;i < MENU_BACK_CELL_NUM;i++) {
                //could precalculate sin(anim_particle[i].constangle) as it doesn't change much
                int spherex = (int)(menu_anim_particle[i].radius * sin(menu_anim_particle[i].angle) * cos(menu_anim_particle[i].constangle));
                int spherey = (int)(menu_anim_particle[i].radius * sin(menu_anim_particle[i].angle) * sin(menu_anim_particle[i].constangle));
                int spherez = (int)(menu_anim_particle[i].radius * cos(menu_anim_particle[i].angle));
                int tempx = spherex;
                int tempy = spherez;
                int particle_x = MENU_BACK_X + tempx + MENU_BACK_POS_HEIGHT_MAX;
                int particle_y = MENU_BACK_Y + tempy + MENU_BACK_POS_HEIGHT_MAX;
                draw(Rend_Display, Text_MenuBackParticle, particle_x, particle_y, menu_anim_particle[i].w, menu_anim_particle[i].h);
            }
            draw(Rend_Display, Text_MenuText, MENU_TEXT_X, MENU_TEXT_Y);
            
            for (int i = 0;i < NUM_MENU_BUTTONS;i++) {
                draw(Rend_Display, Text_MenuButton[i][menu_button[i].state], menu_button[i].x, menu_button[i].y);
            }
            draw(Rend_Display, Text_Player, MENU_BUTTON_ANIM_X, menu_anim_y);
        };
        break;
        
        case WIND_GAME: {
            //Drawing background replaces cleaing screen
            draw(Rend_Display, Text_Background, 0, 0);
            
            for (int i = 0;i < BACK_CELL_NUM;i++) {
                //could precalculate sin(anim_particle[i].constangle) as it doesn't change much
                double spherex = anim_particle[i].radius * sin(anim_particle[i].angle) * cos(anim_particle[i].constangle);
                double spherey = anim_particle[i].radius * sin(anim_particle[i].angle) * sin(anim_particle[i].constangle);
                double spherez = anim_particle[i].radius * cos(anim_particle[i].angle);
                int tempx = (int)(spherez);
                int tempy = (int)(spherex);
                double dist = ((BACK_POS_HEIGHT_MAX/2.0) - spherey)*0.2 + 200;
                int draw_wh = (int)((anim_particle[i].h / (dist*1.0) * (BACK_POS_HEIGHT_MAX*1.0)) + 0.5);
                draw(Rend_Display, Text_BackParticle, (SCREEN_WIDTH/2)+tempx, (SCREEN_HEIGHT/2)+tempy, draw_wh, draw_wh);
            }
            
            //Draw grid
            for (int i = 0;i < GRID_HEIGHT;i++) {
                for (int j = 0;j < GRID_WIDTH;j++) {
                    draw(Rend_Display, Text_Tile[((int)grid[i][j].assign_type)], (GRID_X + (j*GRID_CELLSIZE)), (GRID_Y + (i*GRID_CELLSIZE)));
                }
            }
            
            //Draw player
            draw(Rend_Display, Text_Player, (GRID_X + ((int)(animx))), (GRID_Y + ((int)(animy))));
            //draw(Rend_Display, Text_Player, (GRID_X + playerx*GRID_CELLSIZE), (GRID_Y + playery*GRID_CELLSIZE));
            
        }
        break;
        
        case WIND_TUTORIAL: {
            //Draw background
            draw(Rend_Display, Text_Tutorial_Background, 0, 0);
            
            //Draw grid
            for (int i = 0;i < GRID_HEIGHT;i++) {
                for (int j = 0;j < GRID_WIDTH;j++) {
                    draw(Rend_Display, Text_Tile[((int)grid[i][j].assign_type)], (GRID_X + (j*GRID_CELLSIZE)), (GRID_Y + (i*GRID_CELLSIZE)));
                }
            }
            
            //Draw button
            draw(Rend_Display, Text_MainMenuButton[tut_button.state], tut_button.x, tut_button.y);
            
            //Draw player
            draw(Rend_Display, Text_Player, (GRID_X + ((int)(animx))), (GRID_Y + ((int)(animy))));
        }
        break;
        
        case WIND_CERTIFICATE: {
            draw(Rend_Display, Text_Certificate, 0, 0);
            
            draw(Rend_Display, Text_MainMenuButton[cert_button.state], cert_button.x, cert_button.y);
        }
        break;
        
        default: {
        }
        break;
    }
    
    //Draw a white area for debug
    {
        SDL_Rect temprect = {1200, 0, 100, 600};
        SDL_SetRenderDrawColor(Rend_Display, 255, 255, 255, 255);
        SDL_RenderFillRect(Rend_Display, &temprect);
    }
    //Print debug
    screenPrintWrap(1202, 0, 100, screenDebug);
    
    if (window == WIND_TUTORIAL) {
        //draw(Rend_Display, );
    }
    
    SDL_RenderPresent(Rend_Display);
    
    return;
}
