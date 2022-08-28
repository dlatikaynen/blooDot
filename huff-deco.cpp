#include "pch.h"
#include "huff-deco.h"
#include <SDL.h>

void* HuffInflate(SDL_RWops* inFile, long long const sourceSize, long long* originalSize, long long* actuallyRead)
{
    long long histogramSize;
    long long storedSize;
    char bufferRootFreq[sizeof(long long)] = {};
    char bufferSize[sizeof(long long)] = {};
    auto countdownSize = sourceSize;
    (*actuallyRead) = 0;

    inFile->read(inFile, &bufferRootFreq, sizeof(long long), 1);
    histogramSize = *(long long*)bufferRootFreq;

    inFile->read(inFile, &bufferSize, sizeof(long long), 1);
    storedSize = *(long long*)bufferSize;

    auto huffmanLookup = _HuffTreeFromStorage(inFile, &countdownSize);
    auto result = (unsigned char*)SDL_malloc(storedSize);
    if (!result)
    {
        const auto decompAllocError = SDL_GetError();
        ReportError("Failed to allocate memory for resource extraction", decompAllocError);
        return NULL;
    }

    memset(result, 0, storedSize);
    (*actuallyRead) = _HuffInflateInternal(inFile, &result, huffmanLookup, histogramSize, &countdownSize, storedSize);
    (*originalSize) = storedSize;

    return result;
}

HuffLookup* _HuffTreeFromStorage(SDL_RWops* in, long long* pCountdownSize)
{
    char ch;
    (*pCountdownSize) -= in->read(in, &ch, sizeof(char), 1);
    if (ch == '1')
    {
        in->read(in, &ch, sizeof(char), 1);
        const auto& codeByte = static_cast<unsigned char>(ch);

        return (new HuffLookup(~codeByte, NULL, NULL));
    }
    else
    {
        HuffLookup* left = _HuffTreeFromStorage(in, pCountdownSize);
        HuffLookup* right = _HuffTreeFromStorage(in, pCountdownSize);

        return(new HuffLookup(0xff, left, right));
    }
}

long long _HuffInflateInternal(SDL_RWops* in, unsigned char** result, HuffLookup* node, long long int numHistoEntries, long long* countdownSize, long long expectedOutputSize)
{
    auto endOfFile = false;
    char byte = 0;
    auto pointer = node;
    auto output = *result;
    long long actuallyExtracted = 0;

    while ((*countdownSize) > 0 && in->read(in, &byte, sizeof(byte), 1))
    {
        --(*countdownSize);

        auto bitNum = 7;
        while (pointer != NULL && bitNum >= 0 && !endOfFile)
        {
            if (!pointer->left && !pointer->right)
            {
                *output = pointer->code;
                ++output;
                ++actuallyExtracted;

                if (--numHistoEntries == 0 || actuallyExtracted >= expectedOutputSize)
                {
                    endOfFile = true;
                    break;
                }

                pointer = node;
                continue;
            }

            if ((byte & (1 << bitNum)))
            {
                pointer = pointer->right;
                --bitNum;
            }
            else
            {
                pointer = pointer->left;
                --bitNum;
            }
        }
    }

    return actuallyExtracted;
}
