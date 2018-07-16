#pragma once

#ifndef _CCLOTHCONSTRAINT_H_
#define _CCLOTHCONSTRAINT_H_

// Library Includes:

// Local Includes:
#include "Utility.h"
#include "ClothPiece.h"

// Class Declaration:
class CClothConstraint
{
	// class functions
	public:
		CClothConstraint(const float _kfRestDistance, const float _kfStiffness, CClothPiece* _pPiece1, CClothPiece* _pPiece2);
		~CClothConstraint();

		bool Process(float _fDelta);
		CClothPiece* GetPiece(int _iPiece) const;

		float GetFireDamagePercent() const;
		void SetOnFire(bool _bOnFire);
		bool IsOnFire() const;
		void CountStress();

	protected:

	private:
		bool SatisfyConstraints(float _fDelta);

	// class members
	public:

	protected:

	private:
		// Consts
		const float m_kfRestDistance;
		const float m_kfStiffness;

		// Pieces constraining
		CClothPiece* m_pPiece1;
		CClothPiece* m_pPiece2;

		// Stress
		float m_fFireDamagePercent;
		float m_fStretchPercent;

		// Fire data
		bool m_bOnFire;	
};

#endif // _CCLOTHCONSTRAINT_H_