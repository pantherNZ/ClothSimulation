// Self Include:
#include "Graphics.h"

// Local Includes:
#include "System.h"

// Library Includes:

// Implementation:
CGraphics::CGraphics() : m_iFPS(0), m_pFont(nullptr), m_pDirectXClass(nullptr), m_pCameraClass(nullptr), m_pCloth(nullptr), m_pClothShader(nullptr), m_pFloor(nullptr), 
						m_bMouseMovement(false), m_pSpriteInterface(nullptr), m_iClothRods(5), m_iClothWidth(60), m_iClothHeight(30), m_pInput(nullptr),
						m_pSphere(nullptr), m_pPyramid(nullptr), m_pCapsule(nullptr), m_pSelected(nullptr), m_pFan(nullptr)
{										  

}

CGraphics::~CGraphics()
{
	DeleteOBJ(m_pDirectXClass);
	DeleteOBJ(m_pCameraClass);
	DeleteOBJ(m_pCloth);
	DeleteOBJ(m_pClothShader);
	DeleteOBJ(m_pFloor);
	ReleaseCOM(m_pSpriteInterface);
	ReleaseCOM(m_pFont);
	DeleteOBJ(m_pSphere);
	DeleteOBJ(m_pPyramid);
	DeleteOBJ(m_pCapsule);
	DeleteOBJ(m_pFan);
}

