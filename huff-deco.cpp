#include "pch.h"
#include "huff-deco.h"

int HuffDeflate()
{
    std::ifstream inFile("test.ligma", std::ios::binary);
    if (!inFile.good())
    {
        return errno;
    }

    char buffer[sizeof(long long)] = {};
    inFile.read(buffer, sizeof(long long));
    long long histogramSize;
    histogramSize = *(long long*)buffer;
    auto huffmanLookup = _HuffTreeFromStorage(inFile);
    const auto result = _HuffDeflateInternal(inFile, huffmanLookup, histogramSize);
    inFile.close();

    return result;
}

HuffLookup* _HuffTreeFromStorage(std::ifstream& in)
{
    char ch;
    in.get(ch);
    if (ch == '1')
    {
        in.get(ch);
        const auto codeByte = static_cast<unsigned char>(ch);

        return (new HuffLookup(~codeByte, NULL, NULL));
    }
    else
    {
        HuffLookup* left = _HuffTreeFromStorage(in);
        HuffLookup* right = _HuffTreeFromStorage(in);

        return(new HuffLookup(0xff, left, right));
    }
}

int _HuffDeflateInternal(std::ifstream& in, HuffLookup* node, long long int numHistoEntries)
{
    std::ofstream output("deflated.xassy", std::ios::binary);
    if (!output.good())
    {
        return errno;
    }

    auto endOfFile = false;
    char byte = 0;
    auto pointer = node;
    while (in.get(byte))
    {
        auto bitNum = 7;
        while (pointer != NULL && bitNum >= 0 && !endOfFile)
        {
            if (!pointer->left && !pointer->right)
            {
                output << pointer->code;
                --numHistoEntries;
                if (numHistoEntries == 0)
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

    output.close();

    return 0;
}
