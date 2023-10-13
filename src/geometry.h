#pragma once

#include <vector>
#include <iostream>

struct VertexNormal {
	float vertex[3];
	float normal[3];
};

class Geometry {

public:
	virtual VertexNormal* GetGeometry() = 0;

	virtual unsigned int GeometrySize() = 0;

};

class Block : public Geometry {
	float w;
	float h;
	float d;

	VertexNormal* mesh;

public:
	Block();
	Block(float w, float h, float d);

	~Block();

	void UpdateGeometry();

	VertexNormal* GetGeometry() override;

	unsigned int GeometrySize() override;

	void PrintSize();
};