#ifndef _CRENDER_H_
    #define _CRENDER_H_

#include <SDL2\SDL.h>

//Nice renderering library I made, inspired a long long time ago by http://www.sdltutorials.com/
class CRender {
    public:
        CRender();
    
    public:
        static SDL_Surface* LoadSurface(const SDL_PixelFormat* fmt, const char* File);
        static SDL_Surface* LoadSurface(const char* File);
        static SDL_Surface* OptimiseSurface(const SDL_PixelFormat* fmt, SDL_Surface* Surf_Src);
        static SDL_Texture* LoadTexture(SDL_Renderer* Renderer, const char* File);
        static SDL_Texture* LoadTexture(SDL_Renderer* Renderer, SDL_Surface* Surf_Src);
        //LoadTransparentTexture is obselete soon
        static SDL_Texture* LoadTransparentTexture(SDL_Renderer* Renderer, const char* File, int R, int G, int B);
        static SDL_Surface* CloneSurface(SDL_Surface* Surf_Src);
        static bool Transparent(SDL_Surface* Surf_Src, int R, int G, int B);
        
        static bool OnDrawTexture(SDL_Renderer* Rend_Dest, SDL_Texture* Text_Src, int X, int Y);
        static bool OnDrawTexture(SDL_Renderer* Rend_Dest, SDL_Texture* Text_Src, int X, int Y, int W, int H);
        static bool OnDrawTexture(SDL_Renderer* Rend_Dest, SDL_Texture* Text_Src, int X, int Y, int W, int H, int X2, int Y2, int W2, int H2);
};

#endif
