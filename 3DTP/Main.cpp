#include "stdafx.h"
#include "3DTP.h"
#include "InputManager.h"
#include "D3Dcompiler.h"
#include "Camera.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "DirectXTK\DDSTextureLoader.h"

// Global Variables:
HINSTANCE									hInst;			// current instance
HWND											hWnd;				// windows handle used in DirectX initialization
IAEngine::InputManager*		g_pInputManager = NULL;
IDXGISwapChain*						g_pSwapChain = NULL;
ID3D11Device*							g_pDevice = NULL;
ID3D11DeviceContext*			g_pImmediateContext = NULL;
ID3D11RenderTargetView*		g_pRenderTargetView = NULL;

ID3D11Texture2D*					g_pDepthStencil = NULL;
ID3D11DepthStencilView*		g_pDepthStencilView = NULL;

// Forward declarations
bool				CreateWindows(HINSTANCE, int, HWND& hWnd);
bool				CreateDevice();
bool				CreateDefaultRT();
bool				CompileShader(LPCWSTR pFileName, bool bPixel, LPCSTR pEntrypoint, ID3DBlob** ppCompiledShader);//utiliser un L devant une chaine de caractère pour avoir un wchar* comme L"MonEffet.fx"

bool LoadRAW(const std::string& map, float** m_height, unsigned short *m_sizeX, unsigned short *m_sizeY, const float *m_maxZ);

