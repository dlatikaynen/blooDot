﻿#include "..\PreCompiledHeaders.h"
#include "blooDot.h"
#include <DirectXColors.h> // For named colors
#include "..\dx\DirectXHelper.h" // For ThrowIfFailed

using namespace blooDot;
using namespace Windows::Gaming::Input;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::System::Diagnostics;

const byte blooDotMain::BLOODOTFILE_SIGNATURE[8]{ 0x03, 'L','S','L', 0x04, 'J','M','L' };
const byte blooDotMain::BLOODOTFILE_CONTENTTYPE_GOLANIMATION[2]{ 0xc9, 0x05 };

inline D2D1_RECT_F ConvertRect(Windows::Foundation::Size source)
{
    // ignore the source.X and source.Y  These are the location on the screen
    // yet we don't want to use them because all coordinates are window relative.
    return D2D1::RectF(0.0f, 0.0f, source.Width, source.Height);
}

inline void InflateRect(D2D1_RECT_F& rect, float x, float y)
{
    rect.left -= x;
    rect.right += x;
    rect.top -= y;
    rect.bottom += y;
}

// Loads and initializes application assets when the application is loaded.
blooDotMain::blooDotMain(const std::shared_ptr<DX::DeviceResources>& deviceResources) : 
    m_deviceResources(deviceResources),
    m_gameState(GameState::Initial),
    m_windowActive(false),
	m_deferredResourcesReadyPending(false),
    m_deferredResourcesReady(false),
	m_FPSCircular(0),
	m_FPSWatermark(0)
{
    // Register to be notified if the Device is lost or recreated.
    m_deviceResources->RegisterDeviceNotify(this);

    ZeroMemory(&m_mazeConstantBufferData, sizeof(m_mazeConstantBufferData));
    ZeroMemory(&m_marbleConstantBufferData, sizeof(m_marbleConstantBufferData));
    m_vertexStride = 0;
    m_resetCamera = false;
    m_resetMarbleRotation = false;
    m_currentCheckpoint = 0;
    m_windowActive = false;


    m_lightStrength = 0.0f;
    m_targetLightStrength = 0.0f;
    m_resetCamera = true;
    m_resetMarbleRotation = true;

    // Checkpoints (from start to goal).
    m_checkpoints.push_back(XMFLOAT3(45.7f, -43.6f, -45.0f)); // Start
    m_checkpoints.push_back(XMFLOAT3(120.7f, -35.0f, -45.0f)); // Checkpoint 1
    m_checkpoints.push_back(XMFLOAT3(297.6f, -194.6f, -45.0f)); // Checkpoint 2
    m_checkpoints.push_back(XMFLOAT3(770.1f, -391.5f, -45.0f)); // Checkpoint 3
    m_checkpoints.push_back(XMFLOAT3(552.0f, -148.6f, -45.0f)); // Checkpoint 4
    m_checkpoints.push_back(XMFLOAT3(846.8f, -377.0f, -45.0f)); // Goal

    m_persistentState = ref new PersistentState();
    m_persistentState->Initialize(Windows::Storage::ApplicationData::Current->LocalSettings->Values, "blooDot");

    m_camera = std::unique_ptr<Camera>(new Camera());
    m_sampleOverlay = std::unique_ptr<SampleOverlay>(new SampleOverlay(m_deviceResources, L"Lukas Spiel Test Demo"));

    m_audio.Initialize();
    m_accelerometer = Windows::Devices::Sensors::Accelerometer::GetDefault();

    m_loadScreen = std::unique_ptr<LoadScreen>(new LoadScreen());
    m_loadScreen->Initialize(m_deviceResources);

	m_worldScreen = std::unique_ptr<WorldScreenBase>(new LevelEditor());
	m_worldScreen->Initialize(m_deviceResources);

    UserInterface::GetInstance().Initialize(
        m_deviceResources->GetD2DDevice(),
        m_deviceResources->GetD2DDeviceContext(),
        m_deviceResources->GetWicImagingFactory(),
        m_deviceResources->GetDWriteFactory()
    );

    LoadState();

    CreateWindowSizeDependentResources();

    // TODO: Change timer settings if you want something other than the default variable timestep mode.
    // eg. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */

	// Input
	m_myGamepads = ref new Vector<Gamepad^>();

	for (auto gamepad : Gamepad::Gamepads)
	{
		m_myGamepads->Append(gamepad);
	}

	Gamepad::GamepadAdded += ref new EventHandler<Gamepad^>([=](Platform::Object^, Gamepad^ args)
	{
		m_myGamepads->Append(args);
		m_currentGamepadNeedsRefresh = true;
	});

	Gamepad::GamepadRemoved += ref new EventHandler<Gamepad ^>([=](Platform::Object^, Gamepad^ args)
	{
		unsigned int indexRemoved;

		if (m_myGamepads->IndexOf(args, &indexRemoved))
		{
			m_myGamepads->RemoveAt(indexRemoved);
			m_currentGamepadNeedsRefresh = true;
		}
	});

	m_gamepad = GetLastGamepad();
	m_currentGamepadNeedsRefresh = false;
}

blooDotMain::~blooDotMain()
{
    // Deregister device notification
    m_deviceResources->RegisterDeviceNotify(nullptr);
}

