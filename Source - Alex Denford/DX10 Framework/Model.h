#pragma once

#ifndef _CMODEL_H_
#define _CMODEL_H_

// Library Includes:

// Local Includes:
#include "Utility.h"

struct Vertex;

// Class Declaration:
class CModel
{
	// class functions
	public:
		CModel();
		~CModel();
		bool Initialise(ID3D10Device* _pD3DDevice, std::string _modelFilename, D3DXVECTOR3 _vecPosition, D3DXVECTOR3 _vecRotation, D3DXVECTOR3 _vecScale, D3DXVECTOR4 _vec4Colour);
		void ApplyRenderOptions();
		int GetFaceCount() const;

		void SetPosition(D3DXVECTOR3 _vecPosition);
		D3DXVECTOR3 GetPosition() const;

		void SetRotation(D3DXVECTOR3 _vecRotation);
		D3DXVECTOR3 GetRotation() const;

		void SetScale(D3DXVECTOR3 _vecScale);
		D3DXVECTOR3 GetScale() const;

		void Move(D3DXVECTOR3 _vec3Amount);
		void SetColour(D3DXVECTOR4 _vec4Colour);

		D3DXMATRIX GetWorldMatrix() const;

	protected:

	private:
		bool LoadModel(std::string _modelFilename, D3DXVECTOR4 _vec4Colour);
		void BuildWorldMatrix();

	// class members
	public:

	protected:

	private:
		// store copy of device
		ID3D10Device* m_pD3DDevice;

		ID3D10Buffer* m_pVertexBuffer; // Vertex buffer
		ID3D10Buffer* m_pIndexBuffer; // Index buffer

		int m_iNumVertices;
		int m_iNumFaces;

		// vertices and indices
		Vertex* m_pVertices;
		DWORD* m_pIndices;

		// position & rotation
		D3DXVECTOR3 m_vecPosition;
		D3DXVECTOR3 m_vecRotation;
		D3DXVECTOR3 m_vecScale;

		D3DXMATRIX m_matWorld;
};

#endif // _CMODEL_H_