/*
Exemple of possible triangle coodinates in 3D
P0
0.0f, 1.0f, 0.0f,
P1
5.0f, 1.0f, 0.0f,
P2
5.0f, 1.0f, 5.0f,
*/
using namespace DirectX::SimpleMath;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG oMsg;
	ulong	iElaspedTime = 0;
	ulong	iLastTime = 0;

	hInst = hInstance;
	if (!CreateWindows(hInstance, nCmdShow, hWnd))
	{
		MessageBox(NULL, L"Erreur lors de la création de la fenêtre", L"Error", 0);
		return false;
	}
	g_pInputManager = new IAEngine::InputManager();
	if (!g_pInputManager->Create(hInst, hWnd))
	{
		MessageBox(NULL, L"Erreur lors de la création de l'input manager", L"Error", 0);
		delete g_pInputManager;
		return false;
	}
	if (!CreateDevice())
	{
		MessageBox(NULL, L"Erreur lors de la création du device DirectX 11", L"Error", 0);
		return false;
	}
	if (!CreateDefaultRT())
	{
		MessageBox(NULL, L"Erreur lors de la création des render targets", L"Error", 0);
		return false;
	}


	ID3D11RasterizerState* pRasterizerState;
	D3D11_RASTERIZER_DESC oDesc;
	ZeroMemory(&oDesc, sizeof(D3D11_RASTERIZER_DESC));
	oDesc.FillMode = D3D11_FILL_SOLID;
	//oDesc.FillMode = D3D11_FILL_WIREFRAME;
	oDesc.CullMode = D3D11_CULL_NONE;
	g_pDevice->CreateRasterizerState(&oDesc, &pRasterizerState);
	g_pImmediateContext->RSSetState(pRasterizerState);

	D3D11_VIEWPORT vp;
	vp.Width = WINDOW_WIDTH;
	vp.Height = WINDOW_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	//Create and fill other DirectX Stuffs like Vertex/Index buffer, shaders 
	ID3DBlob* vs;
	ID3D11VertexShader *pVS;
	CompileShader(L"BasicFireShader.fx", false, "DiffuseVS", &vs);
	g_pDevice->CreateVertexShader(vs->GetBufferPointer(), vs->GetBufferSize(), NULL, &pVS);
	g_pImmediateContext->VSSetShader(pVS, 0, 0);

	ID3DBlob* ps;
	ID3D11PixelShader *pPS;
	CompileShader(L"BasicFireShader.fx", true, "DiffusePS", &ps);
	g_pDevice->CreatePixelShader(ps->GetBufferPointer(), ps->GetBufferSize(), NULL, &pPS);
	g_pImmediateContext->PSSetShader(pPS, 0, 0);

	struct VERTEX
	{
		FLOAT x, y, z;
		FLOAT u, v;
	};

	struct VIEW_BUFFER
	{
		DirectX::SimpleMath::Matrix World;
	};

	struct NOISE_BUFFER
	{
		float frameTime;
		float scrollSpeedX;
		float scrollSpeedY;
		float scrollSpeedZ;
		float scaleX;
		float scaleY;
		float scaleZ;
		float padding;
	};

	struct DISTORTION_BUFFER
	{
		float distortion1X, distortion1Y;
		float distortion2X, distortion2Y;
		float distortion3X, distortion3Y;
		float distortionScale;
		float distortionBias;
	};

	float* m_height;
	unsigned short m_sizeX, m_sizeY;
	float m_maxZ = 50;
	auto width = 10, height = 10, size = width * height;

	VERTEX* vertices = new VERTEX[size];

	for (auto i = 0; i < size; i++)
	{
		float x = i / width;
		float y = i % width;
		float u = float(x) / float(width);
		float v = 1.0f - (float(y) / float(height));

		vertices[i] = { x, 0, y, u, v };
	}

	// INDEXES
	int nbSquares = (width - 1) * (height - 1);
	int sizeIndexes = nbSquares * (3 * 2);

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

	ID3D11Buffer *pIndexBuffer;

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
	ID3D11Buffer *pVBuffer;    // global

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
	bd.ByteWidth = sizeof(VERTEX) * size;             // size is the VERTEX struct * 3
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

	g_pDevice->CreateBuffer(&bd, NULL, &pVBuffer);       // create the buffer

	D3D11_MAPPED_SUBRESOURCE ms;
	g_pImmediateContext->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);   // map the buffer
	memcpy(ms.pData, vertices, sizeof(VERTEX) * size);                // copy the data
	g_pImmediateContext->Unmap(pVBuffer, NULL);                                     // unmap the buffer

																					// input layout
	ID3D11InputLayout *pLayout;    // global
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	g_pDevice->CreateInputLayout(ied, 2, vs->GetBufferPointer(), vs->GetBufferSize(), &pLayout);
	g_pImmediateContext->IASetInputLayout(pLayout);


	// world buffer
	ID3D11Buffer* g_pViewBuffer11 = NULL;
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VIEW_BUFFER);
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = 0;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	HRESULT hr = g_pDevice->CreateBuffer(&cbDesc, NULL, &g_pViewBuffer11);

	if (FAILED(hr))
		return hr;
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pViewBuffer11);

	// noise buffer
	ID3D11Buffer* g_pNoiseBuffer = NULL;
	D3D11_BUFFER_DESC noiseBuffer;
	noiseBuffer.ByteWidth = sizeof(VIEW_BUFFER);
	noiseBuffer.Usage = D3D11_USAGE_DEFAULT;
	noiseBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	noiseBuffer.CPUAccessFlags = 0;
	noiseBuffer.MiscFlags = 0;
	noiseBuffer.StructureByteStride = 0;

	hr = g_pDevice->CreateBuffer(&noiseBuffer, NULL, &g_pNoiseBuffer);

	if (FAILED(hr))
		return hr;
	g_pImmediateContext->VSSetConstantBuffers(1, 1, &g_pNoiseBuffer);

	// distortion buffer
	ID3D11Buffer* g_pDistortionBuffer = NULL;
	D3D11_BUFFER_DESC distortionBuffer;
	distortionBuffer.ByteWidth = sizeof(DISTORTION_BUFFER);
	distortionBuffer.Usage = D3D11_USAGE_DEFAULT;
	distortionBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	distortionBuffer.CPUAccessFlags = 0;
	distortionBuffer.MiscFlags = 0;
	distortionBuffer.StructureByteStride = 0;

	hr = g_pDevice->CreateBuffer(&distortionBuffer, NULL, &g_pDistortionBuffer);

	if (FAILED(hr))
		return hr;
	g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pDistortionBuffer);

	ID3D11Resource* texture;
	ID3D11ShaderResourceView* textureView;
	hr = DirectX::CreateDDSTextureFromFile(g_pDevice, L"data/fire01.dds", &texture, &textureView, 0, NULL);
	if (FAILED(hr)) {
		return hr;
	}
	g_pImmediateContext->PSSetShaderResources(0, 1, &textureView);

	ID3D11Resource* textureNoise;
	ID3D11ShaderResourceView* textureNoiseView;
	hr = DirectX::CreateDDSTextureFromFile(g_pDevice, L"data/noise01.dds", &textureNoise, &textureNoiseView, 0, NULL);
	if (FAILED(hr)) {
		return hr;
	}
	g_pImmediateContext->PSSetShaderResources(1, 1, &textureNoiseView);

	ID3D11Resource* textureAlpha;
	ID3D11ShaderResourceView* textureAlphaView;
	hr = DirectX::CreateDDSTextureFromFile(g_pDevice, L"data/alpha01.dds", &textureAlpha, &textureAlphaView, 0, NULL);
	if (FAILED(hr)) {
		return hr;
	}
	g_pImmediateContext->PSSetShaderResources(2, 1, &textureAlphaView);


	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	ID3D11SamplerState* m_sampleState;

	hr = g_pDevice->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(hr)) {
		return hr;
	}

	g_pImmediateContext->PSSetSamplers(0, 1, &m_sampleState);


	D3D11_SAMPLER_DESC samplerDesc2;
	samplerDesc2.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc2.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc2.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc2.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc2.MipLODBias = 0.0f;
	samplerDesc2.MaxAnisotropy = 1;
	samplerDesc2.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc2.MinLOD = 0;
	samplerDesc2.MaxLOD = D3D11_FLOAT32_MAX;
	ID3D11SamplerState* m_sampleState2;

	hr = g_pDevice->CreateSamplerState(&samplerDesc2, &m_sampleState2);
	if (FAILED(hr)) {
		return hr;
	}

	g_pImmediateContext->PSSetSamplers(1, 1, &m_sampleState2);


	float frameTime = 0.0f;

	IAEngine::FreeCamera oFreeCamera;
	iLastTime = timeGetTime();
	PeekMessage(&oMsg, NULL, 0, 0, PM_NOREMOVE);
	while (oMsg.message != WM_QUIT)
	{

		if (PeekMessage(&oMsg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&oMsg);
			DispatchMessage(&oMsg);
		}
		else
		{
			ulong iTime = timeGetTime();
			iElaspedTime = iTime - iLastTime;
			iLastTime = iTime;
			float fElaspedTime = iElaspedTime * 0.001f;

			g_pInputManager->Manage();

			ImGui_ImplDX11_NewFrame();
			ImGui::Begin("Menu Debug");
			ImGui::Text("Hello World Imgui");
			ImGui::End();

			oFreeCamera.Update(g_pInputManager, fElaspedTime);
			const Matrix& oViewMatrix = oFreeCamera.GetViewMatrix();
			Matrix oProjMatrix = Matrix::CreatePerspectiveFieldOfView(M_PI / 4.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.01f, 1000.0f);

			// Do a lot of thing like draw triangles with DirectX

			// Frametime
			frameTime += 0.01f;
			if (frameTime > 1000.0f)
				frameTime = 0.0f;

			// View with world buffer
			Matrix world = Matrix();
			VIEW_BUFFER constant_buffer;
			constant_buffer.World = (world * oViewMatrix * oProjMatrix).Transpose();
			g_pImmediateContext->UpdateSubresource(g_pViewBuffer11, 0, NULL, &constant_buffer, 0, 0);

			// Noise buffer
			NOISE_BUFFER noise_buffer;
			noise_buffer.frameTime = frameTime;
			noise_buffer.scrollSpeedX = 1.3f;
			noise_buffer.scrollSpeedY = 2.1f;
			noise_buffer.scrollSpeedZ = 2.3f;
			noise_buffer.scaleX = 1.0f;
			noise_buffer.scaleY = 2.0f;
			noise_buffer.scaleZ = 3.0f;
			noise_buffer.padding = 0.0f;
			g_pImmediateContext->UpdateSubresource(g_pNoiseBuffer, 0, NULL, &noise_buffer, 0, 0);

			// Distortion buffer
			DISTORTION_BUFFER distortion_buffer;
			distortion_buffer.distortion1X = 0.1f;
			distortion_buffer.distortion1Y = 0.2f;

			distortion_buffer.distortion2X = 0.1f;
			distortion_buffer.distortion2Y = 0.3f;

			distortion_buffer.distortion3X = 0.1f;
			distortion_buffer.distortion3Y = 0.1f;

			distortion_buffer.distortionScale = 0.8f;
			distortion_buffer.distortionBias = 0.5f;
			g_pImmediateContext->UpdateSubresource(g_pDistortionBuffer, 0, NULL, &distortion_buffer, 0, 0);

			g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);
			g_pImmediateContext->RSSetViewports(1, &vp);

			FLOAT rgba[] = { 1.0f, 1.0f, 1.0f, 0.0f };
			g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, rgba);
			g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0, 0);


			UINT stride = sizeof(VERTEX);
			UINT offset = 0;
			g_pImmediateContext->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
			g_pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
			g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			g_pImmediateContext->DrawIndexed(sizeIndexes, 0, 0);

			ImGui::Render();
			g_pSwapChain->Present(0, 0);
		}
	}
	//Release D3D objects
	ImGui_ImplDX11_Shutdown();
	g_pRenderTargetView->Release();
	g_pDepthStencilView->Release();
	g_pDepthStencil->Release();
	pRasterizerState->Release();
	g_pImmediateContext->Release();
	g_pSwapChain->Release();
	g_pDevice->Release();
	pVS->Release();
	pPS->Release();
	delete g_pInputManager;
	return (int)oMsg.wParam;
}