// Updates application state when the window size changes (e.g. device orientation change)
void blooDotMain::CreateWindowSizeDependentResources()
{
    // update the 3D projection matrix
    m_camera->SetProjectionParameters(
        40.0f,                                                  // use a 70-degree vertical field of view
        m_deviceResources->GetOutputSize().Width / m_deviceResources->GetOutputSize().Height,  // specify the aspect ratio of the window
        50.0f,                                                  // specify the nearest Z-distance at which to draw vertices
        500.0f                                                  // specify the farthest Z-distance at which to draw vertice
        );

    // game specific
    XMFLOAT4X4 projection;
    m_camera->GetProjectionMatrix(&projection);

    XMStoreFloat4x4(&projection, XMMatrixMultiply(XMMatrixTranspose(XMLoadFloat4x4(&m_deviceResources->GetOrientationTransform3D())), XMLoadFloat4x4(&projection)));

    m_mazeConstantBufferData.projection = projection;
    m_marbleConstantBufferData.projection = projection;

    // user interface
    const float padding = 32.0f;
    D2D1_RECT_F clientRect = ConvertRect(m_deviceResources->GetLogicalSize());
    InflateRect(clientRect, -padding, -padding);

    D2D1_RECT_F topHalfRect = clientRect;
    topHalfRect.bottom = ((clientRect.top + clientRect.bottom) / 2.0f) - (padding / 2.0f);
    D2D1_RECT_F bottomHalfRect = clientRect;
    bottomHalfRect.top = topHalfRect.bottom + padding;

    m_startGameButton.Initialize();
    m_startGameButton.SetAlignment(AlignCenter, AlignFar);
    m_startGameButton.SetContainer(topHalfRect);
    m_startGameButton.SetText(L"Start Game");
    m_startGameButton.SetTextColor(D2D1::ColorF(D2D1::ColorF::White));
    m_startGameButton.GetTextStyle().SetFontWeight(DWRITE_FONT_WEIGHT_BLACK);
    m_startGameButton.SetPadding(D2D1::SizeF(32.0f, 16.0f));
    m_startGameButton.GetTextStyle().SetFontSize(72.0f);
    UserInterface::GetInstance().RegisterElement(&m_startGameButton);

    m_highScoreButton.Initialize();
    m_highScoreButton.SetAlignment(AlignCenter, AlignNear);
    m_highScoreButton.SetContainer(bottomHalfRect);
    m_highScoreButton.SetText(L"High Scores");
    m_highScoreButton.SetTextColor(D2D1::ColorF(D2D1::ColorF::White));
    m_highScoreButton.GetTextStyle().SetFontWeight(DWRITE_FONT_WEIGHT_BLACK);
    m_highScoreButton.SetPadding(D2D1::SizeF(32.0f, 16.0f));
    m_highScoreButton.GetTextStyle().SetFontSize(72.0f);
    UserInterface::GetInstance().RegisterElement(&m_highScoreButton);

    m_highScoreTable.Initialize();
    m_highScoreTable.SetAlignment(AlignCenter, AlignCenter);
    m_highScoreTable.SetContainer(clientRect);
    m_highScoreTable.SetTextColor(D2D1::ColorF(D2D1::ColorF::White));
    m_highScoreTable.GetTextStyle().SetFontWeight(DWRITE_FONT_WEIGHT_BOLD);
    m_highScoreTable.GetTextStyle().SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    m_highScoreTable.GetTextStyle().SetFontSize(60.0f);
    UserInterface::GetInstance().RegisterElement(&m_highScoreTable);

    m_preGameCountdownTimer.Initialize();
    m_preGameCountdownTimer.SetAlignment(AlignCenter, AlignCenter);
    m_preGameCountdownTimer.SetContainer(clientRect);
    m_preGameCountdownTimer.SetTextColor(D2D1::ColorF(D2D1::ColorF::White));
    m_preGameCountdownTimer.GetTextStyle().SetFontWeight(DWRITE_FONT_WEIGHT_BLACK);
    m_preGameCountdownTimer.GetTextStyle().SetFontSize(144.0f);
    UserInterface::GetInstance().RegisterElement(&m_preGameCountdownTimer);

	m_nerdStatsDisplay.Initialize();
	m_nerdStatsDisplay.SetAlignment(AlignType::AlignNear, AlignType::AlignNear);
	m_nerdStatsDisplay.SetContainer(clientRect);
	m_nerdStatsDisplay.SetTextColor(D2D1::ColorF(D2D1::ColorF::LightGoldenrodYellow));
	m_nerdStatsDisplay.GetTextStyle().SetFontWeight(DWRITE_FONT_WEIGHT_DEMI_BOLD);
	m_nerdStatsDisplay.GetTextStyle().SetFontSize(24.0f);
	UserInterface::GetInstance().RegisterElement(&m_nerdStatsDisplay);

    m_inGameStopwatchTimer.Initialize();
    m_inGameStopwatchTimer.SetAlignment(AlignNear, AlignFar);
    m_inGameStopwatchTimer.SetContainer(clientRect);
    m_inGameStopwatchTimer.SetTextColor(D2D1::ColorF(D2D1::ColorF::White, 0.75f));
    m_inGameStopwatchTimer.GetTextStyle().SetFontWeight(DWRITE_FONT_WEIGHT_BLACK);
    m_inGameStopwatchTimer.GetTextStyle().SetFontSize(96.0f);
    UserInterface::GetInstance().RegisterElement(&m_inGameStopwatchTimer);

    m_checkpointText.Initialize();
    m_checkpointText.SetAlignment(AlignCenter, AlignCenter);
    m_checkpointText.SetContainer(clientRect);
    m_checkpointText.SetText(L"Checkpoint!");
    m_checkpointText.SetTextColor(D2D1::ColorF(D2D1::ColorF::White));
    m_checkpointText.GetTextStyle().SetFontWeight(DWRITE_FONT_WEIGHT_BLACK);
    m_checkpointText.GetTextStyle().SetFontSize(72.0f);
    UserInterface::GetInstance().RegisterElement(&m_checkpointText);

    m_pausedText.Initialize();
    m_pausedText.SetAlignment(AlignCenter, AlignCenter);
    m_pausedText.SetContainer(clientRect);
    m_pausedText.SetText(L"Paused");
    m_pausedText.SetTextColor(D2D1::ColorF(D2D1::ColorF::White));
    m_pausedText.GetTextStyle().SetFontWeight(DWRITE_FONT_WEIGHT_BLACK);
    m_pausedText.GetTextStyle().SetFontSize(72.0f);
    UserInterface::GetInstance().RegisterElement(&m_pausedText);

    m_resultsText.Initialize();
    m_resultsText.SetAlignment(AlignCenter, AlignCenter);
    m_resultsText.SetContainer(clientRect);
    m_resultsText.SetTextColor(D2D1::ColorF(D2D1::ColorF::White));
    m_resultsText.GetTextStyle().SetFontWeight(DWRITE_FONT_WEIGHT_BOLD);
    m_resultsText.GetTextStyle().SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    m_resultsText.GetTextStyle().SetFontSize(36.0f);
    UserInterface::GetInstance().RegisterElement(&m_resultsText);

    if ((!m_deferredResourcesReady) && m_loadScreen != nullptr)
    {
        m_loadScreen->UpdateForWindowSizeChange();
	}

	if (m_worldScreen != nullptr)
	{
		m_worldScreen->CreateDeviceDependentResources();
	}

	if (m_sampleOverlay != nullptr)
	{
		m_sampleOverlay->CreateWindowSizeDependentResources();
	}
}