void CGraphics::Initialise(HINSTANCE _hInstance, HWND _hWnd, int _iWidth, int _iHeight)
{
	m_hWnd = _hWnd;
	m_iWidth = _iWidth;
	m_iHeight = _iHeight;

	// Store pointer to input manager
	m_pInput = CSystem::GetInstance()->GetInputManager();

	// Create dx10 class
	m_pDirectXClass = new CDirectX;

	// Initalise Dx10 class
	if(!m_pDirectXClass->Initialise(_hWnd, _iWidth, _iHeight))
	{
		MessageBox(NULL, L"Initialising DirectX10 Failed", L"Error", MB_OK);	
		PostQuitMessage(0);
		return;
	}

	// temp store device to create other classes
	ID3D10Device* pDevice = m_pDirectXClass->GetDevice();

	// Create camera
	m_pCameraClass = new CCamera;

	// Initalise camera class
	if(!m_pCameraClass->Initialise(_hInstance, _hWnd, pDevice))
	{
		MessageBox(NULL, L"Initialising Camera Failed", L"Error", MB_OK);
		PostQuitMessage(0);
		return;
	}

	// Create & Initialise cloth shader
	m_pClothShader = new CClothShader;

	if(!m_pClothShader->Initialise(pDevice, _hWnd))
	{
		MessageBox(_hWnd, L"Initialising Cloth Shader Failed", L"Error", MB_OK);
		PostQuitMessage(0);
		return;
	}

	// Create & Initialise cloth
	m_pCloth = new CCloth;

	if(!m_pCloth->Initialise(pDevice, m_iClothWidth, m_iClothHeight, m_iClothRods))
	{
		MessageBox(_hWnd, L"Initialising Cloth Failed", L"Error", MB_OK);
		PostQuitMessage(0);
		return;
	}

	// Create & initialise floor
	m_pFloor = new CFloor;

	if(!m_pFloor->Initialise(_hWnd, pDevice, 60, 60, 10, D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f)))
	{
		MessageBox(_hWnd, L"Initialising Cloth Failed", L"Error", MB_OK);
		PostQuitMessage(0);
		return;
	}

	// Models + data

	// Sphere
	m_pSphere = new Sphere;
	m_pSphere->fRadius = 2.0f;
	m_pSphere->objModel.Initialise(pDevice, "Data\\Models\\Sphere.txt", D3DXVECTOR3(-10.0f, -5.0f, -15.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 
									D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f));
	
	// Capsule
	m_pCapsule = new Capsule;
	m_pCapsule->fRadius = 1.0f;
	m_pCapsule->fHeight = 5.5f;
	m_pCapsule->objModel.Initialise(pDevice, "Data\\Models\\Capsule.txt", D3DXVECTOR3(10.0f, -4.0f, -15.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f),
									D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR4(0.15f, 0.3f, 0.0f, 1.0f));

	// Pyramid
	m_pPyramid = new Pyramid;
	m_pPyramid->fWidth = 4.0f;
	m_pPyramid->fHeight = 2.828f * 2.0f;
	m_pPyramid->objModel.Initialise(pDevice, "Data\\Models\\Pyramid.txt", D3DXVECTOR3(0.0f, -6.0f, -15.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f),
									D3DXVECTOR3(2.0f, 2.0f, 2.0f), D3DXVECTOR4(0.15f, 0.3f, 0.0f, 1.0f));

	// Fan
	m_pFan = new Fan;
	m_pFan->fAngle = 60.0f;
	m_pFan->fStrength = 50.0f;
	m_pFan->fDistance = 30.0f;
	m_pFan->objModel.Initialise(pDevice, "Data\\Models\\Fan.txt", D3DXVECTOR3(-10.0f, -7.0f, 10.0f), D3DXVECTOR3(float(D3DXToRadian(180.0f)), 0.0f, 0.0f),
									D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR4(0.15f, 0.3f, 0.0f, 1.0f));

	// Default selected to sphere
	m_pSelected = &m_pSphere->objModel;
	  
	// Font description for creating font
	D3DX10_FONT_DESC fontDesc;
	fontDesc.Height          = 35;
    fontDesc.Width           = 0;
    fontDesc.Weight          = 0;
    fontDesc.MipLevels       = 1;
    fontDesc.Italic          = false;
    fontDesc.CharSet         = DEFAULT_CHARSET;
    fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
    fontDesc.Quality         = DEFAULT_QUALITY;
    fontDesc.PitchAndFamily  = DEFAULT_PITCH | FF_DONTCARE;
    wcscpy_s(fontDesc.FaceName, L"Imprint MT Shadow");

	// Create font for GUI
	if(FAILED(D3DX10CreateFontIndirect(pDevice, &fontDesc, &m_pFont)))
	{
		MessageBox(NULL, L"Initialising GUI Failed", L"Error", MB_OK);
		PostQuitMessage(0);
		return;
	}

	// Hide cursor
	ShowCursor(false);

	// Create sprite interface
	if(FAILED(D3DX10CreateSprite(pDevice, 0, &m_pSpriteInterface)))
	{
		MessageBox(NULL, L"Initialising Sprite Interface Failed", L"Error", MB_OK);
		PostQuitMessage(0);
		return;
	}

	// Proj matrix
	D3DXMATRIX matProjection = *D3DXMatrixOrthoLH(&matProjection, (float)m_iWidth, (float)m_iHeight, 0.0f, 1.0f);
	m_pSpriteInterface->SetProjectionTransform(&matProjection);

	// Sprite creation / setup
	CreateSprite(pDevice, &m_sprites[0], L"Data\\Images\\Width.png", matProjection, 174.0f, 100.0f, -m_iWidth / 2.0f + 90.0f, m_iHeight / 2.0f - 100.0f);
	CreateSprite(pDevice, &m_sprites[1], L"Data\\Images\\Height.png", matProjection, 174.0f, 100.0f, -m_iWidth / 2.0f + 90.0f, m_iHeight / 2.0f - 210.0f);
	CreateSprite(pDevice, &m_sprites[2], L"Data\\Images\\Rods.png", matProjection, 174.0f, 100.0f, -m_iWidth / 2.0f + 90.0f, m_iHeight / 2.0f - 320.0f);
	CreateSprite(pDevice, &m_sprites[3], L"Data\\Images\\Fan Speed.png", matProjection, 174.0f, 100.0f, -m_iWidth / 2.0f + 90.0f, m_iHeight / 2.0f - 430.0f);
	CreateSprite(pDevice, &m_sprites[4], L"Data\\Images\\Fan Distance.png", matProjection, 174.0f, 100.0f, -m_iWidth / 2.0f + 90.0f, m_iHeight / 2.0f - 540.0f);
	CreateSprite(pDevice, &m_sprites[5], L"Data\\Images\\Fan Angle.png", matProjection, 174.0f, 100.0f, -m_iWidth / 2.0f + 90.0f, m_iHeight / 2.0f - 650.0f);
	CreateSprite(pDevice, &m_sprites[6], L"Data\\Images\\Reset.png", matProjection, 110.0f, 46.0f, -m_iWidth / 2.0f + 90.0f, m_iHeight / 2.0f - 730.0f);
}

