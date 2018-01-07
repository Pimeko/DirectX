#pragma once
class Model
{
struct VERTEX
{
	FLOAT x, y, z;
	FLOAT u, v;
};
public:
	Model();
	~Model();
	void Model::Initialize(ID3D11Device*, ID3D11DeviceContext*, int, int, int, int, bool);
	void Draw(ID3D11DeviceContext*);
private:
	ID3D11Buffer *pVBuffer;
	ID3D11Buffer *pIndexBuffer;
	int sizeIndexes;
};

