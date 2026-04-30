#pragma once
#include <Windows.h>
#include <iostream>

#include <thread>
#include <memory>

#ifdef _DEBUG
extern FILE* f;
extern void createConsole();
extern void destroyConsole();

#define DEBUG_LOG(s) std::cout << s << std::endl;
#else
extern void createConsole();
extern void destroyConsole();

#define DEBUG_LOG(s)
#endif

struct Dll
{
    std::atomic<bool> shouldQuit;
};

extern Dll& getDllInstance();