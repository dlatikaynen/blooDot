#pragma once
#include<fstream>

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

int HuffDeflate();

HuffLookup* _HuffTreeFromStorage(std::ifstream&);
int _HuffDeflateInternal(std::ifstream&, HuffLookup*, long long int);