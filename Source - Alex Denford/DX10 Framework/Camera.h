#ifndef _CCAMERACLASS_H_
#define _CCAMERACLASS_H_

// Library Includes:

// Local Includes:
#include "Utility.h"
#include "InputManager.h"

// Class Declaration:
class CCamera
{
	// class functions
	public:
		CCamera();
		~CCamera();

		bool Initialise(HINSTANCE _hInstance, HWND _hWnd, ID3D10Device* _pD3DDevice);
		void Process(float _fDelta, bool _bDisableMouseMovement, bool _bSkip);
		void Reset();

		// get methods
		D3DXVECTOR3 GetPosition() const;
		D3DXVECTOR3 GetRotation() const;
		D3DXMATRIX GetViewMatrix() const;

		void SetSensitivity(int _iSensitivity);
		int GetSensitivity() const;

	protected:

	private:
		// player movement input
		void ProcessKeyMovement(float fDelta);
		void ProcessMouseMovement(float _fDelta, bool _bSkip);

		// actual movement functions
		void MoveVertical(bool _bMoveUp, float _fDeltaTick);
		void Walk(bool _bMoveNormal, bool _bStrafe, float _fDeltaTick);

	// class members
	public:

	protected:

	private:
		// vectors to create view matrix
		D3DXVECTOR3 m_upVector;
		D3DXVECTOR3 m_position;
		D3DXVECTOR3 m_aimPosition;
		D3DXVECTOR3 m_rotation;

		// Mouse sens
		CInputManager* m_pInput;
		int m_iSensitivity;
		int m_iMouseX;
		int m_iMouseY;

		// view matrix (camera class revolves around this)
		D3DXMATRIX m_viewMatrix;
};

#endif // _CCAMERACLASS_H_