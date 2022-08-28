#pragma once
#include<fstream>
#include <SDL.h>

typedef struct HuffLookup
{
    HuffLookup* left;
    HuffLookup* right;
    unsigned char code;
    
    HuffLookup(unsigned char c, HuffLookup* l, HuffLookup* r)
    {
        left = l;
        right = r;
        code = c;
    }
} HuffLookup;

extern void ReportError(const char*, const char*);

void* HuffInflate(SDL_RWops* inFile, long long const sourceSize, long long* originalSize, long long* actuallyRead);

HuffLookup* _HuffTreeFromStorage(SDL_RWops* in, long long* countdownSize);
long long _HuffInflateInternal(SDL_RWops* in, unsigned char** result, HuffLookup* node, long long int numHistoEntries, long long* countdownSize, long long expectedOutputSize);