#ifndef DEXASSY_H
#define DEXASSY_H

#include <SDL3/SDL.h>

constexpr auto XassyFile = "xassy-cooked.1.ngld";
constexpr char bom[] = { static_cast<char>(0xEF), static_cast<char>(0xBB), static_cast<char>(0xBF) };
constexpr auto signature = "\4LSL\6JML\5";
constexpr auto lenBom = sizeof(bom);

int constexpr length(const char* str)
{
    return *str ? 1 + length(str + 1) : 0;
}

constexpr auto lenSignature = length(signature);

extern void ReportError(const char*, const char*);

void PrepareIndex();
bool OpenCooked();
void* Retrieve(int chunkKey, SDL_IOStream** stream);
void CloseCooked();

#endif //DEXASSY_H
