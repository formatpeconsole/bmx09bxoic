#include "dllInstance.h"

#ifdef _DEBUG
FILE* f = nullptr;
void createConsole()
{
    AllocConsole();
    freopen_s(&f, "CONOUT$", "w", stdout);
    SetConsoleTitleA("bmx09bxoic");

}
void destroyConsole()
{
    FreeConsole();
    fclose(f);
}

#define DEBUG_LOG(s) std::cout << s << std::endl;
#else
void createConsole() {}
void destroyConsole() {}
#endif