// Self Include:
#include "ClothConstraint.h"

// Local Includes:

// Library Includes:

// Implementation:
CClothConstraint::CClothConstraint(const float _kfRestDistance, const float _kfStiffness, CClothPiece* _pPiece1, CClothPiece* _pPiece2)	
	:	m_kfRestDistance(_kfRestDistance), 	m_kfStiffness(_kfStiffness), m_fFireDamagePercent(0.0f), m_fStretchPercent(0.0f), m_bOnFire(false)
{
	m_pPiece1 = _pPiece1;
	m_pPiece2 = _pPiece2;
}

CClothConstraint::~CClothConstraint()
{

}

bool CClothConstraint::Process(float _fDelta)
{
	if(m_bOnFire)
	{
		m_fFireDamagePercent += (_fDelta / 10.0f);

		// If fire has reached 100%, break constraint
		if(m_fFireDamagePercent >= 1.0f)
		{
			return(true);
		}
	}

	return(SatisfyConstraints(_fDelta));
}

// Returns true if the cloth has torn
bool CClothConstraint::SatisfyConstraints(float _fDelta)
{
	// Find difference
	D3DXVECTOR3 vec3Difference = m_pPiece1->GetPosition() - m_pPiece2->GetPosition();
	float fLength = D3DXVec3Length(&vec3Difference);
	float fDifference = (m_kfRestDistance - fLength) / fLength;

	// Tearing
	if(fDifference >= m_kfRestDistance * 1.8f)
	{
		return(true);
	}

	m_fStretchPercent = abs(fDifference / (m_kfRestDistance * 1.5f));

	if(m_fStretchPercent >= 0.1f)
	{
		int iBreak = 5;
	}

	float fStrength1 = 1.0f / m_pPiece1->GetMass();
	float fStrength2 = 1.0f / m_pPiece2->GetMass();
		
	// Move the pieces towards / away each other
	if(!m_pPiece1->IsStatic())
	{
		m_pPiece1->Move(vec3Difference * (fStrength1 / (fStrength1 + fStrength2)) * m_kfStiffness * fDifference); 
	}

	if(!m_pPiece2->IsStatic())
	{
		m_pPiece2->Move(-vec3Difference * (fStrength2 / (fStrength1 + fStrength2)) * m_kfStiffness * fDifference); 
	}

	return(false);
}

CClothPiece* CClothConstraint::GetPiece(int _iPiece) const
{
	if(_iPiece == 1)
	{
		return(m_pPiece1);
	}
	else
	{
		return(m_pPiece2);
	}
}

void CClothConstraint::CountStress()
{
	float fStress = max(m_fStretchPercent, m_fFireDamagePercent);
	m_pPiece1->CountStress(fStress);
	m_pPiece2->CountStress(fStress);
}

void CClothConstraint::SetOnFire(bool _bOnFire)
{
	m_bOnFire = _bOnFire;
}

bool CClothConstraint::IsOnFire() const
{
	return(m_bOnFire);
}

float CClothConstraint::GetFireDamagePercent() const
{
	return(m_fFireDamagePercent);
}