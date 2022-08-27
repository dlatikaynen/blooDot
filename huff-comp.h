#pragma once
#include<fstream>

constexpr int const CharactersInByte = 256;
constexpr int const CodeStackSize = 128;

typedef struct HuffDictionaryEntry
{
    HuffDictionaryEntry* left;
    HuffDictionaryEntry* right;
    long long int frequency;
    unsigned char code;
    
    HuffDictionaryEntry(
        unsigned char c,
        long long int f,
        HuffDictionaryEntry* l = NULL,
        HuffDictionaryEntry* r = NULL
    )
    {
        left = l;
        right = r;
        frequency = f;
        code = c;
    }
} HuffDictionaryEntry;

int HuffCompress(std::ifstream& inFile, std::ofstream& outFile);

void _HuffTreeFill(std::vector<HuffDictionaryEntry*>&, const signed long long, const signed long long);
HuffDictionaryEntry* _HuffDictionaryBuild(std::vector<HuffDictionaryEntry*>&);
void _HuffTreeInsert(std::vector<HuffDictionaryEntry*>&, HuffDictionaryEntry*);
void _HuffTreeBuild(std::vector<HuffDictionaryEntry*>&, signed long long);
void _HuffAddCode(HuffDictionaryEntry*, char[], int, std::vector<long long int>&);
void _HuffAddTree(std::ofstream&, HuffDictionaryEntry*);
HuffDictionaryEntry* _HuffCompressEntry();
void _HuffCompressInternal(std::ifstream&, std::ofstream&, std::vector<long long int>&);