void blooDotMain::LoadDeferredResources(bool delay, bool deviceOnly)
{
    DX::StepTimer loadingTimer;

    BasicLoader^ loader = ref new BasicLoader(m_deviceResources->GetD3DDevice());

    D3D11_INPUT_ELEMENT_DESC layoutDesc [] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    m_vertexStride = 44; // must set this to match the size of layoutDesc above

    Platform::String^ vertexShaderName = L"BasicVertexShader.cso";
    loader->LoadShader(
        vertexShaderName,
        layoutDesc,
        ARRAYSIZE(layoutDesc),
        &m_vertexShader,
        &m_inputLayout
    );
    
	DX::ThrowIfFailed(m_vertexShader->SetPrivateData(WKPDID_D3DDebugObjectName, vertexShaderName->Length(), vertexShaderName->Data()));

	

    // create the constant buffer for updating model and camera data.
    D3D11_BUFFER_DESC constantBufferDesc = { 0 };
    constantBufferDesc.ByteWidth = ((sizeof(ConstantBuffer) + 15) / 16) * 16; // multiple of 16 bytes
    constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.CPUAccessFlags = 0;
    constantBufferDesc.MiscFlags = 0;
    // this will not be used as a structured buffer, so this parameter is ignored
    constantBufferDesc.StructureByteStride = 0;

    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateBuffer(
            &constantBufferDesc,
            nullptr,             // leave the buffer uninitialized
            &m_constantBuffer
        )
    );

    Platform::String^ constantBufferName = "Constant Buffer created in LoadDeferredResources";
    DX::ThrowIfFailed(m_constantBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, constantBufferName->Length(), constantBufferName->Data()));

    Platform::String^ pixelShaderName = "BasicPixelShader.cso";
    loader->LoadShader(
        pixelShaderName,
        &m_pixelShader
        );
    DX::ThrowIfFailed(m_pixelShader->SetPrivateData(WKPDID_D3DDebugObjectName, pixelShaderName->Length(), pixelShaderName->Data()));

    // create the blend state.
    D3D11_BLEND_DESC blendDesc = { 0 };
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateBlendState(
            &blendDesc,
            &m_blendState
            )
        );
    Platform::String^ blendStateName = "Blend State created in LoadDeferredResources";
    DX::ThrowIfFailed(m_blendState->SetPrivateData(WKPDID_D3DDebugObjectName, blendStateName->Length(), blendStateName->Data()));

    // create the sampler
    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = m_deviceResources->GetDeviceFeatureLevel() > D3D_FEATURE_LEVEL_9_1 ? 4 : 2;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.BorderColor[0] = 0.0f;
    samplerDesc.BorderColor[1] = 0.0f;
    samplerDesc.BorderColor[2] = 0.0f;
    samplerDesc.BorderColor[3] = 0.0f;
    // allow use of all mip levels
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateSamplerState(
            &samplerDesc,
            &m_sampler
        )
    );

    Platform::String^ samplerName = "Sampler created in LoadDeferredResources";
    DX::ThrowIfFailed(m_sampler->SetPrivateData(WKPDID_D3DDebugObjectName, samplerName->Length(), samplerName->Data()));

    if (!deviceOnly)
    {
        // When handling device lost, we only need to recreate the graphics-device related
        // resources. All other delayed resources that only need to be created on app
        // startup go here.

        m_audio.CreateResources();
    }

    if (delay)
    {
        while (loadingTimer.GetTotalSeconds() < 3.5)
        {
            // game doesn't take long to load resources,
            // so we're simulating a longer load time to demonstrate
            // a more real world example
            loadingTimer.Tick([&]()
            {
                //do nothing, just wait
            }
            );
        }

		m_deferredResourcesReadyPending = true;
    }
}

// Renders the current frame according to the current application state.
// Returns true if the frame was rendered and is ready to be displayed.
bool blooDotMain::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return false;
    }

    auto context = m_deviceResources->GetD3DDeviceContext();

    // Reset the viewport to target the whole screen.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    // Reset render targets to the screen.
    ID3D11RenderTargetView *const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
    context->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());

    // Clear the back buffer and depth stencil view.
    context->ClearRenderTargetView(m_deviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::Black);
    context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	if (m_gameState == GameState::LoadScreen)
	{
		// Only render the loading screen for now.
		m_deviceResources->GetD3DDeviceContext()->BeginEventInt(L"Render Loading Screen", 0);
		m_loadScreen->Render(m_deviceResources->GetOrientationTransform2D(), m_pointerPosition);
		m_deviceResources->GetD3DDeviceContext()->EndEvent();
	}
	else if (m_gameState == GameState::LevelEditor)
	{
		m_deviceResources->GetD3DDeviceContext()->BeginEventInt(L"Render Level Editor", 0);
		m_worldScreen->Render(m_deviceResources->GetOrientationTransform2D(), m_pointerPosition);
		m_deviceResources->GetD3DDeviceContext()->EndEvent();
	}
	else if (m_deferredResourcesReady)
	{
		m_deviceResources->GetD3DDeviceContext()->IASetInputLayout(m_inputLayout.Get());

		FLOAT blendFactors[4] = { 0, };
		m_deviceResources->GetD3DDeviceContext()->OMSetBlendState(m_blendState.Get(), blendFactors, 0xffffffff);

		// Set the vertex shader stage state.
		m_deviceResources->GetD3DDeviceContext()->VSSetShader(
			m_vertexShader.Get(),   // use this vertex shader
			nullptr,                // don't use shader linkage
			0                       // don't use shader linkage
		);

		// Set the pixel shader stage state.
		m_deviceResources->GetD3DDeviceContext()->PSSetShader(
			m_pixelShader.Get(),    // use this pixel shader
			nullptr,                // don't use shader linkage
			0                       // don't use shader linkage
		);

		m_deviceResources->GetD3DDeviceContext()->PSSetSamplers(
			0,                       // starting at the first sampler slot
			1,                       // set one sampler binding
			m_sampler.GetAddressOf() // to use this sampler
		);

#pragma region Rendering Maze

		m_deviceResources->GetD3DDeviceContext()->BeginEventInt(L"Render Maze", 0);
		// Update the constant buffer with the new data.
		m_deviceResources->GetD3DDeviceContext()->UpdateSubresource(
			m_constantBuffer.Get(),
			0,
			nullptr,
			&m_mazeConstantBufferData,
			0,
			0
		);

		m_deviceResources->GetD3DDeviceContext()->VSSetConstantBuffers(
			0,                // starting at the first constant buffer slot
			1,                // set one constant buffer binding
			m_constantBuffer.GetAddressOf() // to use this buffer
		);

		m_deviceResources->GetD3DDeviceContext()->PSSetConstantBuffers(
			0,                // starting at the first constant buffer slot
			1,                // set one constant buffer binding
			m_constantBuffer.GetAddressOf() // to use this buffer
		);

		m_mazeMesh.Render(m_deviceResources->GetD3DDeviceContext(), 0, INVALID_SAMPLER_SLOT, INVALID_SAMPLER_SLOT);
		m_deviceResources->GetD3DDeviceContext()->EndEvent();

#pragma endregion

#pragma region Rendering Marble

		m_deviceResources->GetD3DDeviceContext()->BeginEventInt(L"Render Marble", 0);

		// update the constant buffer with the new data
		m_deviceResources->GetD3DDeviceContext()->UpdateSubresource(
			m_constantBuffer.Get(),
			0,
			nullptr,
			&m_marbleConstantBufferData,
			0,
			0
		);

		m_deviceResources->GetD3DDeviceContext()->VSSetConstantBuffers(
			0,                // starting at the first constant buffer slot
			1,                // set one constant buffer binding
			m_constantBuffer.GetAddressOf() // to use this buffer
		);

		m_deviceResources->GetD3DDeviceContext()->PSSetConstantBuffers(
			0,                // starting at the first constant buffer slot
			1,                // set one constant buffer binding
			m_constantBuffer.GetAddressOf() // to use this buffer
		);

		m_marbleMesh.Render(m_deviceResources->GetD3DDeviceContext(), 0, INVALID_SAMPLER_SLOT, INVALID_SAMPLER_SLOT);
		m_deviceResources->GetD3DDeviceContext()->EndEvent();

#pragma endregion
	}

	if (m_audio.m_isAudioStarted)
	{
		m_audio.Render();
	}

    // Draw the user interface and the overlay.
    UserInterface::GetInstance().Render(m_deviceResources->GetOrientationTransform2D());

    //m_deviceResources->GetD3DDeviceContext()->BeginEventInt(L"Render Overlay", 0);
    //m_sampleOverlay->Render();
    //m_deviceResources->GetD3DDeviceContext()->EndEvent();

    return true;
}