void CGraphics::CalculatePlanes(Pyramid* _pPyramid)
{
	// Points on pyramid
	D3DXVECTOR3 vec3PyramidPos = _pPyramid->objModel.GetPosition();
	D3DXVECTOR3 vec3Top = vec3PyramidPos + D3DXVECTOR3(0.0f, _pPyramid->fHeight, 0.0f);
	D3DXVECTOR3 vec3PlanePoint1 = vec3PyramidPos + D3DXVECTOR3(_pPyramid->fWidth / 2.0f, 0.0f, _pPyramid->fWidth / 2.0f);
	D3DXVECTOR3 vec3PlanePoint2 = vec3PyramidPos + D3DXVECTOR3(-_pPyramid->fWidth / 2.0f, 0.0f, _pPyramid->fWidth / 2.0f);
	D3DXVECTOR3 vec3PlanePoint3 = vec3PyramidPos + D3DXVECTOR3(-_pPyramid->fWidth / 2.0f, 0.0f, -_pPyramid->fWidth / 2.0f);
	D3DXVECTOR3 vec3PlanePoint4 = vec3PyramidPos + D3DXVECTOR3(_pPyramid->fWidth / 2.0f, 0.0f, -_pPyramid->fWidth / 2.0f);

	// Side 1
	_pPyramid->tSide[0].vec3Pos = vec3PlanePoint1;
	D3DXVECTOR3 vec3PlaneNormal = *D3DXVec3Cross(&vec3PlaneNormal, &(vec3PlanePoint2 - vec3Top), &(vec3PlanePoint1 - vec3Top));
	D3DXVec3Normalize(&_pPyramid->tSide[0].vec3Normal, &vec3PlaneNormal);

	// Side 2
	_pPyramid->tSide[1].vec3Pos = vec3PlanePoint2;
	D3DXVec3Cross(&vec3PlaneNormal, &(vec3PlanePoint3 - vec3Top), &(vec3PlanePoint2 - vec3Top));
	D3DXVec3Normalize(&_pPyramid->tSide[1].vec3Normal, &vec3PlaneNormal);

	// Side 3
	_pPyramid->tSide[2].vec3Pos = vec3PlanePoint3;
	D3DXVec3Cross(&vec3PlaneNormal, &(vec3PlanePoint4 - vec3Top), &(vec3PlanePoint3 - vec3Top));
	D3DXVec3Normalize(&_pPyramid->tSide[2].vec3Normal, &vec3PlaneNormal);

	// Side 4
	_pPyramid->tSide[3].vec3Pos = vec3PlanePoint4;
	D3DXVec3Cross(&vec3PlaneNormal, &(vec3PlanePoint1 - vec3Top), &(vec3PlanePoint4 - vec3Top));
	D3DXVec3Normalize(&_pPyramid->tSide[3].vec3Normal, &vec3PlaneNormal);

	// Side 5
	_pPyramid->tSide[4].vec3Pos = vec3PlanePoint4;
	vec3PlaneNormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	D3DXVec3Normalize(&_pPyramid->tSide[4].vec3Normal, &vec3PlaneNormal);
}

