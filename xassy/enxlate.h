#ifndef ENXLATE_H
#define ENXLATE_H
#ifndef NDEBUG

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
    bool HasAm;
    bool HasDe;
    bool HasFi;
    bool HasUa;
    std::string am;
    std::string de;
    std::string fi;
    std::string ua;
} XlatableIdentifier;

int Xlate(XassyXlatInfo*);
#endif
#endif //ENXLATE_H
