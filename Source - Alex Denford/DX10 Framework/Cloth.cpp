// Self Include:
#include "Cloth.h"

// Local Includes:
#include "System.h"
#include "Model.h"

// Library Includes:

// Static variables

// Implementation:
CCloth::CCloth() : m_pDevice(nullptr), m_pIndexBuffer(nullptr), m_pVertexBuffer(nullptr), m_fStiffness(0.0f), m_bSelected(false),
				  m_iSelectedVertex(0), m_fDamping(0.0f), m_pVertices(nullptr), m_pIndices(nullptr), m_bSelectToggle(true)
{
	m_vec4Colour   =  D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	m_vec3Position =  D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vec3Rotation =  D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vec3Scale    =  D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	m_matWorld     = *D3DXMatrixIdentity(&m_matWorld);
}

CCloth::~CCloth()
{
	ReleaseCOM(m_pVertexBuffer);
	ReleaseCOM(m_pIndexBuffer);

	delete[] m_pVertices;
	delete[] m_pIndices;

	for(unsigned int i = 0; i < m_vecpClothPieces.size(); ++i)
	{
		DeleteOBJ(m_vecpClothPieces[i]);
	}

	for(unsigned int i = 0; i < m_vecpClothConstraints.size(); ++i)
	{
		DeleteOBJ(m_vecpClothConstraints[i]);
	}

	m_vecpClothPins.clear();
}

bool CCloth::Initialise(ID3D10Device* _pD3DDevice, int _iWidth, int _iHeight, int _iRods)
{
	m_pDevice = _pD3DDevice;

	// Data
	m_fVertexDistance = 0.5f;
	m_fStiffness = 0.8f;
	m_fDamping = 0.8f;
	m_iWidth = _iWidth;
	m_iHeight = _iHeight;
	m_iRods = _iRods;

	// Setup cloth pieces (physics simulation)
	InitialiseCloth();

	// Buffer sizes (vertices / indices)
	m_iNumVertices = m_iWidth * m_iHeight;
	m_iNumIndices = m_vecpClothConstraints.size() * 2;

	// Create index and vertex buffers for DX10 rendering
	if(!InitialiseBuffers())
	{
		return(false);
	}

	return(true);
}

bool CCloth::InitialiseBuffers()
{
	// Create vertex buffer description
	D3D10_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D10_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * m_iNumVertices;
	vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;

	// Create vertex buffer
	if(FAILED(m_pDevice->CreateBuffer(&vertexBufferDesc, NULL, &m_pVertexBuffer)))
	{
		MessageBox(NULL, L"Initialising Vertex Buffer Failed", L"Error", MB_OK);	
		PostQuitMessage(0);
		return(false);
	}

	// Build vertices (call this function - same function used when a vertex is removed (tearing), saves duplicate code)
	RebuildVertices();

	// Create Index buffer description
	D3D10_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D10_USAGE_DYNAMIC;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * m_iNumIndices;
	indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	indexBufferDesc.MiscFlags = 0;

	// Create Index buffer 
	if(FAILED(m_pDevice->CreateBuffer(&indexBufferDesc, NULL, &m_pIndexBuffer)))
	{
		MessageBox(NULL, L"Initialising Index Buffer Failed", L"Error", MB_OK);	
		PostQuitMessage(0);
		return(false);
	}

	// Build indices (call this function - same function used when tearing occurs, saves duplicate code)
	RebuildIndices();

	return(true);
}

