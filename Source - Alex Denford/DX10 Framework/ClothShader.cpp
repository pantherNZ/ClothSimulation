// Self Include:
#include "ClothShader.h"

// Local Includes:

// Library Includes:
#include <fstream>

// Implementation:
CClothShader::CClothShader()
{
	m_pEffect = nullptr;
	m_pTechnique = nullptr;
	m_pLayout = nullptr;

	m_pWorldMatrix = nullptr;
	m_pFinalMatrix = nullptr;
}

CClothShader::~CClothShader()
{
	// Release the pointers to the matrices
	m_pEffect = nullptr;
	m_pTechnique = nullptr;
	m_pLayout = nullptr;

	m_pWorldMatrix = nullptr;
	m_pFinalMatrix = nullptr;

	ReleaseCOM(m_pLayout);
	ReleaseCOM(m_pEffect);
}

bool CClothShader::Initialise(ID3D10Device* _pD3DDevice, HWND _hWnd)
{
	m_pD3DDevice = _pD3DDevice;

	ID3D10Blob* pErrorMessage;

	if(FAILED(D3DX10CreateEffectFromFile(L"Data\\Shaders\\tex.fx", NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS,
										     0, m_pD3DDevice, NULL, NULL, &m_pEffect, &pErrorMessage, NULL)))
	{
		// If the shader failed to compile it should have writen something to the error message
		if(pErrorMessage)
		{
			OutputShaderErrorMessage(pErrorMessage, _hWnd);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself
		else
		{
			MessageBox(_hWnd, L"Missing Shader File", L"Error", MB_OK);
		}

		return(false);
	}

	// Get a pointer to the technique inside the shader
	m_pTechnique = m_pEffect->GetTechniqueByName("TextureTechnique");

	if(!m_pTechnique)
	{
		return(false);
	}

	// Get pointers to the data in the fx file
	m_pWorldMatrix   = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
	m_pFinalMatrix   = m_pEffect->GetVariableByName("gFinalMatrix")->AsMatrix();

	// Now setup the layout of the data that goes into the shader
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"COLOUR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0},
	};

	// Get the description of the first pass described in the shader technique.
	D3D10_PASS_DESC passDesc;
	m_pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

	// Create the input layout.
	if(FAILED(m_pD3DDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_pLayout)))
	{
		return(false);
	}

	return(true);
}

void CClothShader::Render(D3DXMATRIX _worldMatrix, D3DXMATRIX _viewMatrix, D3DXMATRIX _projectionMatrix, int _iIndexCount)
{
	m_pD3DDevice->OMSetDepthStencilState(0, 0);
	m_pD3DDevice->IASetInputLayout(m_pLayout);

	// calculate final matrix
	D3DXMATRIX finalMatrix = _worldMatrix * _viewMatrix * _projectionMatrix;

	// set matricies
	m_pWorldMatrix->SetMatrix((float*)&_worldMatrix);
	m_pFinalMatrix->SetMatrix((float*)&finalMatrix);
 
	// technique description
	D3D10_TECHNIQUE_DESC techniqueDesc;

	// Get the description structure of the technique from inside the shader so it can be used for rendering.
	m_pTechnique->GetDesc(&techniqueDesc);

	// Go through each pass in the technique
	for(unsigned int i = 0; i < techniqueDesc.Passes; ++i)
	{
		m_pTechnique->GetPassByIndex(i)->Apply(0);

		// draw
		m_pD3DDevice->DrawIndexed(_iIndexCount, 0, 0);
	}
}

void CClothShader::OutputShaderErrorMessage(ID3D10Blob* _pErrorMessage, HWND _hWnd)
{
	char* cCompileErrors;
	unsigned long ulBufferSize;
	std::ofstream fout;

	// Get a pointer to the error message text buffer
	cCompileErrors = (char*)(_pErrorMessage->GetBufferPointer());

	// Get the length of the message
	ulBufferSize = _pErrorMessage->GetBufferSize();

	// Open a file to write the error message to
	fout.open("shader-error.txt");

	// Write out the error message.
	for(unsigned int i = 0; i < ulBufferSize; i++)
	{
		fout << cCompileErrors[i];
	}

	// Close the file
	fout.close();

	// Release the error message
	ReleaseCOM(_pErrorMessage);

	// Pop a message up on the screen to notify the user to check the text file for compile errors
	MessageBox(_hWnd, L"Error compiling shader.  Check shader-error.txt for message.", L"Error", MB_OK);
}