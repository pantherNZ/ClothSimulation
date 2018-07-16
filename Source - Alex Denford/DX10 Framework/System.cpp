// Self Include:
#include "System.h"

// Local Includes:
#include "resource.h"

// Library Includes:
#include <sstream>

// Statics
POINT CSystem::m_mousePos;

// Implementation:
CSystem::CSystem() : m_pTimer(nullptr), m_pGraphics(nullptr), m_pInput(nullptr), m_iFPS(0), m_fBase(0.0f), m_iCounter(0)
{

}

CSystem::~CSystem()
{
	// Remove the window
	DestroyWindow(m_hWnd);

	// Remove the application instance
	UnregisterClass(m_applicationName, m_hInstance);

	// Delete used classes
	DeleteOBJ(m_pGraphics);
	DeleteOBJ(m_pTimer);
	DeleteOBJ(m_pInput);
}

void CSystem::Initialise(HINSTANCE _hInstance)
{
	// Basic program settings
	m_applicationName = L"GD2P02 - Cloth Simulation";
	m_hInstance = _hInstance;

	// Initialise the windows api.
	CreateRegisterWindow();

	// Create Input manager
	m_pInput = new CInputManager;

	// Initialise Input
	m_pInput->Initialise(m_hInstance, m_hWnd);

	// Create graphics class
	m_pGraphics = new CGraphics;

	// Initialise graphics class
	m_pGraphics->Initialise(m_hInstance, m_hWnd, m_iWidth, m_iHeight);

	// Create timer 
	m_pTimer = new CTimer;

	// Reset timer
	m_pTimer->Reset();
}

void CSystem::CreateRegisterWindow()
{
	WNDCLASSEX wc;

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);
	
	// Register the window class.
	if(!RegisterClassEx(&wc))
	{
		MessageBox(0, L"Registering Windows Class Failed", 0, 0);
		PostQuitMessage(0);
	}

	// Determine the resolution of the clients desktop screen.
	m_iWidth = GetSystemMetrics(SM_CXSCREEN);
	m_iHeight = GetSystemMetrics(SM_CYSCREEN);

	// Create the window with the screen settings and get the handle to it.
	m_hWnd = CreateWindowEx(NULL, m_applicationName, m_applicationName, 
						WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
						CW_USEDEFAULT, CW_USEDEFAULT, m_iWidth, m_iHeight,
						NULL, NULL, m_hInstance, NULL);

	// check if creating the window failed
	if(!m_hWnd)
	{
		MessageBox(0, L"Creating Windows Class Failed", 0, 0);
		PostQuitMessage(0);
	}

	// load menu in but don't set it
	hMenu = LoadMenu(m_hInstance, MAKEINTRESOURCE(IDR_MENU1));
}

void CSystem::Run()
{
	MSG msg;

	// Initialise the message structure.
	ZeroMemory(&msg, sizeof(MSG));
	
	while(msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			ExecuteFrame();
		}
	}
}

void CSystem::ExecuteFrame()
{
	// Tick the timer
	m_pTimer->Tick();
	float fDelta = m_pTimer->GetDeltaTime();

	static float fTimer = 0.0f;
	fTimer += fDelta;

	if(fTimer >= 1.0f / 60.0f)
	{
		// Process updates & Physics
		Process(m_pTimer->GetGameTime(), fTimer);

		// Render scene
		m_pGraphics->Render();

		fTimer = 0.0f;
	}
}

void CSystem::Process(float _fCurTime, float _fDelta)
{
	// Check if the user pressed escape and wants to exit the application.
	if(GetAsyncKeyState(VK_ESCAPE))
	{
		PostQuitMessage(0);
	}

	// Process input
	m_pInput->Process();

	// Process graphics
	m_pGraphics->Process(_fCurTime, _fDelta);

	// Update fps / timing
	++m_iCounter;

	if((m_pTimer->GetGameTime() - m_fBase) >= 1.0f )
	{
		m_iFPS = m_iCounter;
		m_pGraphics->SetFPS(m_iFPS);

		m_iCounter = 0;
		m_fBase = m_pTimer->GetGameTime();
	}
}

CSystem* CSystem::GetInstance()
{
	if (s_pApplication == nullptr)
	{
		s_pApplication = new CSystem;
	}

	return (s_pApplication);
}

void CSystem::DestroyInstance()
{
	DeleteOBJ(s_pApplication);
}

CInputManager* CSystem::GetInputManager() const
{
	return(m_pInput);
}

POINT CSystem::GetMousePos()
{
	return(m_mousePos);
}

LRESULT CALLBACK CSystem::MessageHandler(HWND _hwnd, UINT _uiMsg, WPARAM _wParam, LPARAM _lParam)
{
	switch(_uiMsg)
	{
		case WM_MOUSEMOVE:
		{
			m_mousePos.x = LOWORD(_lParam);
			m_mousePos.y = HIWORD(_lParam);
			break;
		}
		
		default:
		{
			break;
		}
	}
	
	return(DefWindowProc(_hwnd, _uiMsg, _wParam, _lParam));
}

LRESULT CALLBACK WndProc(HWND _hwnd, UINT _uiMsg, WPARAM _wParam, LPARAM _lParam)
{
	switch(_uiMsg)
	{
		// Check if the window is being destroyed.
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed.
		case WM_CLOSE:
		{
			PostQuitMessage(0);		
			return 0;
		}

		// All other messages pass to the message handler in the system class.
		default:
		{
			return(CSystem::GetInstance()->MessageHandler(_hwnd, _uiMsg, _wParam, _lParam));
		}
	}
}