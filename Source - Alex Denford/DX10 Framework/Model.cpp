// Self Include:
#include "Model.h"

// Local Includes:
#include "Structs.h"

// Library Includes:
#include <fstream>

// Implementation:
CModel::CModel() : m_pVertexBuffer(nullptr), m_pIndexBuffer(nullptr)
{

}

CModel::~CModel()
{
	ReleaseCOM(m_pVertexBuffer);
	ReleaseCOM(m_pIndexBuffer);

	if(m_pVertices) delete[] m_pVertices;
	if(m_pIndices) delete[] m_pIndices;
}

bool CModel::Initialise(ID3D10Device* _pD3DDevice, std::string _modelFilename, D3DXVECTOR3 _vecPosition, D3DXVECTOR3 _vecRotation, D3DXVECTOR3 _vecScale, D3DXVECTOR4 _vec4Colour)
{
	// Store device
	m_pD3DDevice = _pD3DDevice;
	m_vecPosition = _vecPosition;
	m_vecRotation = _vecRotation;
	m_vecScale = _vecScale;

	// load the model from the input file
	if(!LoadModel(_modelFilename, _vec4Colour))
	{
		return(false);
	}

	// buffer desc
	D3D10_BUFFER_DESC indexBufferDesc;

	indexBufferDesc.Usage = D3D10_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * m_iNumFaces * 3;
	indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA indexInitialData;
	indexInitialData.pSysMem = m_pIndices;

	if(FAILED(m_pD3DDevice->CreateBuffer(&indexBufferDesc, &indexInitialData, &m_pIndexBuffer)))
	{
		return(false);
	}

	// buffer settings (desc)
	D3D10_BUFFER_DESC vertexBufferDesc;

	vertexBufferDesc.Usage = D3D10_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * m_iNumVertices;
	vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA vertexInitialData;
	vertexInitialData.pSysMem = m_pVertices;

	if(FAILED(m_pD3DDevice->CreateBuffer(&vertexBufferDesc, &vertexInitialData, &m_pVertexBuffer)))
	{
		return(false);
	}

	BuildWorldMatrix();

	return(true);
}

bool CModel::LoadModel(std::string _modelFilename, D3DXVECTOR4 _vec4Colour)
{	
	// input file stream
	std::ifstream inputFile;

	// Open the model file
	inputFile.open(_modelFilename);
	
	// If failed to open
	if(inputFile.fail())
	{
		return(false);
	}

	char cInput;
	inputFile.get(cInput);

	while(cInput != ':')
	{
		inputFile.get(cInput);
	}

	// Read in the vertex count
	inputFile >> m_iNumVertices;
	m_iNumFaces = m_iNumVertices / 3;

	// Create vertex buffer
	m_pVertices = new Vertex[m_iNumVertices];
	if(!m_pVertices)
	{
		return(false);
	}

	// create index buffer
	m_pIndices = new DWORD[m_iNumFaces * 3];

	inputFile.get(cInput);

	while(cInput != ':')
	{
		inputFile.get(cInput);
	}

	// skip twice
	inputFile.get(cInput);
	inputFile.get(cInput);

	// Read in the vertex data.
	for(int i = 0; i < m_iNumVertices; ++i)
	{
		float fJunk;

		inputFile >> m_pVertices[i].pos.x >> m_pVertices[i].pos.y >> m_pVertices[i].pos.z;
		inputFile >> fJunk >> fJunk;
		inputFile >> fJunk >> fJunk >> fJunk;
		m_pVertices[i].colour = _vec4Colour;

		m_pIndices[i] = i;
	}

	// Close the model file.
	inputFile.close();

	return(true);
}

void CModel::ApplyRenderOptions()
{
	UINT uiStride = sizeof(Vertex);
	UINT uiOffset = 0;

	// set buffers to be used
	m_pD3DDevice->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &uiStride, &uiOffset);
	m_pD3DDevice->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

int CModel::GetFaceCount() const
{
	return(m_iNumFaces);
}

void CModel::SetPosition(D3DXVECTOR3 _vecPosition)
{
	m_vecPosition = _vecPosition;
	BuildWorldMatrix();
}

D3DXVECTOR3 CModel::GetPosition() const
{
	return(m_vecPosition);
}

void CModel::SetRotation(D3DXVECTOR3 _vecRotation)
{
	m_vecRotation = _vecRotation;
	BuildWorldMatrix();
}

D3DXVECTOR3 CModel::GetRotation() const
{
	return(m_vecRotation);
}

void CModel::SetScale(D3DXVECTOR3 _vecScale)
{
	m_vecScale = _vecScale;
	BuildWorldMatrix();
}

D3DXVECTOR3 CModel::GetScale() const
{
	return(m_vecScale);
}

D3DXMATRIX CModel::GetWorldMatrix() const
{
	return(m_matWorld);
}

void CModel::BuildWorldMatrix()
{
	D3DXMATRIX matTranslation = *D3DXMatrixTranslation(&matTranslation, m_vecPosition.x, m_vecPosition.y, m_vecPosition.z);
	D3DXMATRIX matRotation = *D3DXMatrixRotationYawPitchRoll(&matRotation, m_vecRotation.x, m_vecRotation.y, m_vecRotation.z);
	D3DXMATRIX matScaling = *D3DXMatrixScaling(&matScaling, m_vecScale.x, m_vecScale.y, m_vecScale.z);

	m_matWorld = matScaling * matRotation * matTranslation;
}

void CModel::Move(D3DXVECTOR3 _vec3Amount)
{
	m_vecPosition += _vec3Amount;
	BuildWorldMatrix();
}

void CModel::SetColour(D3DXVECTOR4 _vec4Colour)
{
	for(int i = 0; i < m_iNumVertices; ++i)
	{
		m_pVertices[i].colour = _vec4Colour;
	}

	void* vpVertices;

	// Lock vertex buffer to CPU (MAP)
	m_pVertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&vpVertices);

	// Copy the new data into the vertex buffer
	memcpy(vpVertices, (void*)m_pVertices, (sizeof(Vertex) * m_iNumVertices));

	// Unlock the vertex buffer (UNMAP)
	m_pVertexBuffer->Unmap();
}