void CCloth::InitialiseCloth()
{
	D3DXVECTOR3 vec3BasePos(-m_iWidth * m_fVertexDistance / 2.0f, -m_iHeight * m_fVertexDistance / 2.0f, 0.0f);
	bool bCreateRods = m_iRods != 0;
	int iGap = m_iRods == 1 ? m_iWidth : int(m_iWidth / (m_iRods - 1));
	int iCount = 0;

	for(int x = 0; x < m_iWidth; ++x)
	{
		for(int y = 0; y < m_iHeight; ++y)
		{
			bool bStatic = false;

			if(bCreateRods && y == m_iHeight - 1 && ((x % iGap == 0 && x <= max(m_iWidth - iGap / 3, 3)) || (x == m_iWidth - 1 && iCount == m_iRods - 1)))
			{
				++iCount;
				bStatic = true; 
			}

			D3DXVECTOR3 vec3Pos = vec3BasePos + D3DXVECTOR3(float(x * m_fVertexDistance), float(y * m_fVertexDistance), 0.0f);
			CClothPiece* pPiece = new CClothPiece(x * m_iHeight + y, 10.0f, m_fDamping, bStatic, vec3Pos, vec3Pos);
			m_vecpClothPieces.push_back(pPiece);

			if(bStatic)
			{
				m_vecpClothPins.push_back(pPiece);
			}
		}
	}

	for(int x = 0; x < m_iWidth; ++x)
	{
		for(int y = 0; y < m_iHeight; ++y)
		{
			// Horizontal constraints
			if(x < m_iWidth - 1)
			{
				CClothConstraint* pConstraint = new CClothConstraint(m_fVertexDistance, m_fStiffness, m_vecpClothPieces[x * m_iHeight + y], m_vecpClothPieces[(x + 1) * m_iHeight + y]);
				m_vecpClothConstraints.push_back(pConstraint);
			}

			// Vertical constraints
			if(y < m_iHeight - 1)
			{
				CClothConstraint* pConstraint = new CClothConstraint(m_fVertexDistance, m_fStiffness, m_vecpClothPieces[x * m_iHeight + y], m_vecpClothPieces[x * m_iHeight + (y + 1)]);
				m_vecpClothConstraints.push_back(pConstraint);
			}

			// Diagonal forward constraints
			if(x < m_iWidth - 1 && y < m_iHeight - 1)
			{
				float fDiagonalDistance = sqrt(2 * pow(m_fVertexDistance, 2));
				CClothConstraint* pConstraint = new CClothConstraint(fDiagonalDistance, m_fStiffness, m_vecpClothPieces[x * m_iHeight + y], m_vecpClothPieces[(x + 1) * m_iHeight + (y + 1)]);
				m_vecpClothConstraints.push_back(pConstraint);
			}

			// Diagonal backward constraints
			if(x > 0 && y < m_iHeight - 1)
			{
				float fDiagonalDistance = sqrt(2 * pow(m_fVertexDistance, 2));
				CClothConstraint* pConstraint = new CClothConstraint(fDiagonalDistance, m_fStiffness, m_vecpClothPieces[x * m_iHeight + y], m_vecpClothPieces[(x - 1)* m_iHeight + (y + 1)]);
				m_vecpClothConstraints.push_back(pConstraint);
			}

			// Jump constraints horizontal
			/*if(x < m_iWidth - 2)
			{
				CClothConstraint* pConstraint = new CClothConstraint(m_fVertexDistance * 2.0f, m_fStiffness, m_vecpClothPieces[x * m_iHeight + y], m_vecpClothPieces[(x + 2)* m_iHeight + y]);
				m_vecpClothConstraints.push_back(pConstraint);
			}

			// Jump constraints vertical
			if(y < m_iHeight - 2)
			{
				CClothConstraint* pConstraint = new CClothConstraint(m_fVertexDistance * 2.0f, m_fStiffness, m_vecpClothPieces[x * m_iHeight + y], m_vecpClothPieces[x* m_iHeight + (y + 2)]);
				m_vecpClothConstraints.push_back(pConstraint);
			}*/
		}
	}
}

void CCloth::RebuildIndices()
{
	// If indices already exist, delete them first
	if(m_pIndices != nullptr)
	{
		delete[] m_pIndices;
		m_pIndices = nullptr;
	}

	int iCounter = 0;
	m_iNumIndices = m_vecpClothConstraints.size() * 2;
	m_pIndices = new DWORD[m_iNumIndices];

	// Create indices using the vector of constraints
	for(unsigned int i = 0; i < m_vecpClothConstraints.size(); ++i)
	{
		m_pIndices[iCounter] = m_vecpClothConstraints[i]->GetPiece(1)->GetVertexNumber();
		m_pIndices[iCounter + 1] = m_vecpClothConstraints[i]->GetPiece(2)->GetVertexNumber();
		iCounter += 2;
	}

	void* vpIndices;

	// Lock index buffer to CPU (MAP)
	m_pIndexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&vpIndices);

	// Copy the new data into the vertex buffer
	memcpy(vpIndices, (void*)m_pIndices, (sizeof(DWORD) * m_iNumIndices));

	// Unlock the index buffer (UNMAP)
	m_pIndexBuffer->Unmap();
}