void blooDotMain::SetGameState(GameState nextState)
{
    // previous state cleanup
    switch (m_gameState)
    {
    case GameState::MainMenu:
        m_startGameButton.SetVisible(false);
        m_highScoreButton.SetVisible(false);
        break;

    case GameState::HighScoreDisplay:
        m_highScoreTable.SetVisible(false);
        break;

    case GameState::PreGameCountdown:
        m_inGameStopwatchTimer.Reset();
        m_inGameStopwatchTimer.SetVisible(true);
        break;

    case GameState::PostGameResults:
        m_resultsText.SetVisible(false);
        break;
    }

    // next state setup
    switch (nextState)
    {
    case GameState::MainMenu:
        m_startGameButton.SetVisible(true);
        m_startGameButton.SetSelected(true);
        m_highScoreButton.SetVisible(true);
        m_highScoreButton.SetSelected(false);
        m_pausedText.SetVisible(false);

        m_resetCamera = true;
        m_resetMarbleRotation = true;
        m_physics.SetPosition(XMFLOAT3(305, -210, -43));
        m_targetLightStrength = 0.6f;
        break;

    case GameState::HighScoreDisplay:
        m_highScoreTable.SetVisible(true);
        break;

    case GameState::PreGameCountdown:
        m_inGameStopwatchTimer.SetVisible(false);
        m_preGameCountdownTimer.SetVisible(true);
        m_preGameCountdownTimer.StartCountdown(3);

        ResetCheckpoints();
        m_resetCamera = true;
        m_resetMarbleRotation = true;
        m_physics.SetPosition(m_checkpoints[0]);
        m_physics.SetVelocity(XMFLOAT3(0, 0, 0));
        m_targetLightStrength = 1.0f;
        break;

    case GameState::InGameActive:
        m_pausedText.SetVisible(false);
        m_inGameStopwatchTimer.Start();
        m_targetLightStrength = 1.0f;
        break;

    case GameState::InGamePaused:
        m_pausedText.SetVisible(true);
        m_inGameStopwatchTimer.Stop();
        m_targetLightStrength = 0.6f;
        break;

    case GameState::PostGameResults:
        m_inGameStopwatchTimer.Stop();
        m_inGameStopwatchTimer.SetVisible(false);
        m_resultsText.SetVisible(true);
        {
            WCHAR formattedTime[32];
            m_inGameStopwatchTimer.GetFormattedTime(formattedTime, m_newHighScore.elapsedTime);
            WCHAR buffer[64];
            swprintf_s(
                buffer,
                L"%s\nYour time: %s",
                (m_newHighScore.wasJustAdded ? L"New High Score!" : L"Finished!"),
                formattedTime
                );
            m_resultsText.SetText(buffer);
            m_resultsText.SetVisible(true);
        }
        m_targetLightStrength = 0.6f;
        break;
    }

    m_gameState = nextState;
}

void blooDotMain::ResetCheckpoints()
{
    m_currentCheckpoint = 0;
}

CheckpointState blooDotMain::UpdateCheckpoints()
{
    if (m_currentCheckpoint >= (m_checkpoints.size() - 1))
        return CheckpointState::None;

    const float checkpointRadius = 20.0f;
    float radius = m_physics.GetRadius();
    float horizDistSq = (radius + checkpointRadius) * (radius + checkpointRadius);
    XMVECTOR horizDistSqLimit = XMVectorSet(horizDistSq, horizDistSq, horizDistSq, horizDistSq);
    float vertDistSq = radius * radius;
    XMVECTOR vertDistSqLimit = XMVectorSet(vertDistSq, vertDistSq, vertDistSq, vertDistSq);

    XMVECTOR position = XMLoadFloat3(&m_physics.GetPosition());
    XMVECTOR up = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);

    for (size_t i = m_currentCheckpoint + 1; i < m_checkpoints.size(); ++i)
    {
        XMVECTOR checkpointPos = XMLoadFloat3(&m_checkpoints[i]);
        XMVECTOR posCheckSpace = position - checkpointPos;
        XMVECTOR posVertical = up * XMVector3Dot(up, posCheckSpace);
        XMVECTOR posHorizontal = posCheckSpace - posVertical;
        XMVECTOR vHorizDistSq = XMVector3LengthSq(posHorizontal);
        XMVECTOR vVertDistSq = XMVector3LengthSq(posVertical);

        XMVECTOR check = XMVectorAndInt(
            XMVectorLessOrEqual(vHorizDistSq, horizDistSqLimit),
            XMVectorLessOrEqual(vVertDistSq, vertDistSqLimit)
            );
        if (XMVector3EqualInt(check, XMVectorTrueInt()))
        {
            m_currentCheckpoint = i;

            if (i == (m_checkpoints.size() - 1))
                return CheckpointState::Goal;
            else
                return CheckpointState::Save;
        }
    }

    return CheckpointState::None;
}

