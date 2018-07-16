// Library Includes:
#include <time.h>
#include <vld.h>

// Local Includes:
#include "Utility.h"
#include "System.h"

// Initialise pointer of instance
CSystem* CSystem::s_pApplication = nullptr;

// Winmain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	// Seed rand
	srand((unsigned int)(time(0)));

	// Create system class instance 
	CSystem* pSystemClass = CSystem::GetInstance();

	// Initialise system class
	pSystemClass->Initialise(hInstance);

	// Run the game
	pSystemClass->Run();

	// Destroy system class instance
	CSystem::DestroyInstance();

	return(0);
}