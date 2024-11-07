#pragma once
#include<fstream>

constexpr int const CharactersInByte = 256;
constexpr int const CodeStackSize = 128;

typedef struct HuffDictionaryEntry
{
    std::shared_ptr<HuffDictionaryEntry> left;
    std::shared_ptr<HuffDictionaryEntry> right;
    long long int frequency;
    unsigned char code;
    
    HuffDictionaryEntry(
        unsigned char c,
        long long int f,
        std::shared_ptr<HuffDictionaryEntry> l = nullptr,
        std::shared_ptr<HuffDictionaryEntry> r = nullptr
    )
    {
        left = l;
        right = r;
        frequency = f;
        code = c;
    }
} HuffDictionaryEntry;

int HuffCompress(std::ifstream& inFile, long long const uncompressedSize, std::ofstream& outFile);

void _HuffTreeFill(std::vector<std::shared_ptr<HuffDictionaryEntry>>*, const signed long long, const signed long long);
std::shared_ptr<HuffDictionaryEntry> _HuffDictionaryBuild(std::vector<std::shared_ptr<HuffDictionaryEntry>>&);
void _HuffTreeInsert(std::vector<std::shared_ptr<HuffDictionaryEntry>>&, HuffDictionaryEntry*);
void _HuffTreeBuild(std::vector<std::shared_ptr<HuffDictionaryEntry>>*, signed long long);
void _HuffAddCode(std::shared_ptr<HuffDictionaryEntry>&, char[], int, std::vector<long long int>&);
void _HuffAddTree(std::ofstream&, std::shared_ptr<HuffDictionaryEntry>&);
std::shared_ptr<HuffDictionaryEntry>& _HuffCompressEntry(std::vector<std::shared_ptr<HuffDictionaryEntry>>* dictionary);
void _HuffCompressInternal(std::ifstream&, std::ofstream&, std::vector<long long int>&);