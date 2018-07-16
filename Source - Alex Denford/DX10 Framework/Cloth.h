#pragma once

#ifndef _CCLOTH_H_
#define _CCLOTH_H_

// Library Includes:
#include <vector>

// Local Includes:
#include "Utility.h"
#include "ClothPiece.h"
#include "ClothConstraint.h"
#include "Structs.h"

// Class Declaration:
class CCloth
{
	// class functions
	public:
		CCloth();
		~CCloth();

		bool Initialise(ID3D10Device* _pD3DDevice, int _iWidth, int _iHeight, int _iRods);
		void ApplyRenderOptions();
		int GetIndexCount() const;
		D3DXMATRIX GetWorldMatrix() const;
		void Process(float _fDelta, D3DXVECTOR3 _vec3Wind, Sphere* _pSphere, Pyramid* _pPyramid, Capsule* _pCapsule, Fan* _pFan);
		void CheckIntersection(int _iButton, D3DXVECTOR3 _vec3RayOrigin, D3DXVECTOR3 _vec3RayDirection);
							   
		void SetPosition(D3DXVECTOR3 _vec3Pos);
		void SetRotation(D3DXVECTOR3 _vec3Rotation);
		void SetScale(D3DXVECTOR3 _vec3Scale);
		void Rotate(D3DXVECTOR3 _vec3Rotation);
		bool IsSelected();
		void Deselect(bool _bAllow);

	protected:

	private:
		bool InitialiseBuffers();
		void InitialiseCloth();
		void ProcessMovement(float _fDelta);
		void BuildWorldMatrix();
		void RebuildIndices();
		void RebuildVertices();

		void SphereCollision(CClothPiece* _pPiece, Sphere* _pSphere);
		void PyramidCollision(CClothPiece* _pPiece, Pyramid* _pPyramid);
		void CapsuleCollision(CClothPiece* _pPiece, Capsule* _pCapsule);
		D3DXVECTOR3 CalculateFanForce(D3DXVECTOR3 _vec3PiecePos, Fan* _pFan);

	// class members
	public:

	protected:

	private:
		// General Data
		int m_iWidth;
		int m_iHeight;
		float m_fVertexDistance;
		float m_fStiffness;
		float m_fDamping;
		int m_iNumIndices;
		int m_iNumVertices;
		bool m_bSelected;
		bool m_bSelectToggle;
		int m_iSelectedVertex;
		int m_iRods;

		// Buffers & Device
		ID3D10Device* m_pDevice;
		ID3D10Buffer* m_pVertexBuffer;
		ID3D10Buffer* m_pIndexBuffer; 

		// Transformations
		D3DXVECTOR4 m_vec4Colour;
		D3DXVECTOR3 m_vec3Position;
		D3DXVECTOR3 m_vec3Rotation;
		D3DXVECTOR3 m_vec3Scale;
		D3DXMATRIX m_matWorld;

		// Other cloth data & rendering members
		Vertex* m_pVertices;
		DWORD* m_pIndices;
		std::vector<CClothPiece*> m_vecpClothPieces;
		std::vector<CClothConstraint*> m_vecpClothConstraints;
		std::vector<CClothPiece*> m_vecpClothPins;
};

#endif // _CCLOTH_H_