void CCloth::RebuildVertices()
{
	// If indices already exist, delete them first
	if(m_pVertices != nullptr)
	{
		delete[] m_pVertices;
		m_pVertices = nullptr;
	}

	// Vertices (just use the cloth piece positions and set the colours to the default
	m_pVertices = new Vertex[m_vecpClothPieces.size()];

	for(unsigned int i = 0; i < m_vecpClothPieces.size(); ++i)
	{
		m_pVertices[i].colour = m_vec4Colour;
		m_pVertices[i].pos = m_vecpClothPieces[i]->GetPosition();
	}
}

void CCloth::Process(float _fDelta, D3DXVECTOR3 _vec3Wind, Sphere* _pSphere, Pyramid* _pPyramid, Capsule* _pCapsule, Fan* _pFan)
{
	bool bRebuildIndices = false;

	// Process constraints
	for(int i = m_vecpClothConstraints.size() - 1; i >= 0; --i)
	{
		// Satisfy the constraints, if it returns true it means it has torn
		if(m_vecpClothConstraints[i]->Process(_fDelta))
		{
			// Delete constraint
			DeleteOBJ(m_vecpClothConstraints[i]);
			m_vecpClothConstraints.erase(m_vecpClothConstraints.begin() + i);
			bRebuildIndices = true;
			continue;
		}

		// Count stress from constraints
		m_vecpClothConstraints[i]->CountStress();

		// Spread fire
		if(m_vecpClothConstraints[i]->IsOnFire())
		{
			float fFirePercent = m_vecpClothConstraints[i]->GetFireDamagePercent();

			int iArray[] = {-1, 1, -2, 2, -3, 3, -4, 4, -4 * m_iHeight, 4 * m_iHeight, 
							-4 * m_iHeight + 1, 4 * m_iHeight + 1, 
							-4 * m_iHeight - 1, 4 * m_iHeight - 1,
							-4 * m_iHeight + 2, 4 * m_iHeight + 2,
							-4 * m_iHeight - 2, 4 * m_iHeight - 2};

			float fRand = RandomFloat();

			if(fRand <= fFirePercent && fRand * _fDelta <= 0.01f)
			{
				int iIndex = i + iArray[rand() % 18];

				if(iIndex >= 0 && iIndex < int(m_vecpClothConstraints.size()))
				{
					m_vecpClothConstraints[iIndex]->SetOnFire(true);
				}
			}
		}
	}

	if(bRebuildIndices)
	{
		// Rebuild indices (to remove the old constraint / line)
		RebuildIndices();
	}

	// Update vertex positions / process vertices
	for(unsigned int i = 0; i < m_vecpClothPieces.size(); ++i)
	{
		D3DXVECTOR3 vec3Pos = m_vecpClothPieces[i]->GetPosition();
		D3DXVECTOR3 vec3FanForce = CalculateFanForce(vec3Pos, _pFan);

		bool bOnFloor = (vec3Pos.y <= -8.0f);

		// Floor friction
		if(bOnFloor)
		{
			_vec3Wind *= 0.9f;
		}

		// Apply force of wind + gravity + fan force
		m_vecpClothPieces[i]->ApplyForce(vec3FanForce + _vec3Wind + D3DXVECTOR3(0.0f, -50.0f, 0.0f)); 

		// Process the piece of the cloth 
		m_vecpClothPieces[i]->Process(_fDelta);
		
		// Floor collision
		if(bOnFloor)
		{
			vec3Pos.y = -8.0f;
			m_vecpClothPieces[i]->SetPosition(vec3Pos);
		}

		// Sphere collision
		SphereCollision(m_vecpClothPieces[i], _pSphere);

		// Pyramid collision
		PyramidCollision(m_vecpClothPieces[i], _pPyramid);

		// Capsule collision
		CapsuleCollision(m_vecpClothPieces[i], _pCapsule);

		// Update vertex pos with actual cloth position
		m_pVertices[i].pos = vec3Pos;

		// Stress colour
		m_pVertices[i].colour = m_vecpClothPieces[i]->GetColour();
	}

	void* vpVertices;

	// Lock vertex buffer to CPU (MAP)
	m_pVertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&vpVertices);

	// Copy the new data into the vertex buffer
	memcpy(vpVertices, (void*)m_pVertices, (sizeof(Vertex) * m_iNumVertices));

	// Unlock the vertex buffer (UNMAP)
	m_pVertexBuffer->Unmap();

	// Movement with A & D
	ProcessMovement(_fDelta);
}