void CGraphics::Render()
{
	// Begin scene
	m_pDirectXClass->BeginScene();

	// Render cloth
	m_pCloth->ApplyRenderOptions();
	m_pClothShader->Render(m_pCloth->GetWorldMatrix(), m_pCameraClass->GetViewMatrix(), m_pDirectXClass->GetProjectionMatrix(), m_pCloth->GetIndexCount());
	
	// Render floor
	m_pFloor->ApplyRenderOptions();
	m_pClothShader->Render(m_pFloor->GetWorldMatrix(), m_pCameraClass->GetViewMatrix(), m_pDirectXClass->GetProjectionMatrix(), m_pFloor->GetFaceCount() * 3);

	// Render sphere
	m_pSphere->objModel.ApplyRenderOptions();
	m_pClothShader->Render(m_pSphere->objModel.GetWorldMatrix(), m_pCameraClass->GetViewMatrix(), m_pDirectXClass->GetProjectionMatrix(), m_pSphere->objModel.GetFaceCount() * 3);

	// Pyramid
	m_pCapsule->objModel.ApplyRenderOptions();
	m_pClothShader->Render(m_pCapsule->objModel.GetWorldMatrix(), m_pCameraClass->GetViewMatrix(), m_pDirectXClass->GetProjectionMatrix(), m_pCapsule->objModel.GetFaceCount() * 3);

	// Capsule
	m_pPyramid->objModel.ApplyRenderOptions();
	m_pClothShader->Render(m_pPyramid->objModel.GetWorldMatrix(), m_pCameraClass->GetViewMatrix(), m_pDirectXClass->GetProjectionMatrix(), m_pPyramid->objModel.GetFaceCount() * 3);

	// Fan 
	m_pFan->objModel.ApplyRenderOptions();
	m_pClothShader->Render(m_pFan->objModel.GetWorldMatrix(), m_pCameraClass->GetViewMatrix(), m_pDirectXClass->GetProjectionMatrix(), m_pFan->objModel.GetFaceCount() * 3);

	// Switch wireframe off for GUI
	m_pDirectXClass->SetWireFrameMode(false);

	// Render GUI
	RenderGUI();

	// Switch wireframe back on
	m_pDirectXClass->SetWireFrameMode(true);

	// End scene
	m_pDirectXClass->EndScene();
}