bool CreateDevice()
{
	UINT Flags = D3D11_CREATE_DEVICE_DEBUG;

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = WINDOW_WIDTH;
	sd.BufferDesc.Height = WINDOW_HEIGHT;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;// 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, Flags, NULL, 0, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pDevice, NULL, &g_pImmediateContext);
	if (FAILED(hr))
		return false;
	ImGui_ImplDX11_Init(hWnd, g_pDevice, g_pImmediateContext);

	return true;
}

bool CreateDefaultRT()
{

	ID3D11Texture2D*	pBackBuffer;
	if (FAILED(g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer)))
		return false;

	HRESULT hr = g_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
	pBackBuffer->Release();



	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = WINDOW_WIDTH;
	descDepth.Height = WINDOW_HEIGHT;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = g_pDevice->CreateTexture2D(&descDepth, NULL, &g_pDepthStencil);
	if (FAILED(hr))
		return false;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = g_pDevice->CreateDepthStencilView(g_pDepthStencil, &descDSV, &g_pDepthStencilView);
	if (FAILED(hr))
		return false;
	return true;
}

// entrypoint : fonction dans le shader, ID3DBlob : bytecode compilé
bool CompileShader(LPCWSTR pFileName, bool bPixel, LPCSTR pEntrypoint, ID3DBlob** ppCompiledShader)
{
	ID3DBlob* pErrorMsg = NULL;
	HRESULT hr = D3DCompileFromFile(pFileName, NULL, NULL, pEntrypoint, bPixel ? "ps_5_0" : "vs_5_0", 0, 0, ppCompiledShader, &pErrorMsg);

	if (FAILED(hr))
	{
		if (pErrorMsg != NULL)
		{
			OutputDebugStringA((char*)pErrorMsg->GetBufferPointer());
			pErrorMsg->Release();
		}
		return false;
	}
	if (pErrorMsg)
		pErrorMsg->Release();
	return true;
}

bool LoadRAW(const std::string& map, float** m_height, unsigned short *m_sizeX, unsigned short *m_sizeY, const float *m_maxZ)
{
	FILE *file;
	fopen_s(&file, map.c_str(), "rb");
	if (!file)
		return false;

	//lit la taille en X et en Y depuis le fichier
	fread(m_sizeX, sizeof(unsigned short), 1, file);
	fread(m_sizeY, sizeof(unsigned short), 1, file);
	unsigned int size = *m_sizeX * *m_sizeY;
	unsigned char *tmp = new unsigned char[size];
	*m_height = new float[size];
	fread(tmp, sizeof(unsigned char), size, file);
	fclose(file);
	int i = 0;
	for (unsigned short y = 0; y < *m_sizeY; ++y)
		for (unsigned short x = 0; x < *m_sizeX; ++x, ++i)
			(*m_height)[i] = float((*m_maxZ * tmp[i]) / 255.0f);
	delete[] tmp;
	return true;
}
