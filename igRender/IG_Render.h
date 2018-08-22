/******************************************************
* Copyright(C) VERTVER, 2018. All rights reserved.
* IGUH - "Internal graphic used here" engine.
* MIT - License
* *********************************************************
* Module Name : IGUH render header
**********************************************************
* IG_Render.h
* Master-include for render
*********************************************************/

#include "IG_Kernel.h"
#include <d3d12.h>
#include "d3dx12.h"
#include <d3d11on12.h>
#include <DirectXMath.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <dxgi1_3.h>
#pragma once

#define IG_EXPORTS
namespace IGRender
{
	class BaseRender
	{
	public:
		IGAPI IGCODE SetRenderBegin(_In_ HWND hwnd, _In_ DWORD dwFlags);
	private:
		DWORD dwDeviceWidth;
		DWORD dwDeviceHeight;
	protected:
		IDXGIFactory4* m_pDXGIFactory = nullptr;
		IDXGIAdapter3* m_pDXGIAdapter = nullptr;
		ID3D12Device* m_pDevice = nullptr;
		ID3D12DescriptorHeap* m_pRtvHeap = nullptr;
		ID3D12CommandQueue* m_pCommandQueue = nullptr;
		ID3D12CommandAllocator* m_pCommandAllocator = nullptr;
		ID3D12RootSignature* m_rootSignature = nullptr;
		ID3D12DescriptorHeap* m_rtvHeap = nullptr;
		ID3D12DescriptorHeap* m_dsvHeap = nullptr;
		ID3D12DescriptorHeap* m_cbvSrvHeap = nullptr;
		ID3D12DescriptorHeap* m_samplerHeap = nullptr;
		ID3D12PipelineState* m_pipelineState = nullptr;
		ID3D12PipelineState* m_pipelineStateShadowMap = nullptr;
		IDXGISwapChain3* m_pDXGISwapChain = nullptr;
		ID3D12Resource* m_pRenderTargets[2];
		ID3D12Resource* m_pStagingSurface = nullptr;
		LPVOID m_pStagingSurfaceData = nullptr;

		// d3d11 for nuklear working
		ID3D11Device* m_p11Device = nullptr;
		ID3D11DeviceContext* m_p11Context = nullptr;
		ID3D11On12Device* m_p11On12Device = nullptr;
	};
}