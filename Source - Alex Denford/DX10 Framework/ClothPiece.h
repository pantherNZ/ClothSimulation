#pragma once

#ifndef _CCLOTHPIECE_H_
#define _CCLOTHPIECE_H_

// Library Includes:

// Local Includes:
#include "Utility.h"

class CClothPiece;

// Class Declaration:
class CClothPiece
{
	// class functions
	public:
		CClothPiece(int _iVertex, const float _kfMass, const float _kfDamping, const bool _kbStatic, D3DXVECTOR3 _vec3Pos, D3DXVECTOR3 _vec3PrevPos);
		~CClothPiece();
		void Process(float _fDelta);

		D3DXVECTOR3 GetPosition() const;	
		void SetPosition(D3DXVECTOR3 _vec3NewPos);
		void Move(D3DXVECTOR3 _vec3Amount);
		void ApplyForce(D3DXVECTOR3 _vec3Force);
		const float GetMass() const;
		const bool IsStatic() const;
		int GetVertexNumber() const;
		void CountStress(float _fVal);
		D3DXVECTOR4 GetColour();

	protected:

	private:
		void VerletIntegration(float _fDelta);

	// class members
	public:

	protected:

	private:
		int m_iVertexNumber;
		const float m_kfMass;
		const float m_kfDamping;
		const bool m_kbStatic;
		
		D3DXVECTOR3 m_vec3Position;
		D3DXVECTOR3 m_vec3PreviousPosition;
		D3DXVECTOR3 m_vec3Acceleration;

		float m_fStress;
};

#endif // _CCLOTHPIECE_H_