// Self Include:
#include "Floor.h"

// Local Includes:

// Library Includes:
#include <fstream>

// Definitions

// Implementation:
CFloor::CFloor() : m_pD3DDevice(nullptr), m_pVertexBuffer(nullptr), m_pIndexBuffer(nullptr)
{

}

CFloor::~CFloor()
{
	ReleaseCOM(m_pVertexBuffer);
	ReleaseCOM(m_pIndexBuffer);
}

D3DXMATRIX CFloor::GetWorldMatrix() const
{
	return(m_matWorld);
}

bool CFloor::Initialise(HWND _hWnd, ID3D10Device* _pD3DDevice, int _iWidth, int _iHeight, int _iNumVerticesPerSide, D3DXVECTOR4 _vec4Colour)
{
	m_hWnd = _hWnd;
	m_iNumVerticesPerSide = _iNumVerticesPerSide;

	// store device
	m_pD3DDevice = _pD3DDevice;

	// num of verts & faces
	m_iNumVertices = (m_iNumVerticesPerSide + 1) * (m_iNumVerticesPerSide + 1);
	m_iNumFaces = m_iNumVerticesPerSide * m_iNumVerticesPerSide * 2;

	// Create the index buffer
	DWORD* indices = new DWORD[m_iNumVerticesPerSide * m_iNumVerticesPerSide * 12];

	int iCounter = 0;

	for (int y = 0; y < m_iNumVerticesPerSide; y++) 
	{
		for (int x = 0; x < m_iNumVerticesPerSide; x++)
		{
			// TRI 1
			indices[iCounter] = x + y * (m_iNumVerticesPerSide + 1);
			indices[iCounter + 1] = x + 1 + (y + 1) * (m_iNumVerticesPerSide + 1);
			indices[iCounter + 2] = x + 1 + y * (m_iNumVerticesPerSide + 1);

			// TRI 2
			indices[iCounter + 3] = indices[iCounter];
			indices[iCounter + 4] = x + (y + 1) * (m_iNumVerticesPerSide + 1);
			indices[iCounter + 5] = indices[iCounter + 1];

			iCounter += 6;
		}
	}

	// buffer desc
	D3D10_BUFFER_DESC indexBufferDesc;

	indexBufferDesc.Usage = D3D10_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * m_iNumFaces*3;
	indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA indexInitialData;
	indexInitialData.pSysMem = indices;

	if(FAILED(m_pD3DDevice->CreateBuffer(&indexBufferDesc, &indexInitialData, &m_pIndexBuffer)))
	{
		return(false);
	}

	// Vertices
	Vertex* pVertices = new Vertex[(m_iNumVerticesPerSide + 1) * (m_iNumVerticesPerSide + 1)];

	float fGapX = float(_iWidth) / float(m_iNumVerticesPerSide);
	float fGapY = float(_iHeight) / float(m_iNumVerticesPerSide);

	// Create vertex buffer
	for(int y = 0; y <= m_iNumVerticesPerSide; y++)
	{
		for(int x = 0; x <= m_iNumVerticesPerSide ; x++)
		{
			// position
			pVertices[x + y * (m_iNumVerticesPerSide + 1)].pos = D3DXVECTOR3(x * fGapX, 0.0f, y * fGapY);
			pVertices[x + y * (m_iNumVerticesPerSide + 1)].colour = _vec4Colour;
		}
	}
	 
	// buffer settings (desc)
	D3D10_BUFFER_DESC vertexBufferDesc;

	vertexBufferDesc.Usage = D3D10_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * m_iNumVertices;
	vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA vertexInitialData;
	vertexInitialData.pSysMem = pVertices;

	if(FAILED(m_pD3DDevice->CreateBuffer(&vertexBufferDesc, &vertexInitialData, &m_pVertexBuffer)))
	{
		return(false);
	}

	// delete dynamically allocated memory variables
	delete[] indices;
	delete[] pVertices;

	D3DXMATRIX matTranslation = *D3DXMatrixTranslation(&matTranslation, -_iWidth / 2.0f, -8.0f, -_iHeight / 2.0f);
	D3DXMATRIX matRotation = *D3DXMatrixRotationYawPitchRoll(&matRotation, 0.0f, 0.0f, 0.0f);
	D3DXMATRIX matScaling = *D3DXMatrixScaling(&matScaling, 1.0f, 1.0f, 1.0f);

	m_matWorld = matScaling * matRotation * matTranslation;

	return(true);
}

void CFloor::ApplyRenderOptions()
{
	UINT uiStride = sizeof(Vertex);
	UINT uiOffset = 0;

	// set buffers to be used
	m_pD3DDevice->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &uiStride, &uiOffset);
	m_pD3DDevice->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	m_pD3DDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

int CFloor::GetFaceCount() const
{
	return(m_iNumFaces);
}