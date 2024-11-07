#include "pch.h"
#include "huff-comp.h"

long long int _histogram[CharactersInByte] = { 0 };

int HuffCompress(std::ifstream& inFile, long long const uncompressedSize, std::ofstream& outFile)
{
    std::vector<long long int> codeDict;
    codeDict.resize(CharactersInByte);

    char ch;
    std::memset(&_histogram, 0, CharactersInByte);

    while (inFile.get(ch))
    {
        ++_histogram[static_cast<unsigned char>(ch)];
    }

    inFile.clear();
    inFile.seekg(0);
    const auto& tree = new std::vector<std::shared_ptr<HuffDictionaryEntry>>();
    auto root = _HuffCompressEntry(tree);

    char bufferRootFreq[sizeof(long long)] = {};
    char bufferUncompressed[sizeof(long long)] = {};

    *(long long*)bufferRootFreq = root->frequency;
    outFile.write(bufferRootFreq, sizeof(long long));
    *(long long*)bufferUncompressed = uncompressedSize;
    outFile.write(bufferUncompressed, sizeof(long long));
    
    _HuffAddTree(outFile, root);
    char codeStack[CodeStackSize];
    _HuffAddCode(root, codeStack, 0, codeDict);
    _HuffCompressInternal(inFile, outFile, codeDict);
    
    delete tree;

    return 0;
}

void _HuffTreeFill(std::vector<std::shared_ptr<HuffDictionaryEntry>>* entry, const signed long long i, const signed long long length)
{
    auto minimum = i;
    const auto ix2p1 = i * 2 + 1;
    const auto ix2p2 = ix2p1 + 1;

    if (ix2p1 <= length && entry->at(ix2p1)->frequency < entry->at(i)->frequency)
    {
        minimum = ix2p1;
        if (ix2p2 <= length && entry->at(ix2p2)->frequency < entry->at(ix2p1)->frequency)
        {
            minimum = ix2p2;
        }
    }
    else if (ix2p2 <= length && entry->at(ix2p2)->frequency < entry->at(i)->frequency)
    {
        minimum = ix2p2;
    }

    if (minimum != i)
    {
        std::swap(entry[i], entry[minimum]);
        _HuffTreeFill(entry, minimum, length);
    }
}

std::shared_ptr<HuffDictionaryEntry> _HuffDictionaryBuild(std::vector<std::shared_ptr<HuffDictionaryEntry>>* entries)
{
    if (entries->empty())
    {
        return nullptr;
    }

    auto minimum = entries->at(0);
    entries->at(0) = entries->back();
    entries->pop_back();
    _HuffTreeFill(entries, 0, entries->size() - 1);

    return minimum;
}

void _HuffTreeInsert(std::vector<std::shared_ptr<HuffDictionaryEntry>>* entries, std::shared_ptr<HuffDictionaryEntry>& entry)
{
    entries->push_back(entry);
    auto i = entries->size() - 1;
    while (i > 0 && entries->at((i - 1) / 2)->frequency > entries->at(i)->frequency)
    {
        std::swap(entries->at(i), entries->at((i - 1) / 2));
        i = (i - 1) / 2;
    }
}

void _HuffTreeBuild(std::vector<std::shared_ptr<HuffDictionaryEntry>>* entries, signed long long length)
{
    for (auto i = (length - 1) / 2; i >= 0; i--)
    {
        _HuffTreeFill(entries, i, length);
    }
}

void _HuffAddCode(std::shared_ptr<HuffDictionaryEntry>& node, char codeStack[], int index, std::vector<long long int>& codeDict)
{
    if (node->left)
    {
        codeStack[index] = '0';
        _HuffAddCode(node->left, codeStack, index + 1, codeDict);
    }

    if (node->right)
    {
        codeStack[index] = '1';
        _HuffAddCode(node->right, codeStack, index + 1, codeDict);
    }

    if (!node->left && !node->right)
    {
        codeDict[node->code] = 1;
        for (int i = index - 1; i >= 0; i--)
        {
            codeDict[node->code] *= 10;
            codeDict[node->code] += codeStack[i] - '0';
        }
    }
}

void _HuffAddTree(std::ofstream& output, std::shared_ptr<HuffDictionaryEntry>& node)
{
    if (!node->left && !node->right)
    {
        output << '1';
        const char code = static_cast<char>(~node->code);
        output.write(&code, sizeof(char));

        return;
    }

    output << '0';
    _HuffAddTree(output, node->left);
    _HuffAddTree(output, node->right);
}

std::shared_ptr<HuffDictionaryEntry>& _HuffCompressEntry(std::vector<std::shared_ptr<HuffDictionaryEntry>>* dictionary)
{
    dictionary->reserve(CharactersInByte);
    for (auto i = 0; i < CharactersInByte; i++)
    {
        const auto& index = static_cast<unsigned char>(i);
        const auto& freq = _histogram[index];
        if (freq != 0)
        {
            const auto& entry = std::make_shared<HuffDictionaryEntry>(index, freq);
            dictionary->push_back(entry);
        }
    }

    _HuffTreeBuild(dictionary, dictionary->size() - 1);
    while (dictionary->size() != 1)
    {
        auto newEntry = std::make_shared<HuffDictionaryEntry>(
            (unsigned char)0xff,
            (long long int)0,
            _HuffDictionaryBuild(dictionary),
            _HuffDictionaryBuild(dictionary)
        );

        newEntry->frequency = newEntry->left->frequency + newEntry->right->frequency;
        _HuffTreeInsert(dictionary, newEntry);
    }

    return dictionary->at(0);
}

void _HuffCompressInternal(std::ifstream& input, std::ofstream& output, std::vector<long long int>& codeMap)
{
    char ch;
    unsigned char byte = {};
    long long int counter = 0;

    while (input.get(ch))
    {
        auto temp = codeMap[static_cast<unsigned char>(ch)];
        while (temp != 1)
        {
            byte <<= 1;
            if ((temp % 10) != 0)
            {
                byte |= 1;
            }

            temp /= 10;
            counter++;
            if (counter == 8)
            {
                output << byte;
                counter = 0;
                byte = 0;
            }
        }
    }
    
    while (counter != 8)
    {
        byte <<= 1;
        counter++;
    }

    output << byte;
}