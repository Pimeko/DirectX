#include "stdafx.h"
#include "ModelFire.h"


ModelFire::ModelFire()
{
}


ModelFire::~ModelFire()
{
}

void ModelFire::Initialize(ID3D11Device* g_pDevice, ID3D11DeviceContext* g_pImmediateContext, int offsetX, int offsetY, int offsetZ, int width)
{
	auto height = width, size = width * height;

	VERTEX* vertices = new VERTEX[size];

	for (auto i = 0; i < size; i++)
	{
		float x = i / width;
		float y = i % width;
		float u = float(x) / float(width);
		float v = 1.0f - (float(y) / float(height));

		vertices[i] = { x + offsetX, (float)offsetZ, y + offsetY, u, v };
	}

	// INDEXES
	int nbSquares = (width - 1) * (height - 1);
	sizeIndexes = nbSquares * (3 * 2);

	unsigned int* indexes = new unsigned int[sizeIndexes];

	int j = 0;
	for (auto i = 0; i < nbSquares; i++)
	{
		auto left = i + (i / (width - 1));
		indexes[j++] = left;
		indexes[j++] = left + 1;
		indexes[j++] = left + width;

		indexes[j++] = left + 1;
		indexes[j++] = left + width;
		indexes[j++] = left + width + 1;
	}

	D3D11_BUFFER_DESC bd_index;

	bd_index.Usage = D3D11_USAGE_DEFAULT;
	bd_index.ByteWidth = sizeof(unsigned int) * sizeIndexes;
	bd_index.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd_index.CPUAccessFlags = 0;
	bd_index.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = indexes;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	g_pDevice->CreateBuffer(&bd_index, &initData, &pIndexBuffer);
	g_pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX) * size;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	g_pDevice->CreateBuffer(&bd, NULL, &pVBuffer);

	D3D11_MAPPED_SUBRESOURCE ms;
	g_pImmediateContext->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, vertices, sizeof(VERTEX) * size);
	g_pImmediateContext->Unmap(pVBuffer, NULL);
}

void ModelFire::Draw(ID3D11DeviceContext* g_pImmediateContext)
{
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	g_pImmediateContext->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
	g_pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	g_pImmediateContext->DrawIndexed(sizeIndexes, 0, 0);
}
