#pragma once

#ifndef UTILITY_H
#define UTILITY_H

// Direct input define
#define DIRECTINPUT_VERSION 0x0800

// Library includes
#include <Windows.h>
#include <WindowsX.h>
#include <d3dx10.h>
#include <d3d10.h>
#include <d3dx10math.h>
#include <dinput.h>
#include <dxerr.h>
#include <sstream>

// Links
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

// Convenience macro for releasing COM objects.
#define ReleaseCOM(x) { if(x) { x->Release(); x = nullptr; } }
#define DeleteOBJ(x) {if(x) { delete x; x = nullptr; } }
#define RandomFloat() (rand() / (RAND_MAX + 1.0f))

#endif // UTILITY_H