D3DXVECTOR3 CCloth::CalculateFanForce(D3DXVECTOR3 _vec3PiecePos, Fan* _pFan)
{
	D3DXVECTOR3 vec3FanForce = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vec3FanDir = (_vec3PiecePos - _pFan->objModel.GetPosition());
	float fDistance = D3DXVec3LengthSq(&vec3FanDir);

	if(fDistance <= (_pFan->fDistance * _pFan->fDistance))
	{
		float fYaw = _pFan->objModel.GetRotation().x;
		D3DXVECTOR3 vec3FanForward = D3DXVECTOR3(sin(fYaw), 0.0f, cos(fYaw));
		float fDot = D3DXVec3Dot(&vec3FanForward, &vec3FanDir);
		float fAng = float( D3DXToDegree( acos( fDot / ( D3DXVec3Length(&vec3FanForward) * D3DXVec3Length(&vec3FanDir) ) ) ) );
	
		if(fAng >= -_pFan->fAngle / 2.0f && fAng <= _pFan->fAngle / 2.0f)
		{
			*D3DXVec3Normalize(&vec3FanForce, &vec3FanDir);
			vec3FanForce *= (100.0f / fDistance);
			vec3FanForce *= _pFan->fStrength;
		}
	}

	return(vec3FanForce);
}

void CCloth::SphereCollision(CClothPiece* _pPiece, Sphere* _pSphere)
{
	D3DXVECTOR3 vec3Dir = _pSphere->objModel.GetPosition() - _pPiece->GetPosition();
	float fLength = D3DXVec3LengthSq(&vec3Dir);
	float fRadii = (m_fVertexDistance - 0.4f) / 2.0f + _pSphere->fRadius;
	float fDist = fLength - (fRadii * fRadii);

	if(fDist <= 0.0f)
	{
		_pPiece->Move(vec3Dir * (fDist / fLength));
	}
}

void CCloth::PyramidCollision(CClothPiece* _pPiece, Pyramid* _pPyramid)
{
	D3DXVECTOR3 vec3Point = _pPiece->GetPosition();
	D3DXVECTOR3 vec3PyramidPos = _pPyramid->objModel.GetPosition();

	// Primitive distance check (saves FPS)
	if(D3DXVec3LengthSq(&(vec3Point - vec3PyramidPos)) <= _pPyramid->fHeight * _pPyramid->fHeight)
	{
		float fBestDist = 9999999.0f;
		int iIndex = -1;

		for(int i = 0; i < 5; ++i)
		{
			D3DXVECTOR3 vec3PlanePoint = _pPyramid->tSide[i].vec3Pos;
			D3DXVECTOR3 vec3PlaneNorm = _pPyramid->tSide[i].vec3Normal;
			
			float fDistance = D3DXVec3Dot(&vec3PlaneNorm, &(vec3Point - vec3PlanePoint));

			// If result distance is less than 0, the point is under the plane
			if(fDistance <= 0.0f)
			{				
				if(fDistance < fBestDist)
				{
					fBestDist = fDistance;
					iIndex = i;
				}
			}
			else
			{
				// If we have found a point above one of the planes, we know there is no collision
				return;
			}
		}

		// Move the cloth piece in direction of lowest distance
		_pPiece->Move(_pPyramid->tSide[iIndex].vec3Normal * fBestDist * 0.1f);
	}
}

