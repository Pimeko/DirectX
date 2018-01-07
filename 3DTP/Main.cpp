#include "stdafx.h"
#include "3DTP.h"
#include "InputManager.h"
#include "D3Dcompiler.h"
#include "Camera.h"
#include "Model.h"
#include "FireShaderClass.h"

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


	D3D11_VIEWPORT vp;
	vp.Width = WINDOW_WIDTH;
	vp.Height = WINDOW_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	/*struct VERTEX
	{
		FLOAT x, y, z;
		FLOAT u, v;
	};

	struct MATRIX_BUFFER
	{
		DirectX::SimpleMath::Matrix World;
	};

	struct NOISE_BUFFER
	{
		FLOAT frameTime;
		Vector3 scrollSpeeds;
		Vector3 scales;
		FLOAT padding;
	};

	struct DISTORTION_BUFFER
	{
		Vector3 distortion1, distortion2, distortion3;
		FLOAT distortionScale, distortionBias;
	};


	ID3D11RasterizerState* pRasterizerState;
	D3D11_RASTERIZER_DESC oDesc;
	ZeroMemory(&oDesc, sizeof(D3D11_RASTERIZER_DESC));
	oDesc.FillMode = D3D11_FILL_SOLID;
	//oDesc.FillMode = D3D11_FILL_WIREFRAME;
	oDesc.CullMode = D3D11_CULL_NONE;
	g_pDevice->CreateRasterizerState(&oDesc, &pRasterizerState);
	g_pImmediateContext->RSSetState(pRasterizerState);
	*/

	/*ID3DBlob* vs;
	ID3D11VertexShader *pVS;
	CompileShader(L"FireShader.fx", false, "DiffuseVS", &vs);
	g_pDevice->CreateVertexShader(vs->GetBufferPointer(), vs->GetBufferSize(), NULL, &pVS);
	//g_pImmediateContext->VSSetShader(pVS, 0, 0); METTRE PLUS TARD

	ID3DBlob* ps;
	ID3D11PixelShader *pPS;
	CompileShader(L"FireShader.fx", true, "DiffusePS", &ps);
	g_pDevice->CreatePixelShader(ps->GetBufferPointer(), ps->GetBufferSize(), NULL, &pPS);
	//g_pImmediateContext->PSSetShader(pPS, 0, 0); METTRE PLUS TARD

	ID3D11InputLayout *pLayout;
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	g_pDevice->CreateInputLayout(ied, 2, vs->GetBufferPointer(), vs->GetBufferSize(), &pLayout);
	g_pImmediateContext->IASetInputLayout(pLayout);

	// Matrix Buffer
	ID3D11Buffer *m_matrixBuffer;

	D3D11_BUFFER_DESC matrixBufferDesc;
	ZeroMemory(&matrixBufferDesc, sizeof(matrixBufferDesc));

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MATRIX_BUFFER);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = g_pDevice->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(hr))
		return hr;*/

	/*
	auto width = 10;
	auto height = 10;
	auto size = width * height;

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
	*/

	// Noise Buffer
	/*ID3D11Buffer *m_noiseBuffer;
	D3D11_BUFFER_DESC noiseBuffer;
	ZeroMemory(&noiseBuffer, sizeof(noiseBuffer));

	noiseBuffer.Usage = D3D11_USAGE_DYNAMIC;
	noiseBuffer.ByteWidth = sizeof(NOISE_BUFFER);
	noiseBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	noiseBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = g_pDevice->CreateBuffer(&noiseBuffer, NULL, &m_noiseBuffer);
	if (FAILED(hr))
		return hr;

	// Sampler Desc Noise
	D3D11_SAMPLER_DESC samplerDescNoise;
	samplerDescNoise.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDescNoise.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescNoise.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescNoise.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescNoise.MipLODBias = 0.0f;
	samplerDescNoise.MaxAnisotropy = 1;
	samplerDescNoise.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDescNoise.BorderColor[0] = 0;
	samplerDescNoise.BorderColor[1] = 0;
	samplerDescNoise.BorderColor[2] = 0;
	samplerDescNoise.BorderColor[3] = 0;
	samplerDescNoise.MinLOD = 0;
	samplerDescNoise.MaxLOD = D3D11_FLOAT32_MAX;
	ID3D11SamplerState* m_sampleState;

	hr = g_pDevice->CreateSamplerState(&samplerDescNoise, &m_sampleState);
	if (FAILED(hr))
		return hr;

	// Sampler Desc Clamp
	D3D11_SAMPLER_DESC samplerDescClamp;
	samplerDescClamp.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDescClamp.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDescClamp.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDescClamp.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDescClamp.MipLODBias = 0.0f;
	samplerDescClamp.MaxAnisotropy = 1;
	samplerDescClamp.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDescClamp.BorderColor[0] = 0;
	samplerDescClamp.BorderColor[1] = 0;
	samplerDescClamp.BorderColor[2] = 0;
	samplerDescClamp.BorderColor[3] = 0;
	samplerDescClamp.MinLOD = 0;
	samplerDescClamp.MaxLOD = D3D11_FLOAT32_MAX;
	ID3D11SamplerState* m_sampleStateClamp;

	hr = g_pDevice->CreateSamplerState(&samplerDescClamp, &m_sampleStateClamp);

	if (FAILED(hr))
		return hr;

	// Distortion Buffer
	ID3D11Buffer *m_distortionBuffer;
	D3D11_BUFFER_DESC distortionBufferDesc;
	ZeroMemory(&distortionBufferDesc, sizeof(distortionBufferDesc));

	distortionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	distortionBufferDesc.ByteWidth = sizeof(NOISE_BUFFER);
	distortionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	distortionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = g_pDevice->CreateBuffer(&distortionBufferDesc, NULL, &m_distortionBuffer);

	if (FAILED(hr))
		return hr;*/

	/*
	// Index Buffer
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

	

	D3D11_MAPPED_SUBRESOURCE ms;
	g_pImmediateContext->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);   // map the buffer
	memcpy(ms.pData, vertices, sizeof(VERTEX) * size);                // copy the data
	g_pImmediateContext->Unmap(pVBuffer, NULL);                                     // unmap the buffer

	

#pragma region
	ID3D11Resource* texture;
	ID3D11ShaderResourceView* textureView;
	hr = DirectX::CreateDDSTextureFromFile(g_pDevice, L"fire.dds", &texture, &textureView, 0, NULL);
	if (FAILED(hr)) {
		return hr;
	}
	g_pImmediateContext->PSSetShaderResources(0, 1, &textureView);

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = -FLT_MAX;
	samplerDesc.MaxLOD = FLT_MAX;
	ID3D11SamplerState* myLinearWrapSampler;

	hr = g_pDevice->CreateSamplerState(&samplerDesc, &myLinearWrapSampler);

	if (FAILED(hr)) {
		return hr;
	}
	
	ID3D11Resource* textureDetail;
	ID3D11ShaderResourceView* textureViewDetail;
	hr = DirectX::CreateDDSTextureFromFile(g_pDevice, L"detail.dds", &textureDetail, &textureViewDetail, 0, NULL);

	if (FAILED(hr)) {
		return hr;
	}
	
	g_pImmediateContext->PSSetShaderResources(1, 1, &textureViewDetail);
	

#pragma endregion Texture

	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer11);
	*/

	struct CONSTANT_BUFFER
	{
		Matrix world;
	};

	ID3D11Buffer* g_pConstantBuffer11 = NULL;

	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(CONSTANT_BUFFER);
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = 0;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	HRESULT hr = g_pDevice->CreateBuffer(&cbDesc, NULL, &g_pConstantBuffer11);
	if (FAILED(hr))
		MessageBox(NULL, L"a", L"Error", 0);

	// MODEL
	struct ModelType
	{
		float x, y, z;
		float tu, tv;
	};

	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	Texture *m_Texture1, *m_Texture2, *m_Texture3;
	ModelType* m_model;

	m_vertexCount = 3;
	m_indexCount = m_vertexCount;

	m_model = new ModelType[m_vertexCount];
	m_model[0] = { -1.0f, 1.0f, 0.0f, 0.0f, 1.0f };
	m_model[1] = { 0.0f,  1.0f, 0.0f, 0.5f, 0.0f };
	m_model[2] = { 1.0f, -1.0f, 0.0f, 1.0f, 1.0f };

	// INITIALIZE MODEL
	struct VertexType
	{
		Vector3 position;
		Vector2 texture;
	};
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;


	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];

	// Create the index array.
	indices = new unsigned long[m_indexCount];

	// Load the vertex array and index array with data.
	for (i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = Vector3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = Vector2(m_model[i].tu, m_model[i].tv);
		indices[i] = i;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = g_pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
		return result;

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = g_pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
		return result;

	// TEXTURE MODEL

	ID3D11ShaderResourceView* m_texture;
	ID3D11Resource* texture;
	result = DirectX::CreateDDSTextureFromFile(g_pDevice, L"data/fire01.dds", &texture, &m_texture, 0, NULL);
	if (FAILED(result))
		return result;

	/*Model* m_Model = new Model;
	if (!m_Model)
		MessageBox(NULL, L"b", L"Error", 0);



	m_Model->Initialize(g_pDevice, L"data/fire01.dds", L"data/noise01.dds", L"data/alpha01.dds");

	FireShaderClass* m_FireShader = new FireShaderClass;
	m_FireShader->Initialize(g_pDevice, hWnd);*/

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
			Matrix world = Matrix();
			CONSTANT_BUFFER constant_buffer;
			constant_buffer.world = (world * oViewMatrix * oProjMatrix).Transpose();
			g_pImmediateContext->UpdateSubresource(g_pConstantBuffer11, 0, NULL, &constant_buffer, 0, 0);

			// MODEL RENDER

			unsigned int stride;
			unsigned int offset;

			// Set vertex buffer stride and offset.
			stride = sizeof(VertexType);
			offset = 0;

			// Set the vertex buffer to active in the input assembler so it can be rendered.
			g_pImmediateContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

			// Set the index buffer to active in the input assembler so it can be rendered.
			g_pImmediateContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

			// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
			g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			/*
			bool result;
			Vector3 scrollSpeeds, scales;
			Vector2 distortion1, distortion2, distortion3;
			float distortionScale, distortionBias;
			static float frameTime = 0.0f;

			// Increment the frame time counter.
			frameTime += 0.01f;
			if (frameTime > 1000.0f)
			{
				frameTime = 0.0f;
			}

			// Set the three scrolling speeds for the three different noise textures.
			scrollSpeeds = Vector3(1.3f, 2.1f, 2.3f);

			// Set the three scales which will be used to create the three different noise octave textures.
			scales = Vector3(1.0f, 2.0f, 3.0f);

			// Set the three different x and y distortion factors for the three different noise textures.
			distortion1 = Vector2(0.1f, 0.2f);
			distortion2 = Vector2(0.1f, 0.3f);
			distortion3 = Vector2(0.1f, 0.1f);

			// The the scale and bias of the texture coordinate sampling perturbation.
			distortionScale = 0.8f;
			distortionBias = 0.5f;

			// Put the square model vertex and index buffers on the graphics pipeline to prepare them for drawing.
			m_Model->Render(g_pImmediateContext);

			// Render the square model using the fire shader.
			result = m_FireShader->Render(g_pImmediateContext, m_Model->GetIndexCount(), world, oViewMatrix, oProjMatrix,
				m_Model->GetTexture1(), m_Model->GetTexture2(), m_Model->GetTexture3(), frameTime, scrollSpeeds,
				scales, distortion1, distortion2, distortion3, distortionScale, distortionBias);
			if (!result)
				MessageBox(NULL, L"c", L"Error", 0);

			*/


			g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);
			g_pImmediateContext->RSSetViewports(1, &vp);

			FLOAT rgba[] = { 0.0f, 0.0f, 0.0f, 0.0f };
			g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, rgba);
			g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0, 0);

			//UINT stride = sizeof(VERTEX);
			//UINT offset = 0;
			/*g_pImmediateContext->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
			g_pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
			g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			g_pImmediateContext->DrawIndexed(sizeIndexes, 0, 0);*/

			ImGui::Render();
			g_pSwapChain->Present(0, 0);
		}
	}
	//Release D3D objects
	ImGui_ImplDX11_Shutdown();
	g_pRenderTargetView->Release();
	g_pDepthStencilView->Release();
	g_pDepthStencil->Release();
	//pRasterizerState->Release();
	g_pImmediateContext->Release();
	g_pSwapChain->Release();
	g_pDevice->Release();
	//pVS->Release();
	//pPS->Release();
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
