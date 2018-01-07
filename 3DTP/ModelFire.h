#pragma once
class ModelFire
{
struct VERTEX
{
	FLOAT x, y, z;
	FLOAT u, v;
};
public:
	ModelFire();
	~ModelFire();
	void ModelFire::Initialize(ID3D11Device*, ID3D11DeviceContext*, int, int, int, int);
	void Draw(ID3D11DeviceContext*);
private:
	ID3D11Buffer *pVBuffer;
	ID3D11Buffer *pIndexBuffer;
	int sizeIndexes;
};