// Updates the application state once per frame.
void blooDotMain::Update()
{
	float timerTotal;
	float timerElapsed;

	if (m_gameState == GameState::Initial)
	{
		SetGameState(GameState::LoadScreen);
	}

	// Update scene objects.
    m_timer.Tick([&]()
    {
		timerTotal = static_cast<float>(m_timer.GetTotalSeconds());
		timerElapsed = static_cast<float>(m_timer.GetElapsedSeconds());

		/* compute FPS for for Lukas */
		ComputeFPS(timerElapsed);
		if (m_timer.GetFrameCount() % 100 == 0)
		{
			m_nerdStatsDisplay.UpdateFPS(QueryFPS());
		}

		UserInterface::GetInstance().Update(timerTotal, timerElapsed);

		// When the game is first loaded, we display a load screen
        // and load any deferred resources that might be too expensive
        // to load during initialization.
        if (m_gameState==GameState::LoadScreen)
        {			
            // At this point we can draw a progress bar, or if we had
            // loaded audio, we could play audio during the loading process.
			m_loadScreen->Update(timerTotal, timerElapsed);
			return;
        }
		else if (m_gameState == GameState::LevelEditor)
		{
			if (!m_audio.m_isAudioStarted)
			{
				m_audio.Start();
			}

			m_worldScreen->Update(timerTotal, timerElapsed);
			return;
		}

        

        switch (m_gameState)
        {
			case GameState::PreGameCountdown:
				if (m_preGameCountdownTimer.IsCountdownComplete())
				{
					SetGameState(GameState::InGameActive);
				}
				break;
        }

#pragma region Process Input

        float combinedTiltX = 0.0f;
        float combinedTiltY = 0.0f;

        // Check whether the user paused or resumed the game.
        if (ButtonJustPressed(GamepadButtons::Menu) || m_pauseKeyPressed)
        {
            m_pauseKeyPressed = false;

			if (m_gameState == GameState::InGameActive)
			{
				SetGameState(GameState::InGamePaused);
			}  
			else if (m_gameState == GameState::InGamePaused)
			{
				SetGameState(GameState::InGameActive);
			}
        }

        // Check whether the user restarted the game or cleared the high score table.
        if (ButtonJustPressed(GamepadButtons::View) || m_homeKeyPressed)
        {
            m_homeKeyPressed = false;

            if (m_gameState == GameState::InGameActive ||
                m_gameState == GameState::InGamePaused ||
                m_gameState == GameState::PreGameCountdown)
            {
                SetGameState(GameState::MainMenu);
                m_inGameStopwatchTimer.SetVisible(false);
                m_preGameCountdownTimer.SetVisible(false);
            }
            else if (m_gameState == GameState::HighScoreDisplay)
            {
                m_highScoreTable.Reset();
            }
        }

        // Check whether the user chose a button from the UI.
        bool anyPoints = !m_pointQueue.empty();

        while (!m_pointQueue.empty())
        {
            UserInterface::GetInstance().HitTest(m_pointQueue.front());
            m_pointQueue.pop();
        }

        // Handle menu navigation.
        bool chooseSelection = (ButtonJustPressed(GamepadButtons::A) || ButtonJustPressed(GamepadButtons::Menu));
		bool moveUp = ButtonJustPressed(GamepadButtons::DPadUp);
		bool moveDown = ButtonJustPressed(GamepadButtons::DPadDown);

        switch (m_gameState)
        {
        case GameState::MainMenu:
            if (chooseSelection)
            {
                m_audio.PlaySoundEffect(MenuSelectedEvent);
				if (m_startGameButton.GetSelected())
				{
					m_startGameButton.SetPressed(true);
				}
				if (m_highScoreButton.GetSelected())
				{
					m_highScoreButton.SetPressed(true);
				}
            }
            if (moveUp || moveDown)
            {
                m_startGameButton.SetSelected(!m_startGameButton.GetSelected());
                m_highScoreButton.SetSelected(!m_startGameButton.GetSelected());
                m_audio.PlaySoundEffect(MenuChangeEvent);
            }
            break;

        case GameState::HighScoreDisplay:
			if (chooseSelection || anyPoints)
			{
				SetGameState(GameState::MainMenu);
			}
            break;

        case GameState::PostGameResults:
			if (chooseSelection || anyPoints)
			{
				SetGameState(GameState::HighScoreDisplay);
			}
            break;

        case GameState::InGamePaused:
            if (m_pausedText.IsPressed())
            {
                m_pausedText.SetPressed(false);
                SetGameState(GameState::InGameActive);
            }
            break;
        }

        // Update the game state if the user chose a menu option.
        if (m_startGameButton.IsPressed())
        {
            SetGameState(GameState::PreGameCountdown);
            m_startGameButton.SetPressed(false);
        }

        if (m_highScoreButton.IsPressed())
        {
            SetGameState(GameState::HighScoreDisplay);
            m_highScoreButton.SetPressed(false);
        }

		// Process controller input.
#if WINAPI_FAMILY != WINAPI_FAMILY_PHONE_APP // Only process controller input when the device is not a phone.

		if (m_currentGamepadNeedsRefresh)
		{
			auto mostRecentGamepad = GetLastGamepad();

			if (m_gamepad != mostRecentGamepad)
			{
				m_gamepad = mostRecentGamepad;
			}

			m_currentGamepadNeedsRefresh = false;
		}

		if (m_gamepad != nullptr)
		{
			m_oldReading = m_newReading;
			m_newReading = m_gamepad->GetCurrentReading();
		}

		float leftStickX = static_cast<float>(m_newReading.LeftThumbstickX);
		float leftStickY = static_cast<float>(m_newReading.LeftThumbstickY);

		auto oppositeSquared = leftStickY * leftStickY;
		auto adjacentSquared = leftStickX * leftStickX;

		if ((oppositeSquared + adjacentSquared) > m_deadzoneSquared)
		{
			combinedTiltX += leftStickX * m_controllerScaleFactor;
			combinedTiltY += leftStickY * m_controllerScaleFactor;
		}

#endif

        // Account for touch input.
        for (TouchMap::const_iterator iter = m_touches.cbegin(); iter != m_touches.cend(); ++iter)
        {
            combinedTiltX += iter->second.x * m_touchScaleFactor;
            combinedTiltY += iter->second.y * m_touchScaleFactor;
        }

        // Account for sensors.
        if (m_accelerometer != nullptr)
        {
            Windows::Devices::Sensors::AccelerometerReading^ reading =
                m_accelerometer->GetCurrentReading();

            if (reading != nullptr)
            {
                combinedTiltX += static_cast<float>(reading->AccelerationX) * m_accelerometerScaleFactor;
                combinedTiltY += static_cast<float>(reading->AccelerationY) * m_accelerometerScaleFactor;
            }
        }

        // Clamp input.
        combinedTiltX = max(-1, min(1, combinedTiltX));
        combinedTiltY = max(-1, min(1, combinedTiltY));

        if (m_gameState != GameState::PreGameCountdown &&
            m_gameState != GameState::InGameActive &&
            m_gameState != GameState::InGamePaused)
        {
            // Ignore tilt when the menu is active.
            combinedTiltX = 0.0f;
            combinedTiltY = 0.0f;
        }

#pragma endregion

#pragma region Physics

        const float maxTilt = 1.0f / 8.0f;
        XMVECTOR gravity = XMVectorSet(combinedTiltX * maxTilt, combinedTiltY * maxTilt, 1.0f, 0.0f);
        gravity = XMVector3Normalize(gravity);

        XMFLOAT3A g;
        XMStoreFloat3(&g, gravity);
        m_physics.SetGravity(g);

        if (m_gameState == GameState::InGameActive)
        {
            // Only update physics when gameplay is active.
            m_physics.UpdatePhysicsSimulation(static_cast<float>(m_timer.GetElapsedSeconds()));

            // Handle checkpoints.
            switch (UpdateCheckpoints())
            {
            case CheckpointState::Save:
                // Display checkpoint notice.
                m_checkpointText.SetVisible(true);
                m_checkpointText.SetTextOpacity(1.0f);
                m_checkpointText.FadeOut(2.0f);
                m_audio.PlaySoundEffect(CheckpointEvent);
                SaveState();
                break;

            case CheckpointState::Goal:
                m_inGameStopwatchTimer.Stop();
                m_newHighScore.elapsedTime = m_inGameStopwatchTimer.GetElapsedTime();
                SYSTEMTIME systemTime;
                GetLocalTime(&systemTime);
                WCHAR buffer[64];
                swprintf_s(buffer, L"%d/%d/%d", systemTime.wYear, systemTime.wMonth, systemTime.wDay);
                m_newHighScore.tag = ref new Platform::String(buffer);
                m_highScoreTable.AddScoreToTable(m_newHighScore);

                m_audio.PlaySoundEffect(CheckpointEvent);
                m_audio.StopSoundEffect(RollingEvent);

                // Display game results.
                SetGameState(GameState::PostGameResults);
                SaveState();
                break;
            }
        }

        XMFLOAT3A marblePosition;
        memcpy(&marblePosition, &m_physics.GetPosition(), sizeof(XMFLOAT3));
        static XMFLOAT3A oldMarblePosition = marblePosition;

        const XMMATRIX initialMarbleRotationMatrix = XMMatrixMultiply( XMMatrixRotationY(90.0f * (XM_PI / 180.0f)) , XMMatrixRotationX(90.0f * (XM_PI / 180.0f)));

        static XMMATRIX marbleRotationMatrix = initialMarbleRotationMatrix;

        // Check whether the marble fell off of the maze.
        const float fadeOutDepth = 0.0f;
        const float resetDepth = 80.0f;
        if (marblePosition.z >= fadeOutDepth)
        {
            m_targetLightStrength = 0.0f;
        }
        if (marblePosition.z >= resetDepth)
        {
            // Reset marble.
            memcpy(&marblePosition, &m_checkpoints[m_currentCheckpoint], sizeof(XMFLOAT3));
            oldMarblePosition = marblePosition;
            m_physics.SetPosition((const XMFLOAT3&) marblePosition);
            m_physics.SetVelocity(XMFLOAT3(0, 0, 0));
            m_lightStrength = 0.0f;
            m_targetLightStrength = 1.0f;

            m_resetCamera = true;
            m_resetMarbleRotation = true;
            m_audio.PlaySoundEffect(FallingEvent);
        }

        XMFLOAT3A marbleRotation;
        XMStoreFloat3A(&marbleRotation, (XMLoadFloat3A(&oldMarblePosition) - XMLoadFloat3A(&marblePosition)) / m_physics.GetRadius());
        oldMarblePosition = marblePosition;

        if (m_resetMarbleRotation)
        {
            marbleRotationMatrix = initialMarbleRotationMatrix;
            m_resetMarbleRotation = false;
        }
        else
        {
            marbleRotationMatrix = XMMatrixMultiply(marbleRotationMatrix, XMMatrixRotationY(marbleRotation.x /** 180.0f / 3.1415926535f*/));
            marbleRotationMatrix = XMMatrixMultiply(marbleRotationMatrix, XMMatrixRotationX(-marbleRotation.y /** -180.0f / 3.1415926535f*/));
        }

#pragma endregion

#pragma region Update Camera

        static float eyeDistance = 200.0f;
        static XMFLOAT3A eyePosition = XMFLOAT3A(0, 0, 0);

        // Gradually move the camera above the marble.
        XMFLOAT3A targetEyePosition;
        XMStoreFloat3A(&targetEyePosition, XMLoadFloat3A(&marblePosition) - (XMLoadFloat3A(&g) * eyeDistance));

        if (m_resetCamera)
        {
            eyePosition = targetEyePosition;
            m_resetCamera = false;
        }
        else
        {
            XMStoreFloat3A(&eyePosition, XMLoadFloat3A(&eyePosition) + ((XMLoadFloat3A(&targetEyePosition) - XMLoadFloat3A(&eyePosition)) * min(1, static_cast<float>(m_timer.GetElapsedSeconds()) * 8)));
        }

        // Look at the marble.
        if ((m_gameState == GameState::MainMenu) || (m_gameState == GameState::HighScoreDisplay))
        {
            // Override camera position for menus.
            XMStoreFloat3A(&eyePosition, XMLoadFloat3A(&marblePosition) + XMVectorSet(75.0f, -150.0f, -75.0f, 0.0f));
            m_camera->SetViewParameters(eyePosition, marblePosition, XMFLOAT3(0.0f, 0.0f, -1.0f));
        }
        else
        {
            m_camera->SetViewParameters(eyePosition, marblePosition, XMFLOAT3(0.0f, 1.0f, 0.0f));
        }

#pragma endregion

#pragma region Update Constant Buffers

        // Update the model matrices based on the simulation.
        XMStoreFloat4x4(&m_mazeConstantBufferData.model, XMMatrixIdentity());
        XMStoreFloat4x4(&m_marbleConstantBufferData.model, XMMatrixTranspose(XMMatrixMultiply(marbleRotationMatrix, XMMatrixTranslationFromVector(XMLoadFloat3A(&marblePosition)))));

        // Update the view matrix based on the camera.
        XMFLOAT4X4 view;
        m_camera->GetViewMatrix(&view);
        m_mazeConstantBufferData.view = view;
        m_marbleConstantBufferData.view = view;

        // Update lighting constants.
        m_lightStrength += (m_targetLightStrength - m_lightStrength) * min(1, static_cast<float>(m_timer.GetElapsedSeconds()) * 4);

        m_mazeConstantBufferData.marblePosition = marblePosition;
        m_mazeConstantBufferData.marbleRadius = m_physics.GetRadius();
        m_mazeConstantBufferData.lightStrength = m_lightStrength;
        m_marbleConstantBufferData.marblePosition = marblePosition;
        m_marbleConstantBufferData.marbleRadius = m_physics.GetRadius();
        m_marbleConstantBufferData.lightStrength = m_lightStrength;

#pragma endregion

#pragma region Update Audio

        if (!m_audio.HasEngineExperiencedCriticalError())
        {
            if (m_gameState == GameState::InGameActive)
            {
                float wallDistances[8];
                int returnedCount = m_physics.GetRoomDimensions(wallDistances, ARRAYSIZE(wallDistances));
                assert(returnedCount == ARRAYSIZE(wallDistances));
                m_audio.SetRoomSize(m_physics.GetRoomSize(), wallDistances);
                CollisionInfo ci = m_physics.GetCollisionInfo();

                // Calculate roll sound, and pitch according to velocity.
                XMFLOAT3 velocity = m_physics.GetVelocity();
                XMFLOAT3 position = m_physics.GetPosition();
                float volumeX = abs(velocity.x) / 200;
                if (volumeX > 1.0) volumeX = 1.0;
                if (volumeX < 0.0) volumeX = 0.0;
                float volumeY = abs(velocity.y) / 200;
                if (volumeY > 1.0) volumeY = 1.0;
                if (volumeY < 0.0) volumeY = 0.0;
                float volume = max(volumeX, volumeY);

                // Pitch of the rolling sound ranges from .85 to 1.05f,
                // increasing logarithmically.
                float pitch = .85f + (volume * volume / 5.0f);

                // Play the roll sound only if the marble is actually rolling.
                if (ci.isRollingOnFloor && volume > 0)
                {
                    if (!m_audio.IsSoundEffectStarted(RollingEvent))
                    {
                        m_audio.PlaySoundEffect(RollingEvent);
                    }

                    // Update the volume and pitch by the velocity.
                    m_audio.SetSoundEffectVolume(RollingEvent, volume);
                    m_audio.SetSoundEffectPitch(RollingEvent, pitch);

                    // The rolling sound has at most 8000Hz sounds, so we linearly
                    // ramp up the low-pass filter the faster we go.
                    // We also reduce the Q-value of the filter, starting with a
                    // relatively broad cutoff and get progressively tighter.
                    m_audio.SetSoundEffectFilter(
                        RollingEvent,
                        600.0f + 8000.0f * volume,
                        XAUDIO2_MAX_FILTER_ONEOVERQ - volume*volume
                        );
                }
                else
                {
                    m_audio.SetSoundEffectVolume(RollingEvent, 0);
                }

                if (ci.elasticCollision && ci.maxCollisionSpeed > 10)
                {
                    m_audio.PlaySoundEffect(CollisionEvent);

                    float collisionVolume = ci.maxCollisionSpeed / 150.0f;
                    collisionVolume = min(collisionVolume * collisionVolume, 1.0f);
                    m_audio.SetSoundEffectVolume(CollisionEvent, collisionVolume);
                }
            }
            else
            {
                m_audio.SetSoundEffectVolume(RollingEvent, 0);
            }
        }
#pragma endregion
    });
}

