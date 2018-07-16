#ifndef _CSystem_H_
#define _CSystem_H_

// Definitions
#define WIN32_LEAN_AND_MEAN

// Library Includes:
#include <windows.h>

// Local Includes:
#include "Graphics.h"
#include "Utility.h"
#include "Timer.h"
#include "InputManager.h"

// Class Declaration:
class CSystem
{
	// class functions
	public:	
		~CSystem();

		void Initialise(HINSTANCE _hInstance);
		void Run();

		LRESULT CALLBACK MessageHandler(HWND _hwnd, UINT _uiMsg, WPARAM _wParam, LPARAM _lParam);
		CInputManager* GetInputManager() const;

		// Singleton Methods
		static CSystem* GetInstance();
		static void DestroyInstance();
		static POINT GetMousePos();

	protected:

	private:

		// private constructor (singleton)
		CSystem();

		// privately called functions
		void ExecuteFrame();
		void CreateRegisterWindow();
		void Process(float _fCurTime, float _fDelta);

	// class members
	public:

	protected:

	private:
		// Singleton Instance
		static CSystem* s_pApplication;
		static POINT m_mousePos;

		// System app data
		HWND m_hWnd;
		int m_iWidth;
		int m_iHeight;
		int m_iFPS, m_iCounter;
		float m_fBase;

		// Other data
		HMENU hMenu;
		LPCWSTR m_applicationName;
		HINSTANCE m_hInstance;		

		// Classes used
		CGraphics* m_pGraphics;
		CTimer* m_pTimer;
		CInputManager* m_pInput;
};

// Windows callback definition
static LRESULT CALLBACK WndProc(HWND _hwnd, UINT _uiMsg, WPARAM _wParam, LPARAM _lParam);

#endif // _CSystem_H_