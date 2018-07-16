// Self Include:
#include "InputManager.h"

// Local Includes:

// Library Includes:

// Implementation:
CInputManager::CInputManager() : m_pDirectInput(nullptr), m_pKeyboardDevice(nullptr), m_pMouseDevice(nullptr), m_iMouseX(0), m_iMouseY(0)
{
	for(unsigned int i = 0; i < 256; ++i)
	{
		m_bKeyToggle[i] = true;
		m_bKeyStatePress[i] = false;
		m_bKeyStateRelease[i] = false;
	}

	for(int i = 0; i < 3; ++i)
	{
		m_bMouseToggle[i] = true;
		m_bMouseStatePress[i] = false;
		m_bMouseStateRelease[i] = false;
	}
}

CInputManager::~CInputManager()
{
	m_pMouseDevice->Unacquire();
	ReleaseCOM(m_pMouseDevice);
	ReleaseCOM(m_pDirectInput);
}

bool CInputManager::Initialise(HINSTANCE _hInstance, HWND _hWnd)
{
	// Create direct input 
	if(FAILED(DirectInput8Create(_hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDirectInput, NULL)))
	{
		return(false);
	}

	// Create mouse device
	if(FAILED(m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMouseDevice, NULL)))
	{
		return(false);
	}

	m_pMouseDevice->SetCooperativeLevel(_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	m_pMouseDevice->SetDataFormat(&c_dfDIMouse);
	m_pMouseDevice->Acquire();

	// Create keyboard device
	if(FAILED(m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboardDevice, NULL)))
	{
		return(false);
	}

	m_pKeyboardDevice->SetCooperativeLevel(_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	m_pKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
	m_pKeyboardDevice->Acquire();

	return(true);
}

void CInputManager::Process()
{
	// Get new mouse state
	HRESULT hResult = m_pMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_tMouseState);

	if(FAILED(hResult))
	{
		// If mouse device has been lost
		if(hResult == DIERR_INPUTLOST || hResult == DIERR_NOTACQUIRED)
		{
			// Re-acquire
			m_pMouseDevice->Acquire();
		}
	}

	// Increase mouse pos
	m_iMouseX += (m_tMouseState.lX);
	m_iMouseY += (m_tMouseState.lY);

	// Read the keyboard device.
	hResult = m_pKeyboardDevice->GetDeviceState(sizeof(m_ucKeyboardState), (LPVOID)&m_ucKeyboardState);

	if(FAILED(hResult))
	{
		// If the keyboard has been lost
		if(hResult == DIERR_INPUTLOST || hResult == DIERR_NOTACQUIRED)
		{
			// Re-acquire
			m_pKeyboardDevice->Acquire();
		}
	}

	// Keyboard states
	// Boolean logic for managing keys down / toggle / keys up
	for(unsigned int i = 0; i < 256; ++i)
	{
		int iDown = m_ucKeyboardState[i] & 0x80;
		m_bKeyStateRelease[i] = false;

		if(iDown && m_bKeyToggle[i])
		{
			m_bKeyToggle[i] = false;
			m_bKeyStatePress[i] = true;
		}
		else 
		{
			m_bKeyStatePress[i] = false;

			if(!m_bKeyToggle[i] && !iDown)
			{
				m_bKeyToggle[i] = true;
				m_bKeyStateRelease[i] = true;
			}
		}
	}

	// Mouse states
	// Boolean logic for mouse down (m1, m2, m3), mouse up etc.
	for(int i = 0; i < 3; ++i)
	{
		int iDown = m_tMouseState.rgbButtons[i] & 0x80;
		m_bMouseStateRelease[i] = false;

		if(iDown && m_bMouseToggle[i])
		{
			m_bMouseToggle[i] = false;
			m_bMouseStatePress[i] = true;
		}
		else 
		{
			m_bMouseStatePress[i] = false;

			if(!m_bMouseToggle[i] && !iDown)
			{
				m_bMouseToggle[i] = true;
				m_bMouseStateRelease[i] = true;
			}
		}
	}

	DIK_ESCAPE;
}

bool CInputManager::IsKeyDown(unsigned int _iKey) const
{
	return(m_ucKeyboardState[_iKey] & 0x80 ? true : false);
}

bool CInputManager::IsKeyPressed(unsigned int _iKey) const
{
	return(m_bKeyStatePress[_iKey]);
}

bool CInputManager::IsKeyReleased(unsigned int _iKey) const
{
	return(m_bKeyStateRelease[_iKey]);
}

bool CInputManager::IsMouseDown(unsigned int _iButton) const
{
	return(m_tMouseState.rgbButtons[_iButton] & 0x80 ? true : false);
}

bool CInputManager::IsMousePressed(unsigned int _iButton) const
{
	return(m_bMouseStatePress[_iButton]);
}

bool CInputManager::IsMouseReleased(unsigned int _iButton) const
{
	return(m_bMouseStateRelease[_iButton]);
}

POINT CInputManager::GetMousePosition() const
{
	POINT p;
	p.x = m_iMouseX;
	p.y = m_iMouseY;
	return(p);
}

POINT CInputManager::GetMouseChange() const
{
	POINT p;
	p.x = m_tMouseState.lX;
	p.y = m_tMouseState.lY;
	return(p);
}