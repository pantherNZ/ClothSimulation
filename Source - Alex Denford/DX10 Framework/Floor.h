#ifndef _CFLOOR_H_
#define _CFLOOR_H_

// Library Includes:

// Local Includes:
#include "Structs.h"

// Class Declaration:
class CFloor
{
	// class functions
	public:
		CFloor();
		~CFloor();
 
		bool Initialise(HWND _hWnd, ID3D10Device* _pD3DDevice, int _iWidth, int _iHeight, int _iNumVerticesPerSide, D3DXVECTOR4 _vec4Colour);
		void ApplyRenderOptions();
		int GetFaceCount() const;
		D3DXMATRIX GetWorldMatrix() const;

	protected:

	private:

	// class members
	public:

	protected:

	private:
		HWND m_hWnd;
		int m_iNumVerticesPerSide;
		int m_iNumVertices;
		int m_iNumFaces;

		ID3D10Device* m_pD3DDevice;
		ID3D10Buffer* m_pVertexBuffer; // Vertex buffer
		ID3D10Buffer* m_pIndexBuffer; // Index  buffer

		D3DXMATRIX m_matWorld;
};

#endif // _CTERRAIN_H_