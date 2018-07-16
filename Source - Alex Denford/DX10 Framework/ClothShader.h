#pragma once

#ifndef _CCLOTHSHADERCLASS_H_
#define _CCLOTHSHADERCLASS_H_

// Library Includes:

// Local Includes:
#include "Utility.h"

// Class Declaration:
class CClothShader
{
	// class functions
	public:
		CClothShader();
		~CClothShader();

		bool Initialise(ID3D10Device* _pD3DDevice, HWND _hWnd);
		void Render(D3DXMATRIX _worldMatrix, D3DXMATRIX _viewMatrix, D3DXMATRIX _projectionMatrix, int _iIndexCount);

		
	protected:

	private:
		void OutputShaderErrorMessage(ID3D10Blob* _pErrorMessage, HWND _hWnd);

	// class members
	public:

	protected:

	private:
		// store copy of device
		ID3D10Device* m_pD3DDevice;

		// fx vars
		ID3D10Effect* m_pEffect;
		ID3D10EffectTechnique* m_pTechnique;
		ID3D10InputLayout* m_pLayout;

		ID3D10EffectMatrixVariable* m_pWorldMatrix;
		ID3D10EffectMatrixVariable* m_pFinalMatrix;
};

#endif // _CCLOTHCLASS_H_