void CGraphics::RenderGUI()
{
	// Render FPS
	std::wostringstream outs;   
	outs << L"FPS: " << m_iFPS;
	RECT R = {5, 5, 0, 0};
	m_pFont->DrawText(0, outs.str().c_str(), -1, &R, DT_NOCLIP, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	// Begin rendering GUI sprites
	m_pSpriteInterface->Begin(D3DX10_SPRITE_SORT_TEXTURE);

	// Buttons
	m_pSpriteInterface->DrawSpritesImmediate(&m_sprites[0], 7, 0, 0);

	// End rendering GUI sprites
	m_pSpriteInterface->End();

	// Render cloth width
	std::wostringstream outs2;   
	outs2 << m_iClothWidth;
	RECT R2 = {74, 104, 0, 0};
	m_pFont->DrawText(0, outs2.str().c_str(), -1, &R2, DT_NOCLIP, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	// Render cloth height
	std::wostringstream outs3;   
	outs3 << m_iClothHeight;
	RECT R3 = {74, 214, 0, 0};
	m_pFont->DrawText(0, outs3.str().c_str(), -1, &R3, DT_NOCLIP, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	// Render # rods
	std::wostringstream outs4;   
	outs4 << m_iClothRods;
	RECT R4 = {74, 324, 0, 0};
	m_pFont->DrawText(0, outs4.str().c_str(), -1, &R4, DT_NOCLIP, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	// Render fan speed
	std::wostringstream outs5;   
	outs5 << m_pFan->fStrength;
	RECT R5 = {74, 434, 0, 0};
	m_pFont->DrawText(0, outs5.str().c_str(), -1, &R5, DT_NOCLIP, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	// Render fan distance
	std::wostringstream outs6;   
	outs6 << m_pFan->fDistance;
	RECT R6 = {74, 544, 0, 0};
	m_pFont->DrawText(0, outs6.str().c_str(), -1, &R6, DT_NOCLIP, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	// Render fan angle
	std::wostringstream outs7;   
	outs7 << m_pFan->fAngle;
	RECT R7 = {74, 654, 0, 0};
	m_pFont->DrawText(0, outs7.str().c_str(), -1, &R7, DT_NOCLIP, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
}

void CGraphics::Process(float _fCurTime, float _fDelta)
{
	m_fDelta = _fDelta;

	// Calculate planes for pyramid
	CalculatePlanes(m_pPyramid);

	// Process wind
	D3DXVECTOR3 vec3Force = D3DXVECTOR3(0.0f, 0.0f, 15.0f);

	float fCurrent = m_fDelta;

	// Physics cloth iterations
	while(fCurrent > 0.0f)
	{
		float fDelta = min(fCurrent, 1.0f / 60.0f);
		m_pCloth->Process(fDelta, vec3Force, m_pSphere, m_pPyramid, m_pCapsule, m_pFan);
		fCurrent -= fDelta;
	}
	
	bool bSkip = false;

	// Toggle mouse movement style (FPS camera <-> free mouse)
	if(m_pInput->IsKeyPressed(DIK_LCONTROL))
	{
		m_bMouseMovement = !m_bMouseMovement;
		ShowCursor(m_bMouseMovement);

		if(!m_bMouseMovement) 
		{
			bSkip = true;
		}
	}

	// Lock mouse pos
	if(!m_bMouseMovement)
	{
		//SetCursorPos(m_iWidth / 2, m_iHeight / 2);
	}

	// Process player movement
	m_pCameraClass->Process(_fDelta, m_bMouseMovement, bSkip);

	// Mouse picking
	bool bLeftClick = m_pInput->IsMouseDown(0);
	bool bRightClick = m_pInput->IsMouseDown(1);
	bool bMiddleClick = m_pInput->IsMouseDown(2);

	if((bLeftClick || bRightClick || bMiddleClick) && m_bMouseMovement)
	{
		// Get mouse pos
		POINT mousePos = CSystem::GetMousePos();

		// Check interface click
		if(m_pInput->IsMousePressed(0))
		{
			CheckGUIAction(mousePos.x, mousePos.y);
		}

		int iButton = (bLeftClick ? 0 : (bRightClick ? 1 : 2));

		// Check for intersection on cloth
		TestIntersection(iButton, mousePos.x, mousePos.y);
	}
	else
	{
		m_pCloth->Deselect(true);
	}

	// Hard reset
	if(m_pInput->IsKeyPressed(DIK_R))
	{
		m_iClothWidth = 60;
		m_iClothHeight = 30;
		m_iClothRods = 5;

		DeleteOBJ(m_pCloth);
		m_pCloth = new CCloth;
		m_pCloth->Initialise(m_pDirectXClass->GetDevice(), m_iClothWidth, m_iClothHeight, m_iClothRods);
	}

	// Process object selection
	ProcessObjectSelection();

	// Process object movement
	ProcessObjectMovement(_fDelta);
}

void CGraphics::ProcessObjectSelection()
{
	// 1 Sphere
	if(m_pInput->IsKeyPressed(DIK_1))
	{
		m_pSelected->SetColour(D3DXVECTOR4(0.25f, 0.3f, 0.0f, 1.0f));
		m_pSelected = &m_pSphere->objModel;
		m_pSelected->SetColour(D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f));
	}

	// 2 Pyramid
	if(m_pInput->IsKeyPressed(DIK_2))
	{
		m_pSelected->SetColour(D3DXVECTOR4(0.25f, 0.3f, 0.0f, 1.0f));
		m_pSelected = &m_pPyramid->objModel;
		m_pSelected->SetColour(D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f));
	}

	// 3 Capsule
	if(m_pInput->IsKeyPressed(DIK_3))
	{
		m_pSelected->SetColour(D3DXVECTOR4(0.25f, 0.3f, 0.0f, 1.0f));
		m_pSelected = &m_pCapsule->objModel;
		m_pSelected->SetColour(D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f));
	}

	// 4 Fan
	if(m_pInput->IsKeyPressed(DIK_4))
	{
		m_pSelected->SetColour(D3DXVECTOR4(0.25f, 0.3f, 0.0f, 1.0f));
		m_pSelected = &m_pFan->objModel;
		m_pSelected->SetColour(D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f));
	}
}

void CGraphics::ProcessObjectMovement(float _fDelta)
{
	// Arrow key movement + Page up / down
	float fMoveSpeed = 5.0f;

	// -X (Left)
	if(m_pInput->IsKeyDown(DIK_LEFT))
	{
		m_pSelected->Move(D3DXVECTOR3(-fMoveSpeed * _fDelta, 0.0f, 0.0f));
	}

	// X (Right)
	if(m_pInput->IsKeyDown(DIK_RIGHT))
	{
		m_pSelected->Move(D3DXVECTOR3(fMoveSpeed * _fDelta, 0.0f, 0.0f));
	}

	// -Z (DOWN)
	if(m_pInput->IsKeyDown(DIK_DOWN))
	{
		m_pSelected->Move(D3DXVECTOR3(0.0f, 0.0f, -fMoveSpeed * _fDelta));
	}

	// Z (UP)
	if(m_pInput->IsKeyDown(DIK_UP))
	{
		m_pSelected->Move(D3DXVECTOR3(0.0f, 0.0f, fMoveSpeed * _fDelta));
	}

	// -Y (Page Down)
	if(m_pInput->IsKeyDown(DIK_NEXT))
	{
		m_pSelected->Move(D3DXVECTOR3(0.0f, -fMoveSpeed * _fDelta, 0.0f));
	}

	// Y (Page Up)
	if(m_pInput->IsKeyDown(DIK_PRIOR))
	{
		m_pSelected->Move(D3DXVECTOR3(0.0f, fMoveSpeed * _fDelta, 0.0f));
	}
}

void CGraphics::TestIntersection(int _iButton, int _iX, int _iY)
{
	// Adjust the points using the projection matrix to account for the aspect ratio of the viewport
	D3DXMATRIX projMatrix = m_pDirectXClass->GetProjectionMatrix();

	// Move the mouse cursor coordinates into the -1 to +1 range
	D3DXVECTOR3 vec3Pos;
	vec3Pos.x = (((2.0f * (float)_iX) / (float)m_iWidth) - 1.0f) / projMatrix._11;
	vec3Pos.y = -(((2.0f * (float)_iY) / (float)m_iHeight) - 1.0f) / projMatrix._22;
	vec3Pos.z = 1.0f;

	// Get the inverse of the view matrix
	D3DXMATRIX viewMatrixInv = *D3DXMatrixInverse(&viewMatrixInv, NULL, &m_pCameraClass->GetViewMatrix());

	// Ray 
	D3DXVECTOR3 vec3RayOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vec3RayDirection = vec3Pos;

	// Transform
	D3DXVec3TransformCoord(&vec3RayOrigin, &vec3RayOrigin, &viewMatrixInv);
	D3DXVec3TransformNormal(&vec3RayDirection, &vec3RayDirection, &viewMatrixInv);

	// Pass to cloth for collision checking
	m_pCloth->CheckIntersection(_iButton, vec3RayOrigin, vec3RayDirection);
}

void CGraphics::CheckGUIAction(int _iX, int _iY)
{
	float fX = float(_iX) - m_iWidth / 2.0f;
	float fY = m_iHeight / 2.0f - float(_iY);
	float fBasePosX = 57.0f;
	float fBasePosY = -20.0f;
	float fScale = 20.0f;

	for(int i = 0; i < 6; ++i)
	{
		float fPosXLeft = -fBasePosX + m_sprites[i].matWorld._41;
		float fPosXRight = fBasePosX + m_sprites[i].matWorld._41;
		float fPosY = fBasePosY + m_sprites[i].matWorld._42;
	
		// Check mouse collision for left button
		if(fX >= fPosXLeft - fScale && fX <= fPosXLeft + fScale &&
			fY >= fPosY - fScale && fY <= fPosY + fScale)
		{
			switch(i)
			{
				// Width
				case 0:
					m_iClothWidth -= 2;
					break;

				// Height 
				case 1:
					m_iClothHeight -= 2;				
					break;

				// Reset
				case 2:
					FindRodNumber(false);
					break;

				// Fan speed
				case 3:
					m_pFan->fStrength -= 10.0f;
					break;

				// Fan distance
				case 4:
					m_pFan->fDistance -= 2.0f;
					break;

				// Fan angle
				case 5:
					m_pFan->fAngle -= 5.0f;
					break;
			}
		}

		// Check mouse collision for right button
		if(fX >= fPosXRight - fScale && fX <= fPosXRight + fScale &&
			fY >= fPosY - fScale && fY <= fPosY + fScale)
		{
			switch(i)
			{
				// Width
				case 0:
					m_iClothWidth += 2;
					break;

				// Height
				case 1:
					m_iClothHeight += 2;
					break;

				// Reset
				case 2:
					FindRodNumber(true);
					break;

				// Fan speed
				case 3:
					m_pFan->fStrength += 10.0f;
					break;

				// Fan distance
				case 4:
					m_pFan->fDistance += 2.0f;
					break;

				// Fan angle
				case 5:
					m_pFan->fAngle += 5.0f;
					break;
			}
		}

		// Limits
		m_iClothWidth = max(5, min(100, m_iClothWidth));
		m_iClothHeight = max(5, min(100, m_iClothHeight));
		m_iClothRods = max(0, min(m_iClothWidth, m_iClothRods));
		m_pFan->fDistance = max(0.0f, m_pFan->fDistance);
		m_pFan->fStrength = max(0.0f, m_pFan->fStrength);
		m_pFan->fAngle = max(10.0f, min(90.0f, m_pFan->fAngle));
	}

	// Reset button
	float fPosX = m_sprites[6].matWorld._41;
	float fPosY = m_sprites[6].matWorld._42;
	float fScaleX = m_sprites[6].matWorld._11 / 2.0f;
	float fScaleY = m_sprites[6].matWorld._22 / 2.0f;

	if(fX >= fPosX - fScaleX && fX <= fPosX + fScaleX &&
		fY >= fPosY - fScaleY && fY <= fPosY + fScaleY)
	{
		DeleteOBJ(m_pCloth);
		m_pCloth = new CCloth;
		m_pCloth->Initialise(m_pDirectXClass->GetDevice(), m_iClothWidth, m_iClothHeight, m_iClothRods);
	}
}

void CGraphics::FindRodNumber(bool _bIncrease)
{
	if(!_bIncrease && m_iClothRods == 1)
	{
		m_iClothRods = 0;
		return;
	}

	while(true)
	{
		m_iClothRods += (_bIncrease ? 1 : -1);

		if(m_iClothWidth % m_iClothRods == 0)
		{
			return;
		}

		if(m_iClothRods >= m_iClothWidth)
		{
			m_iClothRods = 1;
			return;
		}
	}
}

void CGraphics::SetFPS(int _iFPS)
{
	m_iFPS = _iFPS;
}

CCamera* CGraphics::GetCamera() const
{
	return(m_pCameraClass);
}

void CGraphics::CreateSprite(ID3D10Device* _pDevice, D3DX10_SPRITE* _pSprite, LPCWSTR _strTexture, D3DXMATRIX _matProj, float _fWidth, float _fHeight, float _fX, float _fY)
{
	// Create sprite
	ID3D10ShaderResourceView* pTexture;
	if(FAILED(D3DX10CreateShaderResourceViewFromFile(_pDevice, _strTexture, 0, 0, &pTexture, 0)))
	{
		MessageBox(NULL, L"Initialising Sprite Textures Failed", L"Error", MB_OK);	
		PostQuitMessage(0);
		return;
	}

	// Standard settings
	_pSprite->pTexture = pTexture;
	_pSprite->TexCoord = D3DXVECTOR2(0.0f, 0.0f);
	_pSprite->TexSize = D3DXVECTOR2(1.0f, 1.0f);
	_pSprite->TextureIndex = 0;
	_pSprite->ColorModulate = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// World matrix
	D3DXMATRIX matScale = *D3DXMatrixScaling(&matScale, _fWidth, _fHeight, 1.0f);
	D3DXMATRIX matTranslation = *D3DXMatrixTranslation(&matTranslation, _fX, _fY, 0.1f);
	_pSprite->matWorld = matScale * matTranslation;
}