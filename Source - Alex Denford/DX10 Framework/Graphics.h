#pragma once

#ifndef _CGRAPHICSCLASS_H_
#define _CGRAPHICSCLASS_H_

// Library Includes:
#include <windows.h>

// Local Includes:
#include "Utility.h"
#include "DirectX.h"
#include "Camera.h"
#include "Cloth.h"
#include "ClothShader.h"
#include "Floor.h"
#include "Model.h"
#include "InputManager.h"

// Class Declaration:
class CGraphics
{
	// class functions
	public:
		CGraphics();
		~CGraphics();
		void Initialise(HINSTANCE _hInstance, HWND _hWnd, int _iWidth, int _iHeight);

		void Render();		
		void Process(float _fCurTime, float _fDelta);
		void SetFPS(int _iFPS);

		CCamera* GetCamera() const;

	protected:

	private:
		void RenderGUI();
		void CheckGUIAction(int _iX, int _iY);
		void TestIntersection(int _iButton, int _iX, int _iY);
		void CreateSprite(ID3D10Device* _pDevice, D3DX10_SPRITE* _pSprite, LPCWSTR _strTexture, D3DXMATRIX _matProj, float _fWidth, float _fHeight, float _fX, float _fY);
		void FindRodNumber(bool _bIncrease);
		void ProcessObjectSelection();
		void ProcessObjectMovement(float _fDelta);
		void CalculatePlanes(Pyramid* _pPyramid);

	// class members
	public:

	protected:

	private:
		// Window members
		int m_iWidth;
		int m_iHeight;
		HWND m_hWnd;
		int m_iFPS;
		float m_fDelta;

		// Classes used
		CDirectX* m_pDirectXClass;
		CCamera* m_pCameraClass;
		CCloth* m_pCloth;
		CClothShader* m_pClothShader;
		CFloor* m_pFloor;
		Sphere* m_pSphere;
		Pyramid* m_pPyramid;
		Capsule* m_pCapsule;
		Fan* m_pFan;
		CModel* m_pSelected;
		
		// Pointer to input manager
		CInputManager* m_pInput;

		// Font to display text
		ID3DX10Font* m_pFont;

		// Mouse toggle 
		bool m_bMouseMovement;

		// Sprite interface
		ID3DX10Sprite* m_pSpriteInterface;
		D3DX10_SPRITE m_sprites[7];

		// Cloth data
		int m_iClothRods;
		int m_iClothWidth;
		int m_iClothHeight;
};

#endif // _CGRAPHICSCLASS_H_