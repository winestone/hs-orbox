#include "CRender.h"

#include <cstdio>
#include <cstring>

CRender::CRender() {
}

SDL_Surface* CRender::LoadSurface (const SDL_PixelFormat* fmt, const char* File) {
    SDL_Surface* Surf_Temp = NULL;
    SDL_Surface* Surf_Return = NULL;
    
    Surf_Temp = SDL_LoadBMP(File);
    if (Surf_Temp == NULL) {
        return NULL;
    }
    
    SDL_PixelFormat format = (*fmt);
    format.Amask = ((0xFFFFFFFF ^ fmt->Rmask) ^ fmt->Gmask) ^ fmt->Bmask;
    
    Surf_Return = SDL_ConvertSurface(Surf_Temp, &format, 0);
    if (Surf_Return == NULL) {
        return NULL;
    }
    
    SDL_FreeSurface(Surf_Temp);
    
    return Surf_Return;
}

SDL_Surface* CRender::LoadSurface (const char* File) {
    SDL_Surface* Surf_Return = NULL;
    
    Surf_Return = SDL_LoadBMP(File);
    if (Surf_Return == NULL) {
        return NULL;
    }
    
    return Surf_Return;
}

SDL_Surface* CRender::OptimiseSurface (const SDL_PixelFormat* fmt, SDL_Surface* Surf_Src) {
    SDL_Surface* Surf_Return = NULL;
    SDL_Surface* Surf_Temp = NULL;
    
    if (Surf_Src == NULL) {
        return NULL;
    }
    Surf_Temp = Surf_Src;
    
    //Guarantee Alpha Channel for 32-bit :L
    SDL_PixelFormat format = (*fmt); //Copy *fmt into format
    //SDL_PixelFormat* format = new SDL_PixelFormat;
    //memcpy(format, fmt, sizeof(SDL_PixelFormat));
    format.Amask = ((0xFFFFFFFF ^ fmt->Rmask) ^ fmt->Gmask) ^ fmt->Bmask;
    
    Surf_Return = SDL_ConvertSurface(Surf_Temp, &format, 0);
    if (Surf_Return == NULL) {
        return NULL;
    }
    
    //delete format;
    
    SDL_FreeSurface(Surf_Temp);
    
    return Surf_Return;
}

SDL_Texture* CRender::LoadTexture (SDL_Renderer* Renderer, const char* File) {
    SDL_Surface* Surf_Temp = NULL;
    SDL_Texture* Text_Return = NULL;
    
    Surf_Temp = SDL_LoadBMP(File);
    if (Surf_Temp == NULL) {
        return NULL;
    }
    
    Text_Return = SDL_CreateTextureFromSurface(Renderer, Surf_Temp);
    if (Text_Return == NULL) {
        return NULL;
    }
    
    SDL_FreeSurface(Surf_Temp);
    
    return Text_Return;
}

SDL_Texture* CRender::LoadTexture (SDL_Renderer* Renderer, SDL_Surface* Surf_Src) {
    SDL_Texture* Text_Return = NULL;
    
    if (Surf_Src == NULL) {
        return NULL;
    }
    
    Text_Return = SDL_CreateTextureFromSurface(Renderer, Surf_Src);
    if (Text_Return == NULL) {
        return NULL;
    }
    
    return Text_Return;
}

SDL_Texture* CRender::LoadTransparentTexture(SDL_Renderer* Renderer, const char* File, int R, int G, int B) {
    SDL_Surface* Surf_Temp = NULL;
    SDL_Texture* Text_Return = NULL;
    
    Surf_Temp = SDL_LoadBMP(File);
    if (Surf_Temp == NULL) {
        return NULL;
    }
    
    SDL_SetColorKey(Surf_Temp, SDL_TRUE, SDL_MapRGB(Surf_Temp->format, R, G, B));
    
    Text_Return = SDL_CreateTextureFromSurface(Renderer, Surf_Temp);
    if (Text_Return == NULL) {
        return NULL;
    }
    
    SDL_FreeSurface(Surf_Temp);
    
    return Text_Return;
}

SDL_Surface* CRender::CloneSurface (SDL_Surface* Surf_Src) {
    SDL_Surface* Surf_Return = NULL;
    
    if (Surf_Src == NULL) {
        return NULL;
    }
    
    Surf_Return = SDL_ConvertSurface(Surf_Src, Surf_Src->format, Surf_Src->flags);
    
    return Surf_Return;
}

bool CRender::Transparent (SDL_Surface* Surf_Src, int R, int G, int B) {
    
    if (Surf_Src == NULL) {
        return false;
    }
    
    SDL_SetColorKey(Surf_Src, SDL_TRUE, SDL_MapRGB(Surf_Src->format, R, G, B));
    
    return true;
}

bool CRender::OnDrawTexture (SDL_Renderer* Rend_Dest, SDL_Texture* Text_Src, int X, int Y) {
    if ((Rend_Dest == NULL) || (Text_Src == NULL)) {
        return false;
    }
    
    int currw, currh;
    SDL_QueryTexture(Text_Src, NULL, NULL, &currw, &currh);
    
    SDL_Rect DestR;
    
    DestR.x = X;
    DestR.y = Y;
    DestR.w = currw;
    DestR.h = currh;
    
    SDL_RenderCopy(Rend_Dest, Text_Src, NULL, &DestR);
    
    return true;
}

bool CRender::OnDrawTexture (SDL_Renderer* Rend_Dest, SDL_Texture* Text_Src, int X, int Y, int W, int H) {
    if ((Rend_Dest == NULL) || (Text_Src == NULL)) {
        return false;
    }
    
    SDL_Rect DestR;
    
    DestR.x = X;
    DestR.y = Y;
    DestR.w = W;
    DestR.h = H;
    
    SDL_RenderCopy(Rend_Dest, Text_Src, NULL, &DestR);
    
    return true;
}

bool CRender::OnDrawTexture (SDL_Renderer* Rend_Dest, SDL_Texture* Text_Src, int X, int Y, int W, int H, int X2, int Y2, int W2, int H2) {
    if ((Rend_Dest == NULL) || (Text_Src == NULL)) {
        return false;
    }
    
    SDL_Rect DestR, SrcR;
    
    DestR.x = X;
    DestR.y = Y;
    DestR.w = W;
    DestR.h = H;
    
    SrcR.x = X2;
    SrcR.y = Y2;
    SrcR.w = W2;
    SrcR.h = H2;
    
    SDL_RenderCopy(Rend_Dest, Text_Src, &SrcR, &DestR);
    
    return true;
}
