// Self Include:
#include "Camera.h"

// Local Includes:
#include "System.h"

// Library Includes:

// Implementation:
CCamera::CCamera() : m_pInput(nullptr), m_iMouseX(0), m_iMouseY(0)
{
	Reset();
}

CCamera::~CCamera()
{
	
}

bool CCamera::Initialise(HINSTANCE _hInstance, HWND _hWnd, ID3D10Device* _pD3DDevice)
{
	D3DXMatrixLookAtLH(&m_viewMatrix, &m_position, &m_aimPosition, &m_upVector);

	m_iSensitivity = 5;

	m_pInput = CSystem::GetInstance()->GetInputManager();

	return(true);
}

void CCamera::Process(float _fDelta, bool _bDisableMouseMovement, bool _bSkip)
{
	// Process key movement
	ProcessKeyMovement(_fDelta);

	if(!_bDisableMouseMovement)
	{
		// Process mouse movement
		ProcessMouseMovement(_fDelta, _bSkip);
	}

	// Recreate view matrix
	D3DXMatrixLookAtLH(&m_viewMatrix, &m_position, &m_aimPosition, &m_upVector);
}

void CCamera::ProcessKeyMovement(float _fDelta)
{
	// if W pushed
	if(m_pInput->IsKeyDown(DIK_W))
	{
		Walk(true, false, _fDelta);
	}
	// if A pushed
	if(m_pInput->IsKeyDown(DIK_A))
	{
		Walk(false, true, _fDelta);
	}
	// if S pushed
	if(m_pInput->IsKeyDown(DIK_S))
	{
		Walk(false, false, _fDelta);
	}		
	// if D pushed
	if(m_pInput->IsKeyDown(DIK_D))
	{
		Walk(true, true, _fDelta);
	}
	// if SPACE pushed
	if(m_pInput->IsKeyDown(DIK_SPACE))
	{
		MoveVertical(true, _fDelta);
	}
	// if SHIFT pushed
	if(m_pInput->IsKeyDown(DIK_LSHIFT))
	{
		MoveVertical(false, _fDelta);
	}
}

void CCamera::ProcessMouseMovement(float _fDelta, bool _bSkip)
{
	if(!_bSkip)
	{
		POINT mouse = m_pInput->GetMouseChange();
		 m_iMouseX += mouse.x;
		 m_iMouseY += mouse.y;

		// limit the mouse movement on the Y axis
		int iLimit = 60 * m_iSensitivity;

		m_iMouseY = min(m_iMouseY, iLimit);
		m_iMouseY = max(m_iMouseY, -iLimit);

		m_aimPosition.x = 0;
		m_aimPosition.y = 0;
		m_aimPosition.z = 1;

		m_upVector.x = 0;
		m_upVector.y = 1;
		m_upVector.z = 0;

		m_rotation.x = (float)m_iMouseY / m_iSensitivity;
		m_rotation.y = (float)m_iMouseX / m_iSensitivity; 

		float fPitch = (float)D3DXToRadian(m_rotation.x);
		float fYaw = (float)D3DXToRadian(m_rotation.y);

		D3DXMATRIX rotationMatrix = *D3DXMatrixRotationYawPitchRoll(&rotationMatrix, fYaw, fPitch, 0);

		D3DXVec3TransformCoord(&m_aimPosition, &m_aimPosition, &rotationMatrix);
		D3DXVec3TransformCoord(&m_upVector, &m_upVector, &rotationMatrix);

		m_aimPosition.x += m_position.x;
		m_aimPosition.y += m_position.y;
		m_aimPosition.z += m_position.z;
	}
}

void CCamera::Walk(bool _bMoveNormal, bool _bStrafe, float _fDeltaTick)
{
	// speed
	float fMoveSpeed = 40.0f * _fDeltaTick * (_bMoveNormal ? 1.0f : -1.0f);

	// calculate change in position
	float fAngle = (float)D3DXToRadian(m_rotation.y);
	float iX = (_bStrafe ? cos(-fAngle) : sin(fAngle)) * fMoveSpeed;
	float iY = (_bStrafe ? sin(-fAngle) : cos(fAngle)) * fMoveSpeed;

	// add to position
	m_position.x += iX;
	m_position.z += iY;

	// also add to the aim position
	m_aimPosition.x += iX;
	m_aimPosition.z += iY;
}

void CCamera::MoveVertical(bool _bMoveUp, float _fDeltaTick)
{
	// speed
	float fMoveSpeed = 20.0f * _fDeltaTick * (_bMoveUp ? 1.0f : -1.0f);

	// add to both position's & aim position's height (Y)
	m_position.y += fMoveSpeed;
	m_aimPosition.y += fMoveSpeed;
}

void CCamera::Reset()
{
	m_iMouseX = 0;
	m_iMouseY = 0;

	m_upVector = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_position = D3DXVECTOR3(0.0f, 0.0f, -50.0f);
	m_aimPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	m_rotation.x = 0.0f;
	m_rotation.y = 0;
	m_rotation.z = 0;
}

D3DXVECTOR3 CCamera::GetPosition() const
{
	return(m_position);
}

D3DXVECTOR3 CCamera::GetRotation() const
{
	return(m_rotation);
}

D3DXMATRIX CCamera::GetViewMatrix() const
{
	return(m_viewMatrix);
}

void CCamera::SetSensitivity(int _iSensitivity)
{
	m_iSensitivity = _iSensitivity;
}

int CCamera::GetSensitivity() const
{
	return(m_iSensitivity);
}