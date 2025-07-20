#ifndef ENXLATE_H
#define ENXLATE_H

#include <fstream>
#include <filesystem>

constexpr int XlateReturnCodeSuccess = 0;

struct XassyXlatInfo {
    int numFiles;
    int numLiterals;
};

typedef struct XlatableIdentifierStruct
{
    std::string Identifier;
    bool HasAm = false;
    bool HasDe = false;
    bool HasFi = false;
    bool HasUa = false;
    std::string am;
    std::string de;
    std::string fi;
    std::string ua;
} XlatableIdentifier;

int Xlate(XassyXlatInfo*);
#endif //ENXLATE_H
