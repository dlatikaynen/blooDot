#ifndef ENXASSY_H
#define ENXASSY_H

constexpr int CookReturnCodeSuccess = 0;
constexpr int CookReturnCodeRecipeFail = 0xe1;
constexpr int CookReturnCodeCopyFail = 0xe2;
constexpr int CookReturnCodeInputFileNotFound = 0xe3;

struct XassyCookInfo {
    int numFiles;
    uintmax_t numBytesBefore;
    uintmax_t numBytesAfter;
};

int Cook(XassyCookInfo* cookStats);
#endif //ENXASSY_H
