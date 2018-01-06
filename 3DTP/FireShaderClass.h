#pragma once

#include "3DTP.h"
#include "InputManager.h"
#include "D3Dcompiler.h"
#include "Camera.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "DirectXTK\DDSTextureLoader.h"

using namespace DirectX::SimpleMath;

class FireShaderClass
{
	private:
		struct MATRIX_BUFFER
		{
			Matrix world, view, projection;
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
			Vector2 distortion1, distortion2, distortion3;
			FLOAT distortionScale, distortionBias;
		};

	public:
		FireShaderClass();

		bool Initialize(ID3D11Device*, HWND);
		bool Render(ID3D11DeviceContext*, int, Matrix, Matrix, Matrix, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
			ID3D11ShaderResourceView*, float, Vector3, Vector3, Vector2, Vector2, Vector2, float, float);

	private:
		bool InitializeShader(ID3D11Device*, HWND, LPCWSTR, LPCSTR, LPCSTR);

		bool SetShaderParameters(ID3D11DeviceContext*, Matrix, Matrix, Matrix, ID3D11ShaderResourceView*,
			ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, float, Vector3, Vector3, Vector2,
			Vector2, Vector2, float, float);
		void RenderShader(ID3D11DeviceContext*, int);

	private:
		ID3D11VertexShader* m_vertexShader;
		ID3D11PixelShader* m_pixelShader;
		ID3D11InputLayout* m_layout;
		ID3D11Buffer* m_matrixBuffer;
		ID3D11Buffer* m_noiseBuffer;
		ID3D11SamplerState* m_sampleState;
		ID3D11SamplerState* m_sampleState2;
		ID3D11Buffer* m_distortionBuffer;
};