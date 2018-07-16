// Self Include:
#include "ClothPiece.h"

// Local Includes:

// Library Includes:

// Implementation:
CClothPiece::CClothPiece(int _iVertex, const float _kfMass, const float _kfDamping, const bool _kbStatic, D3DXVECTOR3 _vec3Pos, D3DXVECTOR3 _vec3PrevPos) 
	: m_iVertexNumber(_iVertex), m_kfMass(_kfMass), m_kfDamping(_kfDamping), m_kbStatic(_kbStatic), m_fStress(0.0f)
{
	m_vec3Position = _vec3Pos;
	m_vec3PreviousPosition = _vec3PrevPos;
	m_vec3Acceleration = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

CClothPiece::~CClothPiece()
{

}

void CClothPiece::Process(float _fDelta)
{
	if(!m_kbStatic)
	{
		m_vec3Acceleration -= (m_vec3Position - m_vec3PreviousPosition) * (m_kfDamping / m_kfMass);
		VerletIntegration(_fDelta);
	}
}

void CClothPiece::VerletIntegration(float _fDelta)
{
	D3DXVECTOR3 vec3Temp = m_vec3Position;
	m_vec3Position += (vec3Temp - m_vec3PreviousPosition) + (m_vec3Acceleration * _fDelta * _fDelta);
	m_vec3PreviousPosition = vec3Temp;
}

void CClothPiece::ApplyForce(D3DXVECTOR3 _vec3Force)
{
	if(!m_kbStatic)
	{
		m_vec3Acceleration = (_vec3Force / m_kfMass);
	}
}

D3DXVECTOR3 CClothPiece::GetPosition() const
{
	return(m_vec3Position);
}

void CClothPiece::SetPosition(D3DXVECTOR3 _vec3NewPos)
{
	m_vec3Position = _vec3NewPos;
}

void CClothPiece::Move(D3DXVECTOR3 _vec3Amount)
{
	m_vec3Position += _vec3Amount;
}

const float CClothPiece::GetMass() const
{
	return(m_kfMass);
}

const bool CClothPiece::IsStatic() const
{
	return(m_kbStatic);
}

int CClothPiece::GetVertexNumber() const
{
	return(m_iVertexNumber);
}

void CClothPiece::CountStress(float _fVal) 
{
	m_fStress = max(_fVal, m_fStress);
}

D3DXVECTOR4 CClothPiece::GetColour()
{
	float fVal = max(1.0f - m_fStress, 0.0f);
	m_fStress = 0.0f;

	return(D3DXVECTOR4(1.0f, fVal, fVal, 1.0f));
}