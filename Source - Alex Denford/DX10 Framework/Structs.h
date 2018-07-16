#pragma once

#ifndef STRUCTS_H
#define STRUCTS_H

// Library includes

// Local includes
#include "Utility.h"
#include "Model.h"

// Structs
struct Vertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR4 colour;
};

struct Plane
{
	D3DXVECTOR3 vec3Pos;
	D3DXVECTOR3 vec3Normal;
};

struct Sphere
{
	CModel objModel;
	float fRadius;
};

struct Pyramid
{
	CModel objModel;
	float fWidth;
	float fHeight;
	Plane tSide[5];
};

struct Capsule
{
	CModel objModel;
	float fRadius;
	float fHeight;
};

struct Fan
{
	CModel objModel;
	float fStrength;
	float fAngle;
	float fDistance;
};

#endif // STRUCTS_H