void CCloth::CapsuleCollision(CClothPiece* _pPiece, Capsule* _pCapsule)
{
	D3DXVECTOR3 vec3Point = _pPiece->GetPosition();
	D3DXVECTOR3 vec3CapPos = _pCapsule->objModel.GetPosition();

	// Primitive distance check (saves FPS)
	if(D3DXVec3LengthSq(&(vec3Point - vec3CapPos)) <= _pCapsule->fHeight * _pCapsule->fHeight)
	{
		// Line segment (origin + ft * direction)
		D3DXVECTOR3 vec3RO = vec3CapPos + D3DXVECTOR3(0.0f, _pCapsule->fRadius, 0.0f);
		D3DXVECTOR3 vec3RD = D3DXVECTOR3(0.0f, _pCapsule->fHeight - _pCapsule->fRadius * 2, 0.0f);

		// Points on capsule
		D3DXVECTOR3 vec3Bot = vec3RO;
		D3DXVECTOR3 vec3Top = vec3RO + vec3RD;

		D3DXVECTOR3 vec3ToPoint = vec3Point - vec3RO;

		float f1 = D3DXVec3Dot(&vec3ToPoint, &vec3RD);
		float f2 = D3DXVec3Dot(&vec3RD, &vec3RD);
		float fT = f1 / f2;

		// Closest point on ray cast to the vertex
		D3DXVECTOR3 vec3PointLine = vec3RO + fT * vec3RD;
		float fRadii = (m_fVertexDistance - 0.4f) / 2.0f + _pCapsule->fRadius;

		float fDistToBot = D3DXVec3Length(&(vec3Point - vec3Bot));
		float fDistToTop = D3DXVec3Length(&(vec3Point - vec3Top));

		if(fT >= 0.0f && fT <= 1.0f)
		{
			// Move the vertex along the direction vector from the vertex to the point
			vec3ToPoint = vec3PointLine - vec3Point;

			float fLength = D3DXVec3Length(&vec3ToPoint);
			float fDist = fLength - fRadii;

			if(fDist <= 0.0f)
			{
				_pPiece->Move(vec3ToPoint * (fDist / fLength));
			}
		}
		else if(fDistToBot <= fRadii)
		{
			vec3ToPoint = vec3Bot - vec3Point;
			float fDist = fDistToBot - fRadii;
			_pPiece->Move(vec3ToPoint * (fDist / fDistToBot));
		}
		else if(fDistToTop <= fRadii)
		{
			vec3ToPoint = vec3Top - vec3Point;
			float fDist = fDistToTop - fRadii;
			_pPiece->Move(vec3ToPoint * (fDist / fDistToTop));
		}
	}
}

void CCloth::ProcessMovement(float _fDelta)
{
	// Left arrow
	if(GetAsyncKeyState(VK_OEM_MINUS) & 0x8000)
	{
		if(D3DXVec3LengthSq(&(m_vecpClothPins.front()->GetPosition() - m_vecpClothPins[m_vecpClothPins.size() - 1]->GetPosition())) >= 36.0f)
		{
			for(unsigned int i = 1; i < m_vecpClothPins.size(); ++i)
			{
				float fRatio = float(m_vecpClothPins[i]->GetVertexNumber()) / float(m_iWidth * m_iHeight);
				m_vecpClothPins[i]->Move(D3DXVECTOR3(-6.0f * _fDelta * fRatio, 0.0f, 0.0f));
			}
		}
	}

	// Right arrow
	if(GetAsyncKeyState(VK_OEM_PLUS) & 0x8000)
	{
		for(unsigned int i = 1; i < m_vecpClothPins.size(); ++i)
		{
			float fRatio = float(m_vecpClothPins[i]->GetVertexNumber()) / float(m_iWidth * m_iHeight);
			m_vecpClothPins[i]->Move(D3DXVECTOR3(6.0f * _fDelta * fRatio, 0.0f, 0.0f));
		}
	}
}

void CCloth::CheckIntersection(int _iButton, D3DXVECTOR3 _vec3RO, D3DXVECTOR3 _vec3RD)
{
	D3DXVECTOR3 vec3T1 = _vec3RO + _vec3RD;

	// Put the ray into the cloth's local space
	D3DXMATRIX matWorldInv = *D3DXMatrixInverse(&matWorldInv, NULL, &m_matWorld);
	D3DXVec3TransformCoord(&_vec3RO, &_vec3RO, &matWorldInv);
	D3DXVec3TransformNormal(&_vec3RD, &_vec3RD, &matWorldInv);

	if(!m_bSelected)
	{
		if(m_bSelectToggle)
		{
			m_bSelectToggle = false;

			D3DXVec3Normalize(&_vec3RD, &_vec3RD);
			float fSphereRadSq = m_fVertexDistance * m_fVertexDistance * 0.5f;
			float fClosestDist = 9999999.0f;
			int iClosestIndex = -1;

			// Ray - Sphere collision with each cloth piece
			for(int x = 0; x < m_iWidth; ++x)
			{
				for(int y = 0; y < m_iHeight; ++y)
				{
					int iIndex = x * m_iHeight + y;

					D3DXVECTOR3 vec3L = m_vecpClothPieces[iIndex]->GetPosition() - _vec3RO;
					float fTCA = D3DXVec3Dot(&vec3L, &_vec3RD);
					if(fTCA < 0.0f) continue;
					float fDistSq = D3DXVec3Dot(&vec3L, &vec3L) - (fTCA * fTCA);
					if(fDistSq > fSphereRadSq) continue;

					float fDist = D3DXVec3Length(&vec3L);
					if(fDist < fClosestDist)
					{
						iClosestIndex = iIndex;
						fClosestDist = fDist;
					}
				}
			}

			// If an index has been found (clicked on)
			if(iClosestIndex != -1)
			{
				// If left click (select piece)
				if(_iButton == 0)
				{
					m_bSelected = true;
					m_iSelectedVertex = iClosestIndex;
				}
				// Otherwise, we need to find which constraints are attached to our piece
				else if(_iButton == 1 || _iButton == 2)
				{
					// Loop through the constraints and remove any that use this piece
					for(int i = m_vecpClothConstraints.size() - 1; i >= 0; --i)
					{
						if(m_vecpClothPieces[iClosestIndex] == m_vecpClothConstraints[i]->GetPiece(1) || m_vecpClothPieces[iClosestIndex] == m_vecpClothConstraints[i]->GetPiece(2))
						{
							// If right click (Remove piece)
							if(_iButton == 1)
							{
								DeleteOBJ(m_vecpClothConstraints[i]);
								m_vecpClothConstraints.erase(m_vecpClothConstraints.begin() + i);
							}
							// Else if middle click (set piece on fire)
							else
							{
								m_vecpClothConstraints[i]->SetOnFire(true);
							}
						}
					}

					// Rebuild vertices & indices
					RebuildIndices();
				}
			}
		}
	}
	else
	{
		// If there is a vertex already selected
		// -> Create a perpendicular vector from vertex pos to mouse ray cast
		// -> Move vertex along the vector
		if(!m_vecpClothPieces[m_iSelectedVertex]->IsStatic())
		{
			D3DXVECTOR3 vec3ToPoint = m_vecpClothPieces[m_iSelectedVertex]->GetPosition() - _vec3RO;

			float f1 = D3DXVec3Dot(&vec3ToPoint, &_vec3RD);
			float f2 = D3DXVec3Dot(&_vec3RD, &_vec3RD);
			float fT = f1 / f2;

			// Closest point on ray cast to the vertex
			D3DXVECTOR3 vec3PointLine = _vec3RO + fT * _vec3RD;

			// Move the vertex along the direction vector from the vertex to the point
			vec3ToPoint = vec3PointLine - m_vecpClothPieces[m_iSelectedVertex]->GetPosition();
			m_vecpClothPieces[m_iSelectedVertex]->Move(vec3ToPoint * 1.0f);
		}
	}
}

bool CCloth::IsSelected()
{
	return(m_bSelected);
}

void CCloth::Deselect(bool _bAllow)
{
	m_bSelected = false;
	m_iSelectedVertex = -1;
	m_bSelectToggle = _bAllow;
}

void CCloth::SetPosition(D3DXVECTOR3 _vec3Pos)
{
	m_vec3Position = _vec3Pos;
	BuildWorldMatrix();
}

void CCloth::SetRotation(D3DXVECTOR3 _vec3Rotation)
{
	m_vec3Rotation = _vec3Rotation;
	BuildWorldMatrix();
}

void CCloth::SetScale(D3DXVECTOR3 _vec3Scale)
{
	m_vec3Scale = _vec3Scale;
	BuildWorldMatrix();
}

void CCloth::Rotate(D3DXVECTOR3 _vec3Rotation)
{
	m_vec3Rotation += _vec3Rotation;
	BuildWorldMatrix();
}

void CCloth::BuildWorldMatrix()
{
	D3DXMATRIX matTranslation = *D3DXMatrixTranslation(&matTranslation, m_vec3Position.x, m_vec3Position.y, m_vec3Position.z);
	D3DXMATRIX matRotation = *D3DXMatrixRotationYawPitchRoll(&matRotation, m_vec3Rotation.x, m_vec3Rotation.y, m_vec3Rotation.z);
	D3DXMATRIX matScaling = *D3DXMatrixScaling(&matScaling, m_vec3Scale.x, m_vec3Scale.y, m_vec3Scale.z);

	m_matWorld = matScaling * matRotation * matTranslation;
}

D3DXMATRIX CCloth::GetWorldMatrix() const
{
	return(m_matWorld);
}

void CCloth::ApplyRenderOptions()
{
	UINT uiStride = sizeof(Vertex);
	UINT uiOffset = 0;

	// set buffers to be used
	m_pDevice->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &uiStride, &uiOffset);
	m_pDevice->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	m_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
}

int CCloth::GetIndexCount() const
{
	return(m_iNumIndices);
}