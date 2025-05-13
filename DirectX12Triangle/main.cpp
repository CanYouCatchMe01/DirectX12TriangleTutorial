#include <windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

void set_blend_state(D3D12_BLEND_DESC &blend_desc) {
    blend_desc = {};

    blend_desc.AlphaToCoverageEnable = FALSE;
    blend_desc.IndependentBlendEnable = FALSE;

    D3D12_RENDER_TARGET_BLEND_DESC default_render_target_blend_desc = {};
    default_render_target_blend_desc.BlendEnable = FALSE;
    default_render_target_blend_desc.LogicOpEnable = FALSE;
    default_render_target_blend_desc.SrcBlend = D3D12_BLEND_ONE;
    default_render_target_blend_desc.DestBlend = D3D12_BLEND_ZERO;
    default_render_target_blend_desc.BlendOp = D3D12_BLEND_OP_ADD;
    default_render_target_blend_desc.SrcBlendAlpha = D3D12_BLEND_ONE;
    default_render_target_blend_desc.DestBlendAlpha = D3D12_BLEND_ZERO;
    default_render_target_blend_desc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
    default_render_target_blend_desc.LogicOp = D3D12_LOGIC_OP_NOOP;
    default_render_target_blend_desc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++) {
        blend_desc.RenderTarget[i] = default_render_target_blend_desc;
    }
}

void set_rasterizer_state(D3D12_RASTERIZER_DESC &rasterizer_desc) {
    rasterizer_desc = {};

    rasterizer_desc.FillMode = D3D12_FILL_MODE_SOLID;
    rasterizer_desc.CullMode = D3D12_CULL_MODE_BACK;
    rasterizer_desc.FrontCounterClockwise = FALSE;
    rasterizer_desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    rasterizer_desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    rasterizer_desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    rasterizer_desc.DepthClipEnable = TRUE;
    rasterizer_desc.MultisampleEnable = FALSE;
    rasterizer_desc.AntialiasedLineEnable = FALSE;
    rasterizer_desc.ForcedSampleCount = 0;
    rasterizer_desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
}

void set_depth_stencil_state(D3D12_DEPTH_STENCIL_DESC &depth_stencil_desc) {
    depth_stencil_desc = {};

    depth_stencil_desc.DepthEnable = FALSE;
    depth_stencil_desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depth_stencil_desc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;

    depth_stencil_desc.StencilEnable = FALSE;
    depth_stencil_desc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
    depth_stencil_desc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

    depth_stencil_desc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    depth_stencil_desc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    depth_stencil_desc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

    depth_stencil_desc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    depth_stencil_desc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    depth_stencil_desc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    depth_stencil_desc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    const int width = 800;
    const int height = 800;

    // Register a simple window class
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"DirectX12Triangle";
    RegisterClass(&wc);

    // Create a window
    HWND hwnd = CreateWindow(wc.lpszClassName, L"DirectX 12 Triangle", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, hInstance, nullptr);
    ShowWindow(hwnd, nCmdShow);

    //The device is like a virtual representation of the GPU
    ID3D12Device *device = nullptr;
    HRESULT hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));

    //command queue decides which order the command lists should execute. In our case, we only have one command list.
    ID3D12CommandQueue *command_queue = nullptr;
    D3D12_COMMAND_QUEUE_DESC queue_desc = {};
    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    hr = device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&command_queue));

    //command allocator is used to allocate memory on the GPU for commands
    ID3D12CommandAllocator *command_allocator = nullptr;
    hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&command_allocator));
    hr = command_allocator->Reset();

    //command list is used to store a list of commands that we want to execute on the GPU
    ID3D12GraphicsCommandList *command_list = nullptr;
    hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocator, nullptr, IID_PPV_ARGS(&command_list));
    hr = command_list->Close();

    //helper object to create a swap chain
    IDXGIFactory4 *factory = nullptr;
    hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory));

    //create swap chain
    DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
    swap_chain_desc.BufferCount = 2;
    swap_chain_desc.BufferDesc.Width = width;
    swap_chain_desc.BufferDesc.Height = height;
    swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swap_chain_desc.OutputWindow = hwnd;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.Windowed = TRUE;

    IDXGISwapChain *temp_swap_chain = nullptr;
    hr = factory->CreateSwapChain(command_queue, &swap_chain_desc, &temp_swap_chain);

    //cast the swap chain to IDXGISwapChain3 to leverage the latest features
    IDXGISwapChain3 *swap_chain = {};
    hr = temp_swap_chain->QueryInterface(IID_PPV_ARGS(&swap_chain));
    temp_swap_chain->Release();
    temp_swap_chain = nullptr;

    //memory descriptor heap to store render target views(RTV). Descriptor describes how to interperate resource memory.
    ID3D12DescriptorHeap *rtv_heap = nullptr;
    D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {};
    rtv_heap_desc.NumDescriptors = 2;
    rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    hr = device->CreateDescriptorHeap(&rtv_heap_desc, IID_PPV_ARGS(&rtv_heap));

    ID3D12Resource *render_targets[2];

    UINT rtv_increment_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    {
        D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle(rtv_heap->GetCPUDescriptorHandleForHeapStart());
        for (UINT i = 0; i < 2; i++) {
            hr = swap_chain->GetBuffer(i, IID_PPV_ARGS(&render_targets[i]));

            device->CreateRenderTargetView(render_targets[i], nullptr, rtv_handle);
            rtv_handle.ptr += rtv_increment_size;
        }
    }

    //fence is used to synchronize the CPU with the GPU, so they don't touch the same memory at the same time
    ID3D12Fence *fence = nullptr;
    UINT64 fence_value = 0;
    hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

    HANDLE fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    //Root signature is like have many object buffers and textures we want to use when drawing.
    //For our rotating triangle, we only need a single constant that is going to be our angle
    D3D12_ROOT_PARAMETER root_parameters[1] = {};
    root_parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    root_parameters[0].Constants.Num32BitValues = 1;
    root_parameters[0].Constants.ShaderRegister = 0;
    root_parameters[0].Constants.RegisterSpace = 0;
    root_parameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

    ID3D12RootSignature *root_signature = nullptr;
    D3D12_ROOT_SIGNATURE_DESC root_signature_desc = {};
    root_signature_desc.NumParameters = _countof(root_parameters);
    root_signature_desc.pParameters = root_parameters;
    root_signature_desc.NumStaticSamplers = 0;
    root_signature_desc.pStaticSamplers = nullptr;
    root_signature_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ID3DBlob *signature_blob = nullptr;
    ID3DBlob *error_blob = nullptr;
    hr = D3D12SerializeRootSignature(&root_signature_desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature_blob, &error_blob);
    hr = device->CreateRootSignature(0, signature_blob->GetBufferPointer(), signature_blob->GetBufferSize(), IID_PPV_ARGS(&root_signature));

    if (signature_blob) {
        signature_blob->Release();
        signature_blob = nullptr;
    }
    if (error_blob) {
        error_blob->Release();
        error_blob = nullptr;
    }

    //compile shaders
    ID3DBlob *vertex_shader = nullptr;
    ID3DBlob *pixel_shader = nullptr;
    hr = D3DCompileFromFile(L"shader.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, &vertex_shader, nullptr);
    hr = D3DCompileFromFile(L"shader.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", 0, 0, &pixel_shader, nullptr);

    // Pipeline state
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};
    pso_desc.pRootSignature = root_signature;
    pso_desc.VS.pShaderBytecode = vertex_shader->GetBufferPointer();
    pso_desc.VS.BytecodeLength = vertex_shader->GetBufferSize();
    pso_desc.PS.pShaderBytecode = pixel_shader->GetBufferPointer();
    pso_desc.PS.BytecodeLength = pixel_shader->GetBufferSize();
    set_blend_state(pso_desc.BlendState);
    pso_desc.SampleMask = UINT_MAX;
    set_rasterizer_state(pso_desc.RasterizerState);
    set_depth_stencil_state(pso_desc.DepthStencilState);
    pso_desc.InputLayout.pInputElementDescs = nullptr;
    pso_desc.InputLayout.NumElements = 0;
    pso_desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
    pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pso_desc.NumRenderTargets = 1;
    pso_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    pso_desc.SampleDesc.Count = 1;
    pso_desc.SampleDesc.Quality = 0;

    ID3D12PipelineState *pipeline_state = nullptr;
    hr = device->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&pipeline_state));

    vertex_shader->Release();
    vertex_shader = nullptr;
    pixel_shader->Release();
    pixel_shader = nullptr;

    unsigned int triangle_angle = 0;

    bool running = true;
    while (running) {
        MSG msg = {};
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = false;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Record commands to draw a triangle
        hr = command_allocator->Reset();
        hr = command_list->Reset(command_allocator, nullptr);

        UINT back_buffer_index = swap_chain->GetCurrentBackBufferIndex();

        {
            D3D12_RESOURCE_BARRIER barrier = {};
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barrier.Transition.pResource = render_targets[back_buffer_index];
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            command_list->ResourceBarrier(1, &barrier);
        }

        D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = rtv_heap->GetCPUDescriptorHandleForHeapStart();
        rtv_handle.ptr += back_buffer_index * rtv_increment_size;

        // Clear the render target
        float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
        command_list->ClearRenderTargetView(rtv_handle, clearColor, 0, nullptr);

        // Set viewport and scissor
        D3D12_VIEWPORT viewport = { 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f };
        D3D12_RECT scissor_rect = { 0, 0, LONG_MAX, LONG_MAX };
        command_list->RSSetViewports(1, &viewport);
        command_list->RSSetScissorRects(1, &scissor_rect);

        command_list->OMSetRenderTargets(1, &rtv_handle, FALSE, nullptr);
        command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        command_list->SetGraphicsRootSignature(root_signature);
        command_list->SetPipelineState(pipeline_state);

        // Draw the triangle
        command_list->SetGraphicsRoot32BitConstant(0, triangle_angle, 0);
        command_list->DrawInstanced(3, 1, 0, 0);

        {
            D3D12_RESOURCE_BARRIER barrier = {};
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barrier.Transition.pResource = render_targets[back_buffer_index];
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            command_list->ResourceBarrier(1, &barrier);
        }

        hr = command_list->Close();

        ID3D12CommandList *command_lists[] = { command_list };
        command_queue->ExecuteCommandLists(1, command_lists);

        hr = swap_chain->Present(1, 0);

        // Wait on the CPU for the GPU frame to finish
        const UINT64 current_fence_value = ++fence_value;
        hr = command_queue->Signal(fence, current_fence_value);

        if (fence->GetCompletedValue() < current_fence_value) {
            hr = fence->SetEventOnCompletion(current_fence_value, fence_event);
            WaitForSingleObject(fence_event, INFINITE);
        }

        triangle_angle++;
    }
}