void blooDotMain::SaveState()
{
    m_persistentState->SaveXMFLOAT3(":Position", m_physics.GetPosition());
    m_persistentState->SaveXMFLOAT3(":Velocity", m_physics.GetVelocity());
    m_persistentState->SaveSingle(":ElapsedTime", m_inGameStopwatchTimer.GetElapsedTime());

    m_persistentState->SaveInt32(":GameState", static_cast<int>(m_gameState));
    m_persistentState->SaveInt32(":Checkpoint", static_cast<int>(m_currentCheckpoint));

    int i = 0;
    HighScoreEntries entries = m_highScoreTable.GetEntries();
    const int bufferLength = 16;
    char16 str[bufferLength];

    m_persistentState->SaveInt32(":ScoreCount", static_cast<int>(entries.size()));
    for (auto iter = entries.begin(); iter != entries.end(); ++iter)
    {
        int len = swprintf_s(str, bufferLength, L"%d", i++);
        Platform::String^ string = ref new Platform::String(str, len);

        m_persistentState->SaveSingle(Platform::String::Concat(":ScoreTime", string), iter->elapsedTime);
        m_persistentState->SaveString(Platform::String::Concat(":ScoreTag", string), iter->tag);
    }
}

void blooDotMain::LoadState()
{
    XMFLOAT3 position = m_persistentState->LoadXMFLOAT3(":Position", m_physics.GetPosition());
    XMFLOAT3 velocity = m_persistentState->LoadXMFLOAT3(":Velocity", m_physics.GetVelocity());
    float elapsedTime = m_persistentState->LoadSingle(":ElapsedTime", 0.0f);

    int gameState = m_persistentState->LoadInt32(":GameState", static_cast<int>(m_gameState));
    int currentCheckpoint = m_persistentState->LoadInt32(":Checkpoint", static_cast<int>(m_currentCheckpoint));

    switch (static_cast<GameState>(gameState))
    {
    case GameState::Initial:
        break;

    case GameState::MainMenu:
    case GameState::HighScoreDisplay:
    case GameState::PreGameCountdown:
    case GameState::PostGameResults:
        SetGameState(GameState::LoadScreen);
        break;

    case GameState::InGameActive:
    case GameState::InGamePaused:
        m_inGameStopwatchTimer.SetVisible(true);
        m_inGameStopwatchTimer.SetElapsedTime(elapsedTime);
        m_physics.SetPosition(position);
        m_physics.SetVelocity(velocity);
        m_currentCheckpoint = currentCheckpoint;
        SetGameState(GameState::InGamePaused);
        break;
    }

    int count = m_persistentState->LoadInt32(":ScoreCount", 0);

    const int bufferLength = 16;
    char16 str[bufferLength];

    for (int i = 0; i < count; i++)
    {
        HighScoreEntry entry;
        int len = swprintf_s(str, bufferLength, L"%d", i);
        Platform::String^ string = ref new Platform::String(str, len);

        entry.elapsedTime = m_persistentState->LoadSingle(Platform::String::Concat(":ScoreTime", string), 0.0f);
        entry.tag = m_persistentState->LoadString(Platform::String::Concat(":ScoreTag", string), L"");
        m_highScoreTable.AddScoreToTable(entry);
    }
}

