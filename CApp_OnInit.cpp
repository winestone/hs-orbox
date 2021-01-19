#include "CApp.h"

bool CApp::OnInit () {
    
    //Initialise debug
    debug.Init();
    
    //Makes printing easier
    #define printdi(...) debug.PInd(); fprintf(debug.file, __VA_ARGS__); fflush(debug.file)
    #define printdis(...) debug.PIndS(); fprintf(debug.file, __VA_ARGS__); fflush(debug.file)
    #define printdie(...) debug.PIndE(); fprintf(debug.file, __VA_ARGS__); fflush(debug.file)
    
    printdis("Begin Initialisation.\n");
    
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printdi("SDL could not initialize. SDL_Error: %s\n", SDL_GetError());
    }
    
    //Create window
    Wind_Display = SDL_CreateWindow("CApp", 
                                    SDL_WINDOWPOS_UNDEFINED, 
                                    SDL_WINDOWPOS_UNDEFINED, 
                                    SCREEN_WIDTH, SCREEN_HEIGHT, 
                                    SDL_WINDOW_SHOWN);
    if (Wind_Display == NULL) {
        printdi("Window could not be created. SDL_Error: %s\n", SDL_GetError());
    }
    
    //Get surface for window
    Surf_Display = SDL_GetWindowSurface(Wind_Display);
    
    //Create renderer for window
    Rend_Display = SDL_CreateRenderer(Wind_Display, -1, SDL_RENDERER_ACCELERATED);
    if (Rend_Display == NULL) {
        printdi("Renderer could not be created. SDL_Error: %s\n", SDL_GetError());
    }
    
    //Set texture filtering to linear
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
        printdi("Warning: Linear texture filtering not enabled!");
    }
    
    //printdi("Surf_Display->format: Rmask(0x%08x), Gmask(0x%08x), Bmask(0x%08x), Amask(0x%08x)\n",
    //    Surf_Display->format->Rmask,
    //    Surf_Display->format->Gmask,
    //    Surf_Display->format->Bmask,
    //    Surf_Display->format->Amask);
    
    //Makes loading textures easier
    #define loadsurf(Surface, File) \
        callerMutex.lock(); \
        if ((Surface = CRender::LoadSurface(File)) == NULL) { \
            debug.PInd(); fprintf(debug.file, "Failed loading %s, SDL_Error: %s\n", File, SDL_GetError()); \
        } \
        callerMutex.unlock()
    #define optimise(Surface) \
        callerMutex.lock(); \
        if ((Surface == NULL) || ((Surface = CRender::OptimiseSurface(Surf_Display->format, Surface)) == NULL)) { \
            debug.PInd(); fprintf(debug.file, "Failed optimising %s, SDL_Error: %s\n", #Surface, SDL_GetError()); \
        } \
        callerMutex.unlock()
    #define trans(Surface, R, G, B) \
        if (Surface != NULL) { \
            callerMutex.lock(); \
            CRender::Transparent(Surface, R, G, B); \
            callerMutex.unlock(); \
        }
    #define clone(Destination, Source) \
        if (Source != NULL) { \
            callerMutex.lock(); \
            Destination = CRender::CloneSurface(Source); \
            callerMutex.unlock(); \
        }
    #define convert(Renderer, Texture, Surface) \
        callerMutex.lock(); \
        if ((Surface == NULL) || ((Texture = CRender::LoadTexture(Renderer, Surface)) == NULL)) { \
            debug.PInd(); fprintf(debug.file, "Failed converting %s, SDL_Error: %s\n", #Surface, SDL_GetError()); \
        } \
        callerMutex.unlock()
    #define loadtext(Texture, File) \
        callerMutex.lock(); \
        if ((Texture = CRender::LoadTexture(Rend_Display, File)) == NULL) { \
            debug.PInd(); fprintf(debug.file, "Failed loading %s, SDL_Error: %s\n", File, SDL_GetError()); \
        } \
        callerMutex.unlock();
    #define free(Surface) \
        if (Surface != NULL) { \
            callerMutex.lock(); \
            SDL_FreeSurface(Surface); \
            callerMutex.unlock(); \
            Surface = NULL; \
        }
    
    //Need to define mutex earlier as all SDL functions are wrapped by this as SDL was not designed to be multithreaded
    std::mutex callerMutex;
    
    {//Initialise the character textures
        char dummy[128];
        SDL_Surface* Surf_Temp;
        for (int i = 0;i < 256;i++) {
            sprintf(dummy, "gfx\\Character\\%d.bmp", i);
            loadsurf(Surf_Temp, dummy);
            if (Surf_Temp != NULL) {
                //trans(Surf_Temp, 255, 255, 255);
                convert(Rend_Display, Text_Character[i], Surf_Temp);
                free(Surf_Temp);
            }
        }
    }
    
    #undef free
    #undef convert
    #undef trans
    #undef clone
    #undef optimise
    #undef loadsurf2
    #undef loadsurf
    #undef loadtext
    
    //Initialise screen debugging
    screenDebugPos = sprintf(screenDebug, "Debug:\n");
    
    //Loading screen
    window = WIND_LOADING;
    
    //Start Init thread with a future
    std::future<bool> init = std::async(std::launch::async, &CApp::InitThread, this, std::ref(callerMutex));
    
    //Main loop wrapped with callerMutex lock and unlocks
    SDL_Event Event;
    while (init.wait_for(std::chrono::milliseconds(50)) != std::future_status::ready) {
        callerMutex.lock();
        while (SDL_PollEvent(&Event)) {
            OnEvent(&Event);
            //screenDebugPos += sprintf(screenDebug+screenDebugPos, "event\n");
        }
        //screenDebugPos += sprintf(screenDebug+screenDebugPos, "event loop\n");
        callerMutex.unlock();
        
        callerMutex.lock();
        OnLoop();
        //screenDebugPos += sprintf(screenDebug+screenDebugPos, "loop\n");
        callerMutex.unlock();
        
        callerMutex.lock();
        OnRender();
        //screenDebugPos += sprintf(screenDebug+screenDebugPos, "render\n");
        callerMutex.unlock();
    }
    
    
    //Cleanup extra events
    while (SDL_PollEvent(&Event)) {
        OnEvent(&Event);
        screenDebugPos += sprintf(screenDebug+screenDebugPos, "last event\n");
    }
    
    //Set the starting time things
    Time = SDL_GetTicks();
    PrevTime = Time;
    PrevRunTime = Time;
    
    //Show the main menu
    window = WIND_MAINMENU;
    
    printdie("End Initialisation.\n");
    
    #undef printdi
    #undef printdis
    #undef printdie
    
    return true;
}

bool CApp::InitThread (std::mutex& callerMutex) {
    
    #define printdi(...) debug.PInd(); fprintf(debug.file, __VA_ARGS__); fflush(debug.file)
    #define printdis(...) debug.PIndS(); fprintf(debug.file, __VA_ARGS__); fflush(debug.file)
    #define printdie(...) debug.PIndE(); fprintf(debug.file, __VA_ARGS__); fflush(debug.file)
    
    printdi("Entered thread\n");
    
    //Load images
    #define loadsurf(Surface, File) \
        callerMutex.lock(); \
        if ((Surface = CRender::LoadSurface(File)) == NULL) { \
            debug.PInd(); fprintf(debug.file, "Failed loading %s, SDL_Error: %s\n", File, SDL_GetError()); \
        } \
        callerMutex.unlock()
    #define optimise(Surface) \
        callerMutex.lock(); \
        if ((Surface == NULL) || ((Surface = CRender::OptimiseSurface(Surf_Display->format, Surface)) == NULL)) { \
            debug.PInd(); fprintf(debug.file, "Failed optimising %s, SDL_Error: %s\n", #Surface, SDL_GetError()); \
        } \
        callerMutex.unlock()
    #define trans(Surface, R, G, B) \
        if (Surface != NULL) { \
            callerMutex.lock(); \
            CRender::Transparent(Surface, R, G, B); \
            callerMutex.unlock(); \
        }
    #define clone(Destination, Source) \
        if (Source != NULL) { \
            callerMutex.lock(); \
            Destination = CRender::CloneSurface(Source); \
            callerMutex.unlock(); \
        }
    #define convert(Renderer, Texture, Surface) \
        callerMutex.lock(); \
        if ((Surface == NULL) || ((Texture = CRender::LoadTexture(Renderer, Surface)) == NULL)) { \
            debug.PInd(); fprintf(debug.file, "Failed converting %s, SDL_Error: %s\n", #Surface, SDL_GetError()); \
        } \
        callerMutex.unlock()
    #define loadtext(Texture, File) \
        callerMutex.lock(); \
        if ((Texture = CRender::LoadTexture(Rend_Display, File)) == NULL) { \
            debug.PInd(); fprintf(debug.file, "Failed loading %s, SDL_Error: %s\n", File, SDL_GetError()); \
        } \
        callerMutex.unlock();
    #define free(Surface) \
        if (Surface != NULL) { \
            callerMutex.lock(); \
            SDL_FreeSurface(Surface); \
            callerMutex.unlock(); \
            Surface = NULL; \
        }
    
    //Load the background
    loadtext(Text_Background, "gfx\\Background.bmp");
    
    {//Load the background particles
        SDL_Surface* Surf_Temp;
        //Load the surface
        loadsurf(Surf_Temp, "gfx\\BackParticle.bmp");
        optimise(Surf_Temp);
        {
            //Make variables easier to use
            int pitch = Surf_Temp->pitch/4;
            int width = Surf_Temp->w;
            int height = Surf_Temp->h;
            SDL_PixelFormat* fmt = Surf_Temp->format;
            Uint32* pixels = (Uint32*)Surf_Temp->pixels;
            
            //Set the surface to blend
            SDL_SetSurfaceBlendMode(Surf_Temp, SDL_BLENDMODE_BLEND);
            SDL_SetSurfaceAlphaMod(Surf_Temp, 255);
            
            //Lock the surface for editing
            SDL_LockSurface(Surf_Temp);
            Uint32* currpixel;
            //Get smallest R, G, B mask
            Uint32 smallestmask = fmt->Rmask;
            if (smallestmask > fmt->Gmask) {smallestmask = fmt->Gmask;}
            if (smallestmask > fmt->Bmask) {smallestmask = fmt->Bmask;}
            printdi("Rmask(0x%08x), Gmask(0x%08x), Bmask(0x%08x), Amask(0x%08x)\n", fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
            printdi("smallestmask: 0x%08x\n", smallestmask);
            Uint32 intensity;
            //For each pixel
            for (int i = 0;i < height;i++) {
                debug.PInd();
                fprintf(debug.file, "Intensity[%2d]: ", i);
                //printdi("pixel[y=%2d]: ", i);
                for (int j = 0;j < width;j++) {
                    //Make it easier to access pixel
                    currpixel = &pixels[i*pitch+j];
                    //fprintf(debug.file, "[%2d]:0x%08x ", j, *currpixel);
                    //Set the pixel to be white with a higher alpha if the pixel's smallest is higher
                    intensity = /*(0xFFFFFFFF & smallestmask) -*/ (*currpixel & smallestmask);
                    intensity = (intensity >> 8) + intensity + (intensity << 8) + (intensity << 16) + (intensity << 24);
                    fprintf(debug.file, "[%d]: 0x%08x ", j, intensity);
                    *currpixel = (~(0xFFFFFFFF & fmt->Amask)) + (fmt->Amask & intensity);
                }
                fprintf(debug.file, "\n");
            }
            //Unlock surface, we finished using it
            SDL_UnlockSurface(Surf_Temp);
        }
        //Convert surface to texture
        convert(Rend_Display, Text_BackParticle, Surf_Temp);
        //Free surface
        free(Surf_Temp);
        //Enable blending on texture
        SDL_SetTextureBlendMode(Text_BackParticle, SDL_BLENDMODE_BLEND);
    }
    
    {//Load the menu background particle, same as before
        SDL_Surface* Surf_Temp;
        loadsurf(Surf_Temp, "gfx\\MenuBackParticle.bmp");
        optimise(Surf_Temp);
        {
            int pitch = Surf_Temp->pitch/4;
            int width = Surf_Temp->w;
            int height = Surf_Temp->h;
            SDL_PixelFormat* fmt = Surf_Temp->format;
            Uint32* pixels = (Uint32*)Surf_Temp->pixels;
            
            SDL_SetSurfaceBlendMode(Surf_Temp, SDL_BLENDMODE_BLEND);
            SDL_SetSurfaceAlphaMod(Surf_Temp, 255);
            
            SDL_LockSurface(Surf_Temp);
            Uint32* currpixel;
            Uint32 smallestmask = fmt->Rmask;
            if (smallestmask > fmt->Gmask) {smallestmask = fmt->Gmask;}
            if (smallestmask > fmt->Bmask) {smallestmask = fmt->Bmask;}
            printdi("Rmask(0x%08x), Gmask(0x%08x), Bmask(0x%08x), Amask(0x%08x)\n", fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
            printdi("smallestmask: 0x%08x\n", smallestmask);
            Uint32 intensity;
            for (int i = 0;i < height;i++) {
                debug.PInd();
                fprintf(debug.file, "Intensity[%2d]: ", i);
                //printdi("pixel[y=%2d]: ", i);
                for (int j = 0;j < width;j++) {
                    currpixel = &pixels[i*pitch+j];
                    //fprintf(debug.file, "[%2d]:0x%08x ", j, *currpixel);
                    intensity = /*(0xFFFFFFFF & smallestmask) -*/ (*currpixel & smallestmask);
                    intensity = (intensity >> 8) + intensity + (intensity << 8) + (intensity << 16) + (intensity << 24);
                    fprintf(debug.file, "[%d]: 0x%08x ", j, intensity);
                    *currpixel = (~(0xFFFFFFFF & fmt->Amask)) + (fmt->Amask & intensity);
                }
                fprintf(debug.file, "\n");
            }
            SDL_UnlockSurface(Surf_Temp);
        }
        convert(Rend_Display, Text_MenuBackParticle, Surf_Temp);
        free(Surf_Temp);
        SDL_SetTextureBlendMode(Text_MenuBackParticle, SDL_BLENDMODE_BLEND);
    }
    
    /*
    loadtext(Text_HealthFront, "gfx\\HealthFront.bmp");
    loadtext(Text_HealthBack, "gfx\\HealthBack.bmp");
    
    {SDL_Surface* Surf_Temp[2];
    loadsurf(Surf_Temp[0], "gfx\\Particle.bmp");
    optimise(Surf_Temp[0]);
    clone(Surf_Temp[1], Surf_Temp[0]);
    {
        for (int i = 0;i < 2;i++) {
            int pitch = Surf_Temp[i]->pitch/4;
            int width = Surf_Temp[i]->w;
            int height = Surf_Temp[i]->h;
            SDL_PixelFormat* fmt = Surf_Temp[i]->format;
            Uint32* pixels = (Uint32*)Surf_Temp[i]->pixels;
            
            //SDL_SetSurfaceBlendMode(Surf_Temp[i], SDL_BLENDMODE_BLEND);
            //SDL_SetSurfaceAlphaMod(Surf_Temp[i], 255);
            
            SDL_LockSurface(Surf_Temp[i]);
            Uint32* currpixel;
            Uint32 smallestmask = fmt->Rmask;
            if (smallestmask > fmt->Gmask) {smallestmask = fmt->Gmask;}
            if (smallestmask > fmt->Bmask) {smallestmask = fmt->Bmask;}
            printdi("Rmask(0x%08x), Gmask(0x%08x), Bmask(0x%08x), Amask(0x%08x)\n", fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
            printdi("smallestmask: 0x%08x\n", smallestmask);
            Uint32 intensity;
            for (int j = 0;j < height;j++) {
                debug.PInd();
                fprintf(debug.file, "Intensity[%d][%2d]: ", i, j);
                //printdi("pixel[y=%2d]: ", j);
                for (int k = 0;k < width;k++) {
                    currpixel = &pixels[j*pitch+k];
                    //fprintf(debug.file, "[%2d]:0x%08x ", k, *currpixel);
                    intensity = 255 - (*currpixel & smallestmask);
                    intensity = intensity + (intensity << 8);
                    intensity = intensity + (intensity << 16);
                    fprintf(debug.file, "[%d]: 0x%08x ", k, intensity);
                    switch (i) {
                        case 0:
                            *currpixel = (fmt->Rmask & 0xFFFFFFFF) + (fmt->Amask & intensity);
                            break;
                        case 1:
                            *currpixel = (fmt->Gmask & 0xFFFFFFFF) + (fmt->Amask & intensity);
                            break;
                    }
                }
                fprintf(debug.file, "\n");
            }
            SDL_UnlockSurface(Surf_Temp[i]);
        }
    }
    convert(Rend_Display, Text_ParticleDamage, Surf_Temp[0]);
    convert(Rend_Display, Text_ParticleHeal, Surf_Temp[1]);
    for (int i = 0;i < 2;i++) {
        free(Surf_Temp[i]);
    }}
    SDL_SetTextureBlendMode(Text_ParticleDamage, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(Text_ParticleHeal, SDL_BLENDMODE_BLEND);
    
    {SDL_Surface* Surf_Temp;
    loadsurf(Surf_Temp, "gfx\\Dev.bmp"); //Load the left facing Surf_Temp
    if (Surf_Temp != NULL) { //If successfully loaded
        trans(Surf_Temp, 255, 255, 255); //Transparent some area's
        optimise(Surf_Temp); //Optimise
        convert(Rend_Display, Text_Dev[OBJ_LEFT], Surf_Temp); //Convert it to a texture
        flipSurfaceVertical(Surf_Temp); //Flip the image at the line (x = middle)
        convert(Rend_Display, Text_Dev[OBJ_RIGHT], Surf_Temp); //Convert it to a texture
        free(Surf_Temp); //Free Surf_Temp's used
    }}
    for (int i = 0;i < 2;i++) {
        SDL_SetTextureBlendMode(Text_Dev[i], SDL_BLENDMODE_BLEND);
    }
    */
    {//Load the player texture
        SDL_Surface* Surf_Temp;
        loadsurf(Surf_Temp, "gfx\\Player.bmp");
        if (Surf_Temp != NULL) {
            trans(Surf_Temp, 0, 0, 0);
            convert(Rend_Display, Text_Player, Surf_Temp);
            free(Surf_Temp);
        }
    }
    
    {//Load the menu title
        SDL_Surface* Surf_Temp;
        loadsurf(Surf_Temp, "gfx\\MenuText.bmp");
        if (Surf_Temp != NULL) {
            trans(Surf_Temp, 0, 0, 0);
            convert(Rend_Display, Text_MenuText, Surf_Temp);
            free(Surf_Temp);
        }
    }
    
    //Load the menu button textures
    for (int i = 0;i < NUM_MENU_BUTTONS;i++) {
        for (int j = 0;j < 3;j++) {
            SDL_Surface* Surf_Temp;
            char path[512];
            sprintf(path, "gfx\\Menu\\%d\\%d.bmp", i, j);
            loadsurf(Surf_Temp, path);
            if (Surf_Temp != NULL) {
                trans(Surf_Temp, 0, 0, 0);
                convert(Rend_Display, Text_MenuButton[i][j], Surf_Temp);
                free(Surf_Temp);
            }
        }
    }
    
    loadtext(Text_Tutorial_Background, "gfx\\Tutorial.bmp");
    
    loadtext(Text_Certificate, "gfx\\Certificate\\Certificate.bmp");
    
    //Load the certificate button textures
    for (int i = 0;i < 3;i++) {
        SDL_Surface* Surf_Temp;
        char path[512];
        sprintf(path, "gfx\\MainMenu Button\\%d.bmp", i);
        loadsurf(Surf_Temp, path);
        if (Surf_Temp != NULL) {
            trans(Surf_Temp, 0, 0, 0);
            convert(Rend_Display, Text_MainMenuButton[i], Surf_Temp);
            free(Surf_Temp);
        }
    }
    
    
    {//Load the tile texures
        char temp[128]; SDL_Surface* Surf_Temp;
        for (int i = 0;i < TILE_TYPES;i++) {
            sprintf(temp, "gfx\\Tile\\%d.bmp", i);
            loadsurf(Surf_Temp, temp);
            if (Surf_Temp != NULL) {
                trans(Surf_Temp, 255, 255, 255);
                convert(Rend_Display, Text_Tile[i], Surf_Temp);
                free(Surf_Temp);
            }
            //CRender::LoadTransparentTexture(Rend_Display, temp, 255, 255, 255);
        }
    }
    
    printdi("Loaded all Textures\n");
    
    #undef free
    #undef convert
    #undef trans
    #undef clone
    #undef optimise
    #undef loadsurf2
    #undef loadsurf
    #undef loadtext
    
    //Get a seed to enable randomisation
    srand(time(NULL));
    
    //For all particles
    for (int i = 0;i < BACK_CELL_NUM;i++) {
        //This creates random numbers between 0 and 1
        double r1, r2, r;
        #define newrand() \
        r1 = (double)rand(); \
        r2 = (double)rand(); \
        r = ((r2 > r1) ? (r1/r2) : (r2/r1))
        newrand();
        //Calculate radius randomly
        anim_particle[i].radius = BACK_POS_HEIGHT_MIN + (r * (BACK_POS_HEIGHT_MAX - BACK_POS_HEIGHT_MIN));
        //r = (r/2.0) + 0.15; //normalise r between 0.25 and 0.75
        //anim_particle[i].speed = BACK_CELL_SPEED_MIN + ((1.0/r) * (BACK_CELL_SPEED_MAX - BACK_CELL_SPEED_MIN));
        //Calculate speed slightly randomly
        anim_particle[i].speed = (2.0 * (BACK_CELL_SPEED_DIST_MIN)) / anim_particle[i].radius;
        newrand();
        anim_particle[i].speed += (r * BACK_CELL_SPEED_VARY) - (BACK_CELL_SPEED_VARY / 2.0); //vary everything a bit
        //fprintf(debug.file, "%lf : %lf\n", (2.0 * (BACK_CELL_SPEED_DIST_MIN)) / anim_particle[i].radius, (r * BACK_CELL_SPEED_VARY) - (BACK_CELL_SPEED_VARY / 2.0));
        newrand();
        //Set constangle
        //anim_particle[i].constangle = r * M_PI;
        anim_particle[i].constangle = (70.0/180.0)*M_PI;
        newrand();
        //Randomise starting angle
        anim_particle[i].angle = r * 2.0*M_PI;
        newrand();
        //Randomise height
        anim_particle[i].h = (int)(BACK_CELL_HEIGHT_MIN + (r * (BACK_CELL_HEIGHT_MAX - BACK_CELL_HEIGHT_MIN)));
        //The shape is square
        anim_particle[i].w = anim_particle[i].h;
        #undef newrand
    }
    printdi("Randomised game background particles\n");
    
    //Same as before
    for (int i = 0;i < MENU_BACK_CELL_NUM;i++) {
        double r1, r2, r;
        #define newrand() \
        r1 = (double)rand(); \
        r2 = (double)rand(); \
        r = ((r2 > r1) ? (r1/r2) : (r2/r1))
        newrand();
        //note: exp(x) calculates e^x, therefore exp(1) = e
        menu_anim_particle[i].radius = MENU_BACK_POS_HEIGHT_MIN + (1/(1+pow(exp(1), -24*(r-0.1))))*(MENU_BACK_POS_HEIGHT_MAX - MENU_BACK_POS_HEIGHT_MIN);
        if (menu_anim_particle[i].radius > 0.95) {
            newrand();
            menu_anim_particle[i].radius += (r * MENU_BACK_POS_HEIGHTEDGE_VARY) - (MENU_BACK_POS_HEIGHTEDGE_VARY / 2.0);
        }
        //menu_anim_particle[i].radius = MENU_BACK_POS_HEIGHT_MIN + (r * (MENU_BACK_POS_HEIGHT_MAX - MENU_BACK_POS_HEIGHT_MIN));
        //r = (r/2.0) + 0.15; //normalise r between 0.25 and 0.75
        //anim_particle[i].speed = BACK_CELL_SPEED_MIN + ((1.0/r) * (BACK_CELL_SPEED_MAX - BACK_CELL_SPEED_MIN));
        menu_anim_particle[i].speed = (2.0 * (MENU_BACK_CELL_SPEED_DIST_MIN)) / menu_anim_particle[i].radius;
        newrand();
        menu_anim_particle[i].speed += (r * MENU_BACK_CELL_SPEED_VARY) - (MENU_BACK_CELL_SPEED_VARY / 2.0); //vary everything a bit
        //fprintf(debug.file, "%lf : %lf\n", (2.0 * (BACK_CELL_SPEED_DIST_MIN)) / anim_particle[i].radius, (r * BACK_CELL_SPEED_VARY) - (BACK_CELL_SPEED_VARY / 2.0));
        newrand();
        //anim_particle[i].constangle = r * M_PI;
        menu_anim_particle[i].constangle = (50.0/180.0)*M_PI;
        newrand();
        menu_anim_particle[i].angle = r * 2.0*M_PI;
        newrand();
        menu_anim_particle[i].h = (int)(MENU_BACK_CELL_HEIGHT_MIN + (r * (MENU_BACK_CELL_HEIGHT_MAX - MENU_BACK_CELL_HEIGHT_MIN)));
        menu_anim_particle[i].w = menu_anim_particle[i].h;
        #undef newrand
    }
    printdi("Randomised menu background particles\n");
    
    //For each menu button
    for (int i = 0;i < NUM_MENU_BUTTONS;i++) {
        //Setup location
        menu_button[i].x = MENU_BUTTON_X;
        menu_button[i].y = MENU_BUTTON_Y + i*(MENU_BUTTON_HEIGHT + MENU_BUTTON_BORDER);
        //Not hovered over
        menu_button[i].state = MENU_BUTTON_STATE_NONE;
    }
    //Set play button to be hovered over
    menu_button[MENU_BUTTON_PLAY].state = MENU_BUTTON_STATE_HOVER;
    menu_hover = MENU_BUTTON_PLAY;
    menu_selected = -1;
    menu_me_hover = MENU_BUTTON_PLAY;
    menu_me_selected = -1;
    menu_anim_flag = false;
    menu_anim_y = menu_button[MENU_BUTTON_PLAY].y;
    
    SDL_QueryTexture(Text_MainMenuButton[0], nullptr, nullptr, &Text_MainMenuButton_W, &Text_MainMenuButton_H);
    
    tut_button.x = (SCREEN_WIDTH - Text_MainMenuButton_W) / 2;
    tut_button.y = 350;
    tut_button.state = MENU_BUTTON_STATE_NONE;
    
    cert_button.x = (SCREEN_WIDTH - Text_MainMenuButton_W) / 2;
    cert_button.y = 500;
    cert_button.state = MENU_BUTTON_STATE_NONE;
    
    //Initialise levels
    SetupLevels();
    //Starting level
    currlvl = 1;
    
    //screenDebugPos += sprintf(screenDebug+screenDebugPos, "Pretending to load:\n");
    //for (int i = 10;i > 0;--i) {
    //    screenDebugPos += sprintf(screenDebug+screenDebugPos, "%d\n", i);
    //    std::this_thread::sleep_for(std::chrono::seconds(1));
    //}
    //screenDebugPos += sprintf(screenDebug+screenDebugPos, "Done\n");
    
    printdi("Exited thread\n");
    
    #undef printdi
    #undef printdis
    #undef printdie
    
    return true;
}

void CApp::SetupLevel (int index, const char* string) {
    int teleporter_cache[2][3];//[teleporter id][first one set?, x, y]
    for (int i = 0;i < 2;i++) { teleporter_cache[i][0] = 0; }
    
    int k = 0;
    for (int y = 0;y < GRID_HEIGHT;y++) {
        for (int x = 0;x < GRID_WIDTH;x++) {
            if (('0' <= string[k]) && (string[k] <= '9')) {
                level[index][y][x] = (Tile_Assign)(string[k] - '0');
            } else if (('A' <= string[k]) && (string[k] <= 'Z')) {
                level[index][y][x] = (Tile_Assign)(string[k] - 'A' + 9);
            } else {
                level[index][y][x] = Tile_Assign::EMPTY;
            }
            k++;
            switch (level[index][y][x].type) {
                case Tile::TELEPORTER: {
                    int tel_id = level[index][y][x].id;
                    if (teleporter_cache[tel_id][0] == 0) {
                        teleporter_cache[tel_id][0] = 1;
                        teleporter_cache[tel_id][1] = x;
                        teleporter_cache[tel_id][2] = y;
                        //fprintf(debug.file, "teleporter set[%d] to (%d, %d)\n", tel_id, x, y); fflush(debug.file);
                    } else {
                        level[index][y][x].destx = teleporter_cache[tel_id][1];
                        level[index][y][x].desty = teleporter_cache[tel_id][2];
                        level[index][teleporter_cache[tel_id][2]][teleporter_cache[tel_id][1]].destx = x;
                        level[index][teleporter_cache[tel_id][2]][teleporter_cache[tel_id][1]].desty = y;
                        //fprintf(debug.file, "teleporter [%d] (%d, %d) and (%d, %d)\n", 
                        //    tel_id, x, y, teleporter_cache[tel_id][1], teleporter_cache[tel_id][2]); fflush(debug.file);
                    }
                }
                break;
            }
        }
    }
}

void CApp::SetupLevels () {
    SetupLevel(0,
        "0300000000000000000000000000"
        "3100000000000000000000000060"
        "0000000000000000000000000000"
        "0000000000000000000000090070"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000003000000000000000000"
        "0000000030000000000000000009"
        "0000000000000000000000000000"
        "0800000008000000000000000000"
        "0000000000000000000000000000"
        "0400000007000000000000000000"
    );
    SetupLevel(1,
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000000000000000300000000"
        "0000000000300000000000000000"
        "0000000000000300010000000000"
        "0000000000020000000000000000"
        "0000000000000000000030000000"
        "0000000000000030000000000000"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
    );
    SetupLevel(2,
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000000030000000000000000"
        "0000000000000000300000000000"
        "0000000000000000003000000000"
        "0000000000001000000000000000"
        "0000000000300000000000000000"
        "0000000000000000030000000000"
        "0000000000003000000000000000"
        "0000000000000002000000000000"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
    );
    SetupLevel(3,
        "0000000000000000000000000000"
        "0001000000000003000000000000"
        "0000000000000000030000000000"
        "0000000000003000000000000000"
        "0000000000300000000000000000"
        "0000000000000200000000000000"
        "0000000003000000000000000000"
        "0000000000000000300000000000"
        "0000000000030000000000000000"
        "0000000000000030000000000000"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
    );
    SetupLevel(4,
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000000000030000000000000"
        "0000000000300000000000000000"
        "0000000000000003000000000000"
        "0000000003000000000000000000"
        "0000000000003100000030000000"
        "0000000000000000000023000000"
        "0000000000000000300030000000"
        "0000000000000300000000000000"
        "0000000000030000000000000000"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
    );
    SetupLevel(5,
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000000300000000000000000"
        "0000000030000300000000000000"
        "0000000000030000000000000000"
        "0000000000000000000003000000"
        "0000000002309031000900000000"
        "0000000300000000000000000000"
        "0000000000030000000000300000"
        "0000000000000000000300000000"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
    );
    SetupLevel(6,
        "0000000000300000340000000000"
        "0000000000000300630000000000"
        "0000000000000000000300000000"
        "0000000300000000000300000000"
        "0000000000034000730000300000"
        "0000000010063000650000000000"
        "0000000000000000000003000000"
        "0000003400073000000000000000"
        "0000006300035000000000000000"
        "0000000000000030000030000000"
        "0000000000000000003000000000"
        "0000000000000020000000000000"
        "0000000000034000730000000000"
        "0000000000063000350000000000"
    );
    SetupLevel(7,
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000000000000030000000000"
        "0000000000000030000000000000"
        "0000000000000000000800030000"
        "0000000000000000002000000000"
        "0000000003010009090030000000"
        "0000000000000000003000003000"
        "0000000000000300000000000000"
        "0000000000000000800003000000"
        "0000000000000030000000000000"
        "0000000000000000000300000000"
        "0000000000000000030000000000"
        "0000000000000000000000000000"
    );
    SetupLevel(8,
        "0000003000000000000000000000"
        "0000000000000030000000000000"
        "0000000000000000300000000000"
        "0000000300000000000000000000"
        "0000000000000074030000000000"
        "0000030000300065000000000000"
        "0000000030000000000000000000"
        "0000000000000200000000000000"
        "0000000000000010000000000000"
        "0000000000000000300000000000"
        "0000003000030000000000000000"
        "0000000003000000000000000000"
        "0000000000000003000000000000"
        "0000000300000000000000000000"
    );
    SetupLevel(9,
        "0300000000000000000000000030"
        "0100000000000000000000032000"
        "0000300000000000000000000000"
        "3000000000000000000000000000"
        "0000000000000000000000003000"
        "0003000000000000000000300000"
        "0000030000000000000000000000"
        "0030000000000000000003000000"
        "0000003000000000000000000300"
        "0003000000000000000000000003"
        "0000000000000000000000000000"
        "0000000000000000000000300000"
        "0000030000000000000000000000"
        "0000000300000000000000000000"
    );
    /*SetupLevel(0,
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
        "0000000000000000000000000000"
    );*/
}

void CApp::LoadLevel (int index) {
    for (int y = 0;y < GRID_HEIGHT;y++) {
        for (int x = 0;x < GRID_WIDTH;x++) {
            //fprintf(debug.file, "Copy: level[%d][%d][%d] ", index, i, j); fflush(debug.file);
            grid[y][x] = level[index][y][x];
            //fprintf(debug.file, "Success\n", index, i, j); fflush(debug.file);
            if (grid[y][x].type == Tile::START) {
                playerx = x;
                playery = y;
                animx = ((double)(playerx * GRID_CELLSIZE));
                animy = ((double)(playery * GRID_CELLSIZE));
            }
        }
        //fprintf(debug.file, "\n");
    }
}

void CApp::ResetGame () {
    nextdir = -1;
    
    playerx = 0;
    playery = 0;
    animx = 0.0;
    animy = 0.0;
    
    for (int i = 0;i < GRID_HEIGHT;i++) {
        for (int j = 0;j < GRID_WIDTH;j++) {
            grid[i][j] = Tile_Assign::EMPTY;
        }
    }
    
    anim_time = 0;
    anim_restart_flag = false;
    anim_win_flag = false;
    anim_reflector_data = -1;
    anim_teleporter_flag = false;
    
    state = STATE_GAME_MOVE;
}

void CApp::getLine (FILE* file, char* string) {
    char temp;
    int currpos = 0;
    while (true) {
        fscanf(file, "%c", &temp);
        string[currpos] = temp;
        currpos++;
        if (temp == '\n') {
            string[currpos] = '\0';
            return;
        }
    }
}

bool CApp::flipSurfaceVertical (SDL_Surface* Surf_Src) {
    
    if (Surf_Src == NULL) {
        return false;
    }
    
    //Flip the image at the line (x = middle)
    //SDL_LockSurface(Surf_Temp[0]); //don't think it has to be here
    int pitch = Surf_Src->pitch/4;
    int width = Surf_Src->w;
    int height = Surf_Src->h;
    Uint32* pixels = (Uint32*)Surf_Src->pixels;
    
    SDL_LockSurface(Surf_Src);
    
    for (int i = 0;i < height;i++) {
        for (int j = 0;j < pitch/2;j++) {
            Uint32 temp;
            temp = pixels[i*pitch+j];
            pixels[i*pitch+j] = pixels[i*pitch+width-1-j];
            pixels[i*pitch+width-1-j] = temp;
        }
    }
    
    SDL_UnlockSurface(Surf_Src);
    
    return true;
}

void CApp::screenPrint (int x, int y, const char* string) {
    int length = 0;
    
    //Get the monospace width and height (standardised by the space character...?)
    int textw = 6;
    int texth = 14;
    if (Text_Character[' '] != NULL) {
        SDL_QueryTexture(Text_Character[' '], NULL, NULL, &textw, &texth);
    }
    textw += 2; texth += 2;
    
    for (int i = 0;(unsigned int)i < strlen(string);i++) {
        CRender::OnDrawTexture(Rend_Display, Text_Character[(int)string[i]], (x + length), y);
        length = length + textw;
    }
    
    return;
}

void CApp::screenPrintWrap (int x, int y, int targetw, const char* string) {
    int w = 0, h = 0; //Where we are printing in terms of characters (monospace)
    int length = 0; //length of current line in pixels
    
    //Get the monospace width and height (standardised by the space character...?)
    int textw = 6;
    int texth = 14;
    if (Text_Character[' '] != NULL) {
        SDL_QueryTexture(Text_Character[' '], NULL, NULL, &textw, &texth);
    }
    textw += 2; texth += 2;
    
    char* word = new char[strlen(string)+1];
    int currwordstart;
    int currwordlen = 0;
    
    int currchar = 0;
    bool first = true;
    while (string[currchar] != '\0') {
        //Get current word
        currwordstart = currchar;
        currwordlen = 0;
        //Continue while not space, newline or nul character
        while ((string[currchar] != ' ') && (string[currchar] != '\n') && (string[currchar] != '\0')) {
            word[currwordlen] = string[currchar]; //Append to word
            currwordlen++; //Increase our word length
            currchar++; //Increase our current character
        }
        word[currwordlen] = string[currchar]; //Append to word
        word[currwordlen] = '\0'; //Terminate string
        
        //Increment length
        length += currwordlen * textw;
        
        //Print it if it fits or if it's the first
        if ((length < targetw) || (first)) {
            screenPrint((x + (w * textw)), (y + (h * texth)), word);
        }
        
        //Add word's characters and space to length of things printed
        w += currwordlen + 1;
        
        //No longer first
        if (first) {
            first = false;
        }
        
        //If doesn't fit or newline then
        if ((length > targetw) || (string[currchar] == '\n')) {
            //Move to next line
            first = true;
            length = 0;
            h++;
            w = 0;
        }
        
        if (string[currchar] != '\0') {
            currchar++; //Proceed past the current space/newline character, otherwise end loop
        }
    }
    
    return;
}
