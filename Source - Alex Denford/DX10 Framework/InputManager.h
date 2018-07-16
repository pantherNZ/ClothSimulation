#pragma once

#ifndef _CINPUTMANAGER_H_
#define _CINPUTMANAGER_H_

// Library Includes:

// Local Includes:
#include "Utility.h"

// Class Declaration:
class CInputManager
{
	// class functions
	public:
		CInputManager();
		~CInputManager();
		bool Initialise(HINSTANCE _hInstance, HWND _hWnd);
		void Process();

		bool IsKeyDown(unsigned int _iKey) const;
		bool IsKeyPressed(unsigned int _iKey) const;
		bool IsKeyReleased(unsigned int _iKey) const;
		bool IsMouseDown(unsigned int _iButton) const;
		bool IsMousePressed(unsigned int _iButton) const;
		bool IsMouseReleased(unsigned int _iButton) const;
		POINT GetMousePosition() const;
		POINT GetMouseChange() const;

	protected:

	private:

	// class members
	public:

	protected:

	private:
		// Devices / interface
		IDirectInput8* m_pDirectInput;
		IDirectInputDevice8* m_pKeyboardDevice;
		IDirectInputDevice8* m_pMouseDevice;

		// Keyboard booleans / data
		bool m_bKeyToggle[256];
		bool m_bKeyStatePress[256];
		bool m_bKeyStateRelease[256];
		unsigned char m_ucKeyboardState[256];

		// Mouse booleans / data
		bool m_bMouseToggle[3];
		bool m_bMouseStatePress[3];
		bool m_bMouseStateRelease[3];
		bool m_bMouseState[3];
		DIMOUSESTATE m_tMouseState;

		// Cumulative mouse pos
		int m_iMouseX;	
		int m_iMouseY;
};

#endif // _CINPUTMANAGER_H_