/******************************************************
* Copyright(C) VERTVER, 2018. All rights reserved.
* IGUH - "Internal graphic used here" engine.
* MIT - License
* *********************************************************
* Module Name : IGUH render entry-point
**********************************************************
* IG_RenderEntry.cpp
* Render entry-point
*********************************************************/

#include "IG_Render.h"

IGCODE 
IGRender::BaseRender::SetRenderBegin(
	_In_ HWND hwnd,
	_In_ DWORD dwFlags
)
{
	DWORD dwDescSize = NULL;
	UINT dxgiFactoryFlags = NULL;
	IDXGIAdapter1* pTempAdapter = nullptr;
	IDXGISwapChain1* pSwapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 SwapChainDesc = {};
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	DXGI_ADAPTER_DESC1 Desc = {};

	// create dxgi factory
	if (FAILED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_pDXGIFactory))))
	{
		return IGDX_BAD_DEVICE;
	}

	if (FAILED(m_pDXGIFactory->EnumWarpAdapter(IID_PPV_ARGS(&m_pDXGIAdapter))))
	{
		return IGDX_BAD_DEVICE;
	}

#ifdef WIN10_FEATURES
	// create d3d device
	if (FAILED(D3D12CreateDevice(m_pDXGIAdapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_pDevice))))
	{
		if (FAILED(D3D12CreateDevice(m_pDXGIAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_p11Device))))
		{
			return IGDX_BAD_DEVICE;
		}
	}

	// describe and create the command queue.

	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	if (FAILED(m_pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_pCommandQueue))))
	{
		DEBUG_MESSAGE("D3D::CQ 1");
	}

	SwapChainDesc.BufferCount = 3;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.Height = dwDeviceHeight;
	SwapChainDesc.Scaling = DXGI_SCALING_NONE;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	SwapChainDesc.Width = dwDeviceWidth;
	SwapChainDesc.Flags = 0;

	// create swap chain for hwnd without fullscreen mode
	if (FAILED(m_pDXGIFactory->CreateSwapChainForHwnd(m_pCommandQueue, hwnd, &SwapChainDesc, nullptr, nullptr, &pSwapChain)))
	{
		return IGDX_BAD_DEVICE;
	}

#else
	//#TODO:
	//if (FAILED(D3D11CreateDevice(m_pDXGIAdapter, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, D3D_FEATURE_LEVEL_11_0)))
#endif

	// member fucking stupid faggot: always query interface
	pSwapChain->QueryInterface(&m_pDXGISwapChain);
	_RELEASE(pSwapChain);
	
	// Create descriptor heaps.
	{
		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = 3;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		m_pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));

		// Describe and create a depth stencil view (DSV) descriptor heap.
		// Each frame has its own depth stencils (to write shadows onto) 
		// and then there is one for the scene itself.
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = 1 + 3 * 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		m_pDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap));

		// Describe and create a shader resource view (SRV) and constant 
		// buffer view (CBV) descriptor heap.  Heap layout: null views, 
		// object diffuse + normal textures views, frame 1's shadow buffer, 
		// frame 1's 2x constant buffer, frame 2's shadow buffer, frame 2's 
		// 2x constant buffers, etc...
		const UINT nullSrvCount = 2;		// Null descriptors are needed for out of bounds behavior reads.
		const UINT cbvCount = 6;
		const UINT srvCount = 8;
		D3D12_DESCRIPTOR_HEAP_DESC cbvSrvHeapDesc = {};
		cbvSrvHeapDesc.NumDescriptors = nullSrvCount + cbvCount + srvCount;
		cbvSrvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		cbvSrvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		m_pDevice->CreateDescriptorHeap(&cbvSrvHeapDesc, IID_PPV_ARGS(&m_cbvSrvHeap));

		// Describe and create a sampler descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC samplerHeapDesc = {};
		samplerHeapDesc.NumDescriptors = 2;		// One clamp and one wrap sampler.
		samplerHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		samplerHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		m_pDevice->CreateDescriptorHeap(&samplerHeapDesc, IID_PPV_ARGS(&m_samplerHeap));

		dwDescSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pCommandAllocator));
#if 0
	// get size for descriptors
	DWORD dwRTVDescSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	DWORD dwSamplerDescSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	DWORD dwSRV_UAV_CBVDescSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// create command queue desc for command list struct
	D3D12_COMMAND_QUEUE_DESC commandDesc = { };
	ZeroMemory(&commandDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	commandDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

	if (FAILED(m_pDevice->CreateCommandQueue(&commandDesc, IID_PPV_ARGS(&m_pCommandQueue))))
	{
		return IGDX_COMMAND_LIST_ERROR;
	}

	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
	ZeroMemory(&featureData, sizeof(D3D12_FEATURE_DATA_ROOT_SIGNATURE));

	// create range descriptors for load shaders
	CD3DX12_DESCRIPTOR_RANGE1 descriptorRange[1];
	descriptorRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
	
	// create root params to constant buffer view
	CD3DX12_ROOT_PARAMETER1 rootParameters[2];
	rootParameters[0].InitAsConstantBufferView(NULL);
	rootParameters[1].InitAsDescriptorTable(1, &descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

	CD3DX12_STATIC_SAMPLER_DESC StaticSamplerDesc(0);
	StaticSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	StaticSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC RootSignatureDesc;
	RootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &StaticSamplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	D3D12_INPUT_ELEMENT_DESC InputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

	if (FAILED(m_pDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
	{
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	/********************************************
	* #TODO: Nextly go code for compile shaders
	********************************************/
#endif
	return IG_SUCCESS;
}
