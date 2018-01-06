
#include "stdafx.h"
#include "texture.h"

Texture::Texture()
{
	m_texture = 0;
}

bool Texture::Initialize(ID3D11Device* device, WCHAR* filename)
{
	ID3D11Resource* texture;
	HRESULT result = DirectX::CreateDDSTextureFromFile(device, filename, &texture, &m_texture, 0, NULL);
	return !FAILED(result);
}


ID3D11ShaderResourceView* Texture::GetTexture()
{
	return m_texture;
}