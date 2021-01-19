#include "CApp.h"

void CApp::OnCleanup () {
    
    window = WIND_GAME;
    
    #define free(Texture) if (Texture != NULL) {SDL_DestroyTexture(Texture); Texture = NULL;}
    free(Text_Background);
    free(Text_MenuBackParticle);
    free(Text_BackParticle);
    
    free(Text_MenuText);
    for (int i = 0;i < 3;i++) {
        for (int j = 0;j < NUM_MENU_BUTTONS;j++) {
            free(Text_MenuButton[i][j]);
        }
    }
    
    for (int i = 0;i < 256;i++) {
        free(Text_Character[i]);
    }
    
    free(Text_Tutorial_Background);
    
    free(Text_Certificate);
    for (int i = 0;i < 3;++i) {
        free(Text_MainMenuButton[i]);
    }
    
    free(Text_Player);
    
    for (int i = 0;i < TILE_TYPES;i++) {
        free(Text_Tile[i]);
    }
    #undef free
    
    //fclose(settings);
    
    //Free the window surface
    Surf_Display = NULL;
    
    //Destroy window renderer
    SDL_DestroyRenderer(Rend_Display);
    Rend_Display = NULL;
    
    //Destroy window
    SDL_DestroyWindow(Wind_Display);
    Wind_Display = NULL;
    
    //Quit SDL subsystems
    SDL_Quit();
    
    for (int y = 0;y < GRID_HEIGHT;y++) {
        for (int x = 0;x < GRID_WIDTH;x++) {
            char pc = (char)grid[y][x].assign_type;
            if ((0 <= pc) && (pc <= 9)) {
                pc += '0';
            } else if ((10 <= pc) && (pc <= 36)) {
                pc += 'A';
            }
            fprintf(debug.file, "%c", pc);
        }
        fprintf(debug.file, "\n");
    }
    
    //Cleanup Debug
    fflush(debug.file);
    debug.Cleanup();
    
    return;
}