inline XMFLOAT2 PointToTouch(Windows::Foundation::Point point, Windows::Foundation::Size bounds)
{
    float touchRadius = min(bounds.Width, bounds.Height);
    float dx = (point.X - (bounds.Width / 2.0f)) / touchRadius;
    float dy = ((bounds.Height / 2.0f) - point.Y) / touchRadius;

    return XMFLOAT2(dx, dy);
}

void blooDotMain::AddTouch(int id, Windows::Foundation::Point point)
{
    m_touches[id] = PointToTouch(point, m_deviceResources->GetLogicalSize());

    m_pointQueue.push(D2D1::Point2F(point.X, point.Y));
}

void blooDotMain::UpdateTouch(int id, Windows::Foundation::Point point)
{
    if (m_touches.find(id) != m_touches.end())
        m_touches[id] = PointToTouch(point, m_deviceResources->GetLogicalSize());
}

void blooDotMain::PointerMove(int id, Windows::Foundation::Point point)
{
	//m_pointerPosition = PointToTouch(point, m_deviceResources->GetLogicalSize());
	m_pointerPosition = XMFLOAT2(point.X, point.Y);
}

void blooDotMain::RemoveTouch(int id)
{
    m_touches.erase(id);
}

void blooDotMain::KeyDown(Windows::System::VirtualKey key)
{
    if (key == Windows::System::VirtualKey::P)       // Pause
    {
        m_pauseKeyActive = true;
    }
    if (key == Windows::System::VirtualKey::Home)
    {
        m_homeKeyActive = true;
    }
}

void blooDotMain::KeyUp(Windows::System::VirtualKey key)
{
    if (key == Windows::System::VirtualKey::P)
    {
        if (m_pauseKeyActive)
        {
            // trigger pause only once on button release
            m_pauseKeyPressed = true;
            m_pauseKeyActive = false;
        }
    }
    if (key == Windows::System::VirtualKey::Home)
    {
        if (m_homeKeyActive)
        {
            m_homeKeyPressed = true;
            m_homeKeyActive = false;
        }
    }

	if (m_deferredResourcesReadyPending)
	{
		m_deferredResourcesReady = true;
		SetGameState(GameState::LevelEditor);
	}
}

void blooDotMain::OnSuspending()
{
    SaveState();
    m_audio.SuspendAudio();
}

void blooDotMain::OnResuming()
{
    m_audio.ResumeAudio();
}

void blooDotMain::OnFocusChange(bool active)
{
    static bool lostFocusPause = false;

    if (m_deferredResourcesReady)
    {
        if (m_windowActive != active)
        {
            if (active)
            {
                m_audio.ResumeAudio();
                if ((m_gameState == GameState::InGamePaused) && lostFocusPause)
                {
                    SetGameState(GameState::InGameActive);
                }
            }
            else
            {
                m_audio.SuspendAudio();
                if (m_gameState == GameState::InGameActive)
                {
                    SetGameState(GameState::InGamePaused);
                    lostFocusPause = true;
                    SaveState();
                }
                else if (m_gameState == GameState::PreGameCountdown)
                {
                    SetGameState(GameState::MainMenu);
                    m_inGameStopwatchTimer.SetVisible(false);
                    m_preGameCountdownTimer.SetVisible(false);
                }
            }
            m_windowActive = active;
        }
    }
}

void blooDot::blooDotMain::ComputeFPS(float timeDelta)
{
	auto fpsMomentarily = 1.0f / timeDelta;
	if (m_FPSWatermark == 0)
	{
		m_FPSWatermark = 1;
		m_FPSCircular = 0;
	}
	else
	{
		if (m_FPSWatermark < FPSSampleSize)
		{
			++m_FPSWatermark;
		}

		++m_FPSCircular;
		if (m_FPSCircular == FPSSampleSize)
		{
			m_FPSCircular = 0;
		}
	}
	/* measurement of FPS for Lukas
	 * https://www.gamedev.net/forums/topic/510019-how-to-calculate-frames-per-second/ */
	m_FPS[m_FPSCircular] = fpsMomentarily;
}

int blooDot::blooDotMain::QueryFPS()
{
	if (m_FPSWatermark < FPSSampleSize)
	{
		return 0;
	}
	else
	{
		float fpsSum = 0.0F;
		for (auto i = 0; i < FPSSampleSize; ++i)
		{
			fpsSum += m_FPS[i];
		}

		return static_cast<int>(std::roundf(fpsSum / static_cast<float>(FPSSampleSize)));
	}
}

FORCEINLINE int FindMeshIndexByName(Mesh &mesh, const char *meshName)
{
    UINT meshCount = mesh.GetNumMeshes();
    for (UINT i = 0; i < meshCount; ++i)
    {
        if (0 == _stricmp(mesh.GetMesh(i)->Name, meshName))
            return i;
    }

    return -1; // Not found.
}

void blooDot::blooDotMain::LogMessage(Platform::Object^ obj)
{
	auto str = obj->ToString();
	auto formattedText = std::wstring(str->Data()).append(L"\r\n");
	OutputDebugString(formattedText.c_str());
}

HRESULT blooDotMain::ExtractTrianglesFromMesh(
    Mesh& mesh,
    const char* meshName,
    std::vector<Triangle>& triangles
    )
{
    triangles.clear();

    int meshIndex = FindMeshIndexByName(mesh, meshName);
    if (meshIndex < 0)
    {
        return E_FAIL;
    }
    MESH_MESH *currentmesh = mesh.GetMesh(meshIndex);

    for (UINT i = 0; i < currentmesh->NumSubsets; ++i)
    {
        MESH_SUBSET *subsetmesh = mesh.GetSubset(meshIndex, i);

        USHORT *indices = (USHORT*) mesh.GetRawIndicesAt(currentmesh->IndexBuffer) + subsetmesh->IndexStart;
        BYTE *vertices = mesh.GetRawVerticesAt(currentmesh->VertexBuffers[0]) + (subsetmesh->VertexStart * m_vertexStride);
        for (UINT j = 0; j < subsetmesh->IndexCount; j += 3)
        {
            XMFLOAT3 a, b, c;
            memcpy(&a, vertices + (*(indices++) * m_vertexStride), sizeof(XMFLOAT3));
            memcpy(&b, vertices + (*(indices++) * m_vertexStride), sizeof(XMFLOAT3));
            memcpy(&c, vertices + (*(indices++) * m_vertexStride), sizeof(XMFLOAT3));
            triangles.push_back(Triangle(a, b, c));
        }
    }

    return S_OK;
}

// Notifies renderers that device resources need to be released.
void blooDotMain::OnDeviceLost()
{
    m_sampleOverlay->ReleaseDeviceDependentResources();
    m_loadScreen->ReleaseDeviceDependentResources();
	m_worldScreen->ReleaseDeviceDependentResources();
    UserInterface::ReleaseDeviceDependentResources();

    m_inputLayout.Reset();
    m_vertexShader.Reset();
    m_pixelShader.Reset();
    m_sampler.Reset();
    m_constantBuffer.Reset();
    m_blendState.Reset();

    m_mazeMesh.Destroy();
    m_marbleMesh.Destroy();

	m_deferredResourcesReadyPending = false;
    m_deferredResourcesReady = false;
}

// Notifies renderers that device resources may now be re-created.
void blooDotMain::OnDeviceRestored()
{
	m_worldScreen->CreateDeviceDependentResources();
	m_sampleOverlay->CreateDeviceDependentResources();

    m_loadScreen->Initialize(m_deviceResources);
	m_worldScreen->Initialize(m_deviceResources);

    UserInterface::GetInstance().Initialize(
        m_deviceResources->GetD2DDevice(),
        m_deviceResources->GetD2DDeviceContext(),
        m_deviceResources->GetWicImagingFactory(),
        m_deviceResources->GetDWriteFactory()
    );

    CreateWindowSizeDependentResources();
    LoadDeferredResources(true, true);
}

bool blooDot::blooDotMain::ButtonJustPressed(GamepadButtons selection)
{
	bool newSelectionPressed = (selection == (m_newReading.Buttons & selection));
	bool oldSelectionPressed = (selection == (m_oldReading.Buttons & selection));
	return newSelectionPressed && !oldSelectionPressed;
}

bool blooDot::blooDotMain::ButtonJustReleased(GamepadButtons selection)
{
	bool newSelectionReleased = (GamepadButtons::None == (m_newReading.Buttons & selection));
	bool oldSelectionReleased = (GamepadButtons::None == (m_oldReading.Buttons & selection));
	return newSelectionReleased && !oldSelectionReleased;
}

Gamepad^ blooDot::blooDotMain::GetLastGamepad()
{
	Gamepad^ gamepad = nullptr;

	if (m_myGamepads->Size > 0)
	{
		gamepad = m_myGamepads->GetAt(m_myGamepads->Size - 1);
	}

	return gamepad;
}