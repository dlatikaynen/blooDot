#include "..\PreCompiledHeaders.h"
#include "blooDot.h"
#include <DirectXColors.h>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <ppltasks.h>
#include <WinUser.h>
#include <concurrent_unordered_map.h>
#include "..\dx\DirectXHelper.h"
#include "..\dx\TTFLoader.h"

using namespace blooDot;
using namespace Windows::Gaming::Input;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::System::Diagnostics;
using namespace concurrency;

const byte blooDotMain::BLOODOTFILE_SIGNATURE[8] { 0x03, 'L','S','L', 0x04, 'J','M','L' };
const byte blooDotMain::BLOODOTFILE_CONTENTTYPE_GOLANIMATION[2] { 0xc9, 0x05 };
const byte blooDotMain::BLOODOTFILE_CONTENTTYPE_LEVEL_DESIGN[2] { 0xd1, 0x71 };

inline D2D1_RECT_F ConvertRect(Windows::Foundation::Size source)
{
    // ignore the source.X and source.Y, these are the location on the screen
    // yet we don't want to use them because all coordinates are window-relative
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
	m_triggerSuicide(false),
	m_deferredResourcesReadyPending(false),
    m_deferredResourcesReady(false),
	m_FPSCircular(0),
	m_FPSWatermark(0),
	m_currentLevel(nullptr)
{
    // Register to be notified if the Device is lost or recreated.
	this->m_deviceResources->RegisterDeviceNotify(this);
	this->m_Brushes = std::make_shared<BrushRegistry>();
    this->m_currentCheckpoint = 0;
    this->m_windowActive = false;
	this->m_keyInsertActive = false;
	this->m_keyDeletePressed = false;
	this->m_keyMinusActive = false;
	this->m_keyMinusPressed = false;
	this->m_keyMusicActive = false;
	this->m_keyMusicPressed = false;
	this->m_keyPlusActive = false;
	this->m_keyPlusPressed = false;
	this->m_keyPlaceActive = false;
	this->m_keyPlacePressed = false;
	this->m_keyObliterateActive = false;
	this->m_keyObliteratePressed = false;
	this->m_keySaveActive = false;
	this->m_keySavePressed = false;
	this->m_keySaveAsActive = false;
	this->m_keySaveAsPressed = false;
	this->m_keyLoadActive = false;
	this->m_keyLoadPressed = false;
	this->m_keyImportPressed = false;
	this->m_keyEnterActive = false;
	this->m_keyEnterPressed = false;
	this->m_keySpaceActive = false;
	this->m_keySpacePressed = false;
	this->m_keyEscapeActive = false;
	this->m_keyEscapePressed = false;

    // Checkpoints (from start to goal).
    m_checkpoints.push_back(XMFLOAT3(45.7f, -43.6f, -45.0f)); // Start
    m_checkpoints.push_back(XMFLOAT3(120.7f, -35.0f, -45.0f)); // Checkpoint 1
    m_checkpoints.push_back(XMFLOAT3(297.6f, -194.6f, -45.0f)); // Checkpoint 2
    m_checkpoints.push_back(XMFLOAT3(770.1f, -391.5f, -45.0f)); // Checkpoint 3
    m_checkpoints.push_back(XMFLOAT3(552.0f, -148.6f, -45.0f)); // Checkpoint 4
    m_checkpoints.push_back(XMFLOAT3(846.8f, -377.0f, -45.0f)); // Goal

    m_persistentState = ref new PersistentState();
    m_persistentState->Initialize(Windows::Storage::ApplicationData::Current->LocalSettings->Values, "blooDot");

	this->LoadFontCollection();
	this->m_audio = std::make_shared<Audio>();
    this->m_audio->Initialize();
	this->m_accelerometer = Windows::Devices::Sensors::Accelerometer::GetDefault();
	this->m_loadScreen = std::unique_ptr<LoadScreen>(new LoadScreen());
	this->m_loadScreen->Initialize(
		this->m_deviceResources,
		this->m_audio
	);

	UserInterface::GetInstance().Initialize(
		this->m_deviceResources->GetD2DDevice(),
		this->m_deviceResources->GetD2DDeviceContext(),
		this->m_deviceResources->GetWicImagingFactory(),
		this->m_deviceResources->GetDWriteFactory(),
		this->m_deviceResources->GetDWriteFactory3(),
		this->m_fontCollection
    );

	this->LoadState();
	this->CreateWindowSizeDependentResources();

    // TODO: Change timer settings if you want something other than the default variable timestep mode.
    // eg. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */

	// Input
	this->m_myGamepads = ref new Vector<Gamepad^>();
	for (auto gamepad : Gamepad::Gamepads)
	{
		this->m_myGamepads->Append(gamepad);
	}

	Gamepad::GamepadAdded += ref new EventHandler<Gamepad^>([=](Platform::Object^, Gamepad^ args)
	{
		this->m_myGamepads->Append(args);
		this->m_currentGamepadNeedsRefresh = true;
	});

	Gamepad::GamepadRemoved += ref new EventHandler<Gamepad ^>([=](Platform::Object^, Gamepad^ args)
	{
		unsigned int indexRemoved;
		if (this->m_myGamepads->IndexOf(args, &indexRemoved))
		{
			this->m_myGamepads->RemoveAt(indexRemoved);
			this->m_currentGamepadNeedsRefresh = true;
		}
	});

	this->m_gamepad = GetLastGamepad();
	this->m_currentGamepadNeedsRefresh = false;
}

blooDotMain::~blooDotMain()
{
    // deregister device notification
    m_deviceResources->RegisterDeviceNotify(nullptr);	
}

bool blooDotMain::Suicide()
{
	return this->m_triggerSuicide;
}

// Updates application state when the window size changes (e.g. device orientation change)
void blooDotMain::CreateWindowSizeDependentResources()
{
    // user interface
    constexpr float padding = 32.0f;
    D2D1_RECT_F clientRect = ConvertRect(m_deviceResources->GetLogicalSize());
    InflateRect(clientRect, -padding, -padding);

    D2D1_RECT_F topHalfRect = clientRect;
    topHalfRect.bottom = ((clientRect.top + clientRect.bottom) / 2.0f) - (padding / 2.0f);
    D2D1_RECT_F bottomHalfRect = clientRect;
    bottomHalfRect.top = topHalfRect.bottom + padding;
	
	D2D1_RECT_F containerRect = D2D1::RectF(clientRect.left, clientRect.top, clientRect.right, clientRect.top + 85.0f);
	this->m_mainMenuButtons.push_back(this->CreateMainMenuButton(L"start locally", UIElement::LocalGameButton, &containerRect));
	this->m_mainMenuButtons.push_back(this->CreateMainMenuButton(L"start networked", UIElement::NetworkGameButton, &containerRect));
	this->m_mainMenuButtons.push_back(this->CreateMainMenuButton(L"worldbuilder", UIElement::WorldBuilderButton, &containerRect));
	this->m_mainMenuButtons.push_back(this->CreateMainMenuButton(L"configuration", UIElement::ConfigurationButton, &containerRect));
	this->m_mainMenuButtons.push_back(this->CreateMainMenuButton(L"help~about", UIElement::HelpAboutButton, &containerRect));
	this->m_mainMenuButtons.push_back(this->CreateMainMenuButton(L"exterminate", UIElement::ExterminateButton, &containerRect));

    m_highScoreButton.Initialize(this->m_Brushes);
    m_highScoreButton.SetAlignment(AlignCenter, AlignCenter);
    m_highScoreButton.SetContainer(bottomHalfRect);
    m_highScoreButton.SetText(L"multiplayer");
    m_highScoreButton.SetTextColor(D2D1::ColorF(D2D1::ColorF::White));
    m_highScoreButton.GetTextStyle().SetFontWeight(DWRITE_FONT_WEIGHT_BLACK);
    m_highScoreButton.SetPadding(D2D1::SizeF(32.0f, 16.0f));
    m_highScoreButton.GetTextStyle().SetFontSize(72.0f);
    UserInterface::GetInstance().RegisterElement(blooDot::UIElement::HighScoreButton, &m_highScoreButton);

    m_highScoreTable.Initialize(this->m_Brushes);
    m_highScoreTable.SetAlignment(AlignCenter, AlignCenter);
    m_highScoreTable.SetContainer(clientRect);
    m_highScoreTable.SetTextColor(D2D1::ColorF(D2D1::ColorF::White));
    m_highScoreTable.GetTextStyle().SetFontWeight(DWRITE_FONT_WEIGHT_BOLD);
    m_highScoreTable.GetTextStyle().SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    m_highScoreTable.GetTextStyle().SetFontSize(60.0f);
    UserInterface::GetInstance().RegisterElement(blooDot::UIElement::HighscoreTable, &m_highScoreTable);

    m_preGameCountdownTimer.Initialize(this->m_Brushes);
    m_preGameCountdownTimer.SetAlignment(AlignCenter, AlignCenter);
    m_preGameCountdownTimer.SetContainer(clientRect);
    m_preGameCountdownTimer.SetTextColor(D2D1::ColorF(D2D1::ColorF::White));
    m_preGameCountdownTimer.GetTextStyle().SetFontWeight(DWRITE_FONT_WEIGHT_BLACK);
    m_preGameCountdownTimer.GetTextStyle().SetFontSize(144.0f);
    UserInterface::GetInstance().RegisterElement(blooDot::UIElement::PreGameCountdownTimer, &m_preGameCountdownTimer);

	m_nerdStatsDisplay.Initialize(this->m_Brushes);
	m_nerdStatsDisplay.SetAlignment(AlignType::AlignNear, AlignType::AlignNear);
	m_nerdStatsDisplay.SetContainer(clientRect);
	m_nerdStatsDisplay.SetTextColor(D2D1::ColorF(D2D1::ColorF::LightGoldenrodYellow));
	m_nerdStatsDisplay.GetTextStyle().SetFontWeight(DWRITE_FONT_WEIGHT_DEMI_BOLD);
	m_nerdStatsDisplay.GetTextStyle().SetFontSize(24.0f);
	UserInterface::GetInstance().RegisterElement(blooDot::UIElement::NerdStatsDisplay, &m_nerdStatsDisplay);

	this->m_levelEditorHUD.Initialize(this->m_Brushes);
	this->m_levelEditorHUD.SetAlignment(AlignType::AlignFar, AlignType::AlignNear);
	this->m_levelEditorHUD.SetContainer(clientRect);
	UserInterface::GetInstance().RegisterElement(blooDot::UIElement::LevelEditorHUD, &m_levelEditorHUD);

	this->m_dialogDingSheet.Initialize(this->m_Brushes);
	this->m_dialogDingSheet.SetAlignment(AlignType::AlignCenter, AlignType::AlignCenter);	
	this->m_dialogDingSheet.SetContainer(clientRect);
	UserInterface::GetInstance().RegisterElement(blooDot::UIElement::DingSheetDialog, &m_dialogDingSheet);

	this->m_controllerSetup.Initialize(this->m_Brushes);
	this->m_controllerSetup.SetAlignment(AlignType::AlignCenter, AlignType::AlignCenter);
	this->m_controllerSetup.SetContainer(clientRect);
	UserInterface::GetInstance().RegisterElement(blooDot::UIElement::ControllerSetup, &m_controllerSetup);

    m_inGameStopwatchTimer.Initialize(this->m_Brushes);
    m_inGameStopwatchTimer.SetAlignment(AlignNear, AlignFar);
    m_inGameStopwatchTimer.SetContainer(clientRect);
    m_inGameStopwatchTimer.SetTextColor(D2D1::ColorF(D2D1::ColorF::White, 0.75f));
    m_inGameStopwatchTimer.GetTextStyle().SetFontWeight(DWRITE_FONT_WEIGHT_BLACK);
    m_inGameStopwatchTimer.GetTextStyle().SetFontSize(96.0f);
    UserInterface::GetInstance().RegisterElement(blooDot::UIElement::InGameStopwatchTimer, &m_inGameStopwatchTimer);

    m_checkpointText.Initialize(this->m_Brushes);
    m_checkpointText.SetAlignment(AlignCenter, AlignCenter);
    m_checkpointText.SetContainer(clientRect);
    m_checkpointText.SetText(L"Checkpoint!");
    m_checkpointText.SetTextColor(D2D1::ColorF(D2D1::ColorF::White));
    m_checkpointText.GetTextStyle().SetFontWeight(DWRITE_FONT_WEIGHT_BLACK);
    m_checkpointText.GetTextStyle().SetFontSize(72.0f);
    UserInterface::GetInstance().RegisterElement(blooDot::UIElement::CheckPointText, &m_checkpointText);

    m_pausedText.Initialize(this->m_Brushes);
    m_pausedText.SetAlignment(AlignCenter, AlignCenter);
    m_pausedText.SetContainer(clientRect);
    m_pausedText.SetText(L"Paused");
    m_pausedText.SetTextColor(D2D1::ColorF(D2D1::ColorF::White));
    m_pausedText.GetTextStyle().SetFontWeight(DWRITE_FONT_WEIGHT_BLACK);
    m_pausedText.GetTextStyle().SetFontSize(72.0f);
    UserInterface::GetInstance().RegisterElement(blooDot::UIElement::PausedText, &m_pausedText);

    m_resultsText.Initialize(this->m_Brushes);
    m_resultsText.SetAlignment(AlignCenter, AlignCenter);
    m_resultsText.SetContainer(clientRect);
    m_resultsText.SetTextColor(D2D1::ColorF(D2D1::ColorF::White));
    m_resultsText.GetTextStyle().SetFontWeight(DWRITE_FONT_WEIGHT_BOLD);
    m_resultsText.GetTextStyle().SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    m_resultsText.GetTextStyle().SetFontSize(36.0f);
    UserInterface::GetInstance().RegisterElement(blooDot::UIElement::ResultsText, &m_resultsText);

    if ((!m_deferredResourcesReady) && m_loadScreen != nullptr)
    {
        m_loadScreen->UpdateForWindowSizeChange();
	}

	if (m_worldScreen != nullptr)
	{
		m_worldScreen->CreateDeviceDependentResources();
	}
}

TextButton* blooDotMain::CreateMainMenuButton(Platform::String^ captionText, UIElement elementKey, D2D1_RECT_F* containerRect)
{
	auto newButton = new TextButton();
	newButton->Initialize(this->m_Brushes);
	newButton->SetAlignment(AlignCenter, AlignCenter);
	newButton->SetContainer(*containerRect);
	newButton->SetText(captionText);
	newButton->SetTextColor(D2D1::ColorF(D2D1::ColorF::WhiteSmoke));
	newButton->GetTextStyle().SetFontWeight(DWRITE_FONT_WEIGHT_BOLD);
	newButton->SetPadding(D2D1::SizeF(16.0f, 8.0f));
	newButton->GetTextStyle().SetFontSize(52.0f);
	UserInterface::GetInstance().RegisterElement(elementKey, newButton);
	/* next line automatically */
	auto yDelta = (containerRect->bottom - containerRect->top) + 8.0f;
	(*containerRect).top += yDelta;
	(*containerRect).bottom += yDelta;
	return newButton;
}

void blooDotMain::LoadFontCollection()
{
	MFFontContext fContext(this->m_deviceResources->GetDWriteFactory());
	std::vector<std::wstring> filePaths;
	filePaths.push_back(L"Media\\Fonts\\FreckleFace-Regular.ttf");
	filePaths.push_back(L"Media\\Fonts\\FredokaOne-Regular.ttf");
	DX::ThrowIfFailed(fContext.CreateFontCollection(filePaths, &this->m_fontCollection));
}

void blooDotMain::LoadDeferredResources(bool delay, bool deviceOnly)
{
    DX::StepTimer loadingTimer;
    BasicLoader^ loader = ref new BasicLoader(m_deviceResources->GetD3DDevice());
	
    if (!deviceOnly)
    {
		/* When handling device lost, we only need to recreate the graphics - device related
		 * resources.All other delayed resources that only need to be created on app
		 * startup go here;
		 * (1) audio */
        this->m_audio->CreateResources();

		/* cycle the whole engine once. not doing so fucks up the audio engine on exit. whatevs. */
		this->m_audio->Start();
		this->m_audio->Render();
		this->m_audio->SuspendSfx();
		this->m_audio->SuspendMusic();
    }

    if (delay)
    {
        while (loadingTimer.GetTotalSeconds() < 0.5)
        {
            // game doesn't take long to load resources,
            // so we're simulating a longer load time to demonstrate
            // a more real world example
            loadingTimer.Tick([&]()
            {
                //do nothing, just wait
            });
        }

		this->m_deferredResourcesReadyPending = true;
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

	if (this->m_gameState == GameState::LoadScreen || this->m_gameState==GameState::MainMenu)
	{
		// Only render the loading screen for now.
		this->m_deviceResources->GetD3DDeviceContext()->BeginEventInt(L"Render Loading Screen", 0);
		this->m_loadScreen->Render(m_deviceResources->GetOrientationTransform2D(), m_pointerPosition);
		this->m_deviceResources->GetD3DDeviceContext()->EndEvent();
	}
	else if (this->m_gameState == GameState::LevelEditor)
	{
		this->m_deviceResources->GetD3DDeviceContext()->BeginEventInt(L"Render Level Editor", 0);
		this->m_worldScreen->Render(m_deviceResources->GetOrientationTransform2D(), m_pointerPosition);
		this->m_deviceResources->GetD3DDeviceContext()->EndEvent();
	}
	else if (this->m_gameState == GameState::InGameActive)
	{
		this->m_deviceResources->GetD3DDeviceContext()->BeginEventInt(L"Render World", 0);
		this->m_worldScreen->Render(m_deviceResources->GetOrientationTransform2D(), m_pointerPosition);
		this->m_deviceResources->GetD3DDeviceContext()->EndEvent();
	}
	else if (this->m_deferredResourcesReady)
	{
		this->m_deviceResources->GetD3DDeviceContext()->IASetInputLayout(m_inputLayout.Get());
	}

	if (this->m_audio->IsMusicPlaying())
	{
		this->m_audio->Render();
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
	case GameState::ControllerSelection:
		for (auto iter = this->m_mainMenuButtons.begin(); iter != this->m_mainMenuButtons.end(); ++iter)
		{
			(*iter)->SetVisible(false);
		}

		this->m_controllerSetup.SetVisible(false);
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
		for (auto iter = this->m_mainMenuButtons.begin(); iter != this->m_mainMenuButtons.end(); ++iter) (*iter)->SetVisible(true);
        (*this->m_mainMenuButtons.begin())->SetSelected(true);
		this->m_pausedText.SetVisible(false);
		this->m_physics.SetPosition(XMFLOAT3(305, -210, -43));
        break;

	case GameState::ControllerSelection:
		this->m_controllerSetup.SetVisible(true);
		break;

    case GameState::HighScoreDisplay:
		this->m_highScoreTable.SetVisible(true);
        break;

    case GameState::PreGameCountdown:
		this->m_inGameStopwatchTimer.SetVisible(false);
		this->m_preGameCountdownTimer.SetVisible(true);
		this->m_preGameCountdownTimer.StartCountdown(3);
		this->ResetCheckpoints();
		this->m_physics.SetPosition(m_checkpoints[0]);
		this->m_physics.SetVelocity(XMFLOAT3(0, 0, 0));
        break;

    case GameState::InGameActive:
        //m_pausedText.SetVisible(false);
        //m_inGameStopwatchTimer.Start();
        break;

    case GameState::InGamePaused:
		this->m_pausedText.SetVisible(true);
		this->m_inGameStopwatchTimer.Stop();
        break;

    case GameState::PostGameResults:
		this->m_inGameStopwatchTimer.Stop();
		this->m_inGameStopwatchTimer.SetVisible(false);
		this->m_resultsText.SetVisible(true);
        WCHAR formattedTime[32];
		this->m_inGameStopwatchTimer.GetFormattedTime(formattedTime, m_newHighScore.elapsedTime);
        WCHAR buffer[64];
        swprintf_s(
            buffer,
            L"%s\nYour time: %s",
            (this->m_newHighScore.wasJustAdded ? L"New High Score!" : L"Finished!"),
            formattedTime
        );
		this->m_resultsText.SetText(buffer);
		this->m_resultsText.SetVisible(true);
		break;
    }

    m_gameState = nextState;
}

void blooDotMain::ResetCheckpoints()
{
    m_currentCheckpoint = 0;
}

// Updates the application state once per frame.
void blooDotMain::Update()
{
	float timerTotal;
	float timerElapsed;

	if (this->m_gameState == GameState::Initial)
	{
		this->SetGameState(GameState::LoadScreen);
	}

	// Update scene objects.
	this->m_timer.Tick([&]()
    {
		timerTotal = static_cast<float>(this->m_timer.GetTotalSeconds());
		timerElapsed = static_cast<float>(this->m_timer.GetElapsedSeconds());

		/* compute FPS for for Lukas */
		this->ComputeFPS(timerElapsed);
		if (this->m_timer.GetFrameCount() % 100 == 0)
		{
			this->m_nerdStatsDisplay.UpdateFPS(this->QueryFPS());
		}

		UserInterface::GetInstance().Update(timerTotal, timerElapsed);

		if (this->m_keyMusicPressed)
		{
			this->m_keyMusicPressed = false;
			if (this->m_audio->IsMusicSuspended())
			{
				this->m_audio->ResumeMusic();
			}
			else
			{
				this->m_audio->SuspendMusic();
			}
		}

		// Process controller input.
#if WINAPI_FAMILY != WINAPI_FAMILY_PHONE_APP // Only process controller input when the device is not a phone.
		if (m_currentGamepadNeedsRefresh)
		{
			auto mostRecentGamepad = this->GetLastGamepad();
			if (this->m_gamepad != mostRecentGamepad)
			{
				this->m_gamepad = mostRecentGamepad;
			}

			this->m_currentGamepadNeedsRefresh = false;
		}

		if (this->m_gamepad != nullptr)
		{
			this->m_oldReading = this->m_newReading;
			this->m_newReading = this->m_gamepad->GetCurrentReading();
		}

		float leftStickX = static_cast<float>(m_newReading.LeftThumbstickX);
		float leftStickY = static_cast<float>(m_newReading.LeftThumbstickY);
#endif

		// When the game is first loaded, we display a load screen
        // and load any deferred resources that might be too expensive
        // to load during initialization.
		if (this->m_gameState == GameState::LoadScreen || this->m_gameState == GameState::MainMenu)
		{
			// At this point we can draw a progress bar, or if we had
			// loaded audio, we could play audio during the loading process.
			this->m_loadScreen->Update(timerTotal, timerElapsed);
		}
		else if (this->m_gameState == GameState::InGameActive)
		{
			if (!this->m_audio->IsMusicStarted())
			{
				this->m_audio->Start();
			}

			this->m_worldScreen->SetControl(
				this->m_pointerPosition,
				&this->m_touches,
				this->m_shiftKeyActive,
				this->m_keyLeftArrowPressed || this->ButtonJustPressed(GamepadButtons::DPadLeft),
				this->m_keyRightArrowPressed || this->ButtonJustPressed(GamepadButtons::DPadRight),
				this->m_keyUpArrowPressed || this->ButtonJustPressed(GamepadButtons::DPadUp),
				this->m_keyDownArrowPressed || this->ButtonJustPressed(GamepadButtons::DPadDown),
				this->m_keyLeftFineActive ? -1.0F : this->m_keyRightFineActive ? 1.0F : leftStickX,
				this->m_keyUpFineActive ? -1.0F : this->m_keyDownFineActive ? 1.0F : leftStickY
			);

			if (this->m_keyEnterActive)
			{
				dynamic_cast<WorldScreen*>(this->m_worldScreen.get())->SetControl(true);


			}

			this->m_worldScreen->Update(timerTotal, timerElapsed);
		}
		else if (this->m_gameState == GameState::LevelEditor)
		{
			if (!this->m_levelEditorHUD.IsVisible())
			{
				this->m_levelEditorHUD.SetVisible(true);
			}

			this->m_worldScreen->SetControl(
				this->m_pointerPosition,
				&this->m_touches,
				this->m_shiftKeyActive,
				this->m_keyLeftArrowPressed || this->ButtonJustPressed(GamepadButtons::DPadLeft),
				this->m_keyRightArrowPressed || this->ButtonJustPressed(GamepadButtons::DPadRight),
				this->m_keyUpArrowPressed || this->ButtonJustPressed(GamepadButtons::DPadUp),
				this->m_keyDownArrowPressed || this->ButtonJustPressed(GamepadButtons::DPadDown),
				leftStickX,
				leftStickY
			);

			if (this->m_keyPlacePressed || this->ButtonJustPressed(GamepadButtons::A))
			{
				this->m_keyPlacePressed = false;
				auto levelEditor = dynamic_cast<LevelEditor*>(this->m_worldScreen.get());
				levelEditor->ConsiderPlacement(this->m_shiftKeyActive);
			}

			if (this->m_keyGridPressed)
			{
				this->m_keyGridPressed = false;
				auto levelEditor = dynamic_cast<LevelEditor*>(this->m_worldScreen.get());
				levelEditor->DoToggleGrid();
			}

			if (this->m_keyDingSheetPressed)
			{
				this->m_keyDingSheetPressed = false;
				auto levelEditor = dynamic_cast<LevelEditor*>(this->m_worldScreen.get());
				levelEditor->DoToggleDingSheet();
			}

			if (this->m_keyTogglePanePressed)
			{
				this->m_keyTogglePanePressed = false;
				auto levelEditor = dynamic_cast<LevelEditor*>(this->m_worldScreen.get());
				levelEditor->SendDialogCommand(this->m_shiftKeyActive ? blooDot::DialogCommand::PreviousPane : blooDot::DialogCommand::NextPane);
			}

			if (this->m_keyRotatePressed || this->ButtonJustPressed(GamepadButtons::Y))
			{
				this->m_keyRotatePressed = false;
				auto levelEditor = dynamic_cast<LevelEditor*>(this->m_worldScreen.get());
				levelEditor->DoRotate(this->m_shiftKeyActive, this->m_ctrlKeyActive);
			}

			if (this->m_keyScrollLockStateChanged)
			{
				this->m_keyScrollLockStateChanged = false;
				auto levelEditor = dynamic_cast<LevelEditor*>(this->m_worldScreen.get());
				levelEditor->DoSetScrollLock(this->m_keyScrollLockState);
			}

			if (this->m_keyObliteratePressed || this->ButtonJustPressed(GamepadButtons::B))
			{
				this->m_keyObliteratePressed = false;
				auto levelEditor = dynamic_cast<LevelEditor*>(this->m_worldScreen.get());
				levelEditor->DoObliterateDing();
			}

			if (this->m_keyDeletePressed)
			{
				this->m_keyDeletePressed = false;
				this->m_levelEditorHUD.ToggleEraser();
			}

			if (this->m_keyInsertPressed)
			{
				this->m_keyInsertPressed = false;
				this->m_levelEditorHUD.ToggleOverwrite();
			}

			if (this->m_keyGridPressed)
			{
				this->m_keyGridPressed = false;
				this->m_levelEditorHUD.ToggleGrid();
			}

			if (this->m_keyDingSheetPressed)
			{
				this->m_keyDingSheetPressed = false;
				this->m_levelEditorHUD.ToggleDingSheet();
			}

			if (this->m_keyScrollLockStateChanged)
			{
				this->m_keyScrollLockStateChanged = false;
				this->m_levelEditorHUD.SetScrollLock(this->m_keyScrollLockState);
			}

			if (this->m_keyPlusPressed || this->ButtonJustPressed(GamepadButtons::RightShoulder))
			{
				this->m_keyPlusPressed = false;
				auto levelEditor = dynamic_cast<LevelEditor*>(this->m_worldScreen.get());
				levelEditor->SelectNextDingForPlacement();
			}

			if (this->m_keyMinusPressed || this->ButtonJustPressed(GamepadButtons::LeftShoulder))
			{
				this->m_keyMinusPressed = false;
				auto levelEditor = dynamic_cast<LevelEditor*>(this->m_worldScreen.get());
				levelEditor->SelectPreviousDingForPlacement();
			}

			if (this->m_keySavePressed)
			{
				this->m_keySavePressed = false;
				this->OnActionSaveLevel(false);
			}

			if (this->m_keySaveAsPressed)
			{
				this->m_keySaveAsPressed = false;
				this->OnActionSaveLevel(true);
			}

			if (this->m_keyLoadPressed)
			{
				this->m_keyLoadPressed = false;
				this->OnActionLoadLevel();
			}

			if (this->m_keyImportPressed)
			{
				this->m_keyImportPressed = false;
				this->OnActionImportLevel();
			}

			this->m_worldScreen->Update(timerTotal, timerElapsed);
		}
		
        // Check whether the user chose a button from the UI.
        bool anyPoints = !m_pointQueue.empty();
        while (!m_pointQueue.empty())
        {
            UserInterface::GetInstance().HitTest(m_pointQueue.front());
            m_pointQueue.pop();
        }

        switch (this->m_gameState)
        {
        case GameState::MainMenu:
			auto menuItemToExecute = UIElement::None;
			if (anyPoints)
			{
				menuItemToExecute = this->DetectMenuItemPressed();
			}
			else if (this->m_keySpacePressed || this->m_keyEnterPressed || this->ButtonJustPressed(GamepadButtons::A))
			{
				menuItemToExecute = this->DetectMenuItemSelected();
			}
			else if (this->m_keyEscapePressed)
			{
				menuItemToExecute = UIElement::ExterminateButton;
			}
			else 
			{
				/* navigate the menu with keyboard / gamepad */
				bool moveUp = this->ButtonJustPressed(GamepadButtons::DPadUp);
				if (!moveUp && this->m_keyUpArrowPressed)
				{
					this->m_keyUpArrowPressed = false;
					moveUp = true;
				}
				
				bool moveDown = false;
				if (!moveUp)
				{
					moveDown = this->ButtonJustPressed(GamepadButtons::DPadDown);
					if (!moveDown && this->m_keyDownArrowPressed)
					{
						this->m_keyDownArrowPressed = false;
						moveDown = true;
					}
				}

				if (moveUp || moveDown)
				{
					this->SelectMainMenu(moveUp, moveDown);
				}
			}

			if (menuItemToExecute == UIElement::LocalGameButton)
			{
				this->OnActionStartLocalGame();
			}			
			else if (menuItemToExecute == UIElement::NetworkGameButton)
			{

			}
			else if (menuItemToExecute == UIElement::WorldBuilderButton)
			{
				this->OnActionEnterLevelEditor();
			}
			else if (menuItemToExecute == UIElement::ConfigurationButton)
			{

			}
			else if (menuItemToExecute == UIElement::HelpAboutButton)
			{

			}
			else if (menuItemToExecute == UIElement::ExterminateButton)
			{
				this->OnActionTerminate();
			}

			if (menuItemToExecute != UIElement::None)
			{
				this->SwallowKeyPress();
			}

            break;
		}


        // Account for touch input.
        for (TouchMap::const_iterator iter = m_touches.cbegin(); iter != m_touches.cend(); ++iter)
        {
            //combinedTiltX += iter->second.x * m_touchScaleFactor;
            //combinedTiltY += iter->second.y * m_touchScaleFactor;
        }

        // Account for sensors.
        if (m_accelerometer != nullptr)
        {
            Windows::Devices::Sensors::AccelerometerReading^ reading =
                m_accelerometer->GetCurrentReading();

            if (reading != nullptr)
            {
                //combinedTiltX += static_cast<float>(reading->AccelerationX) * m_accelerometerScaleFactor;
                //combinedTiltY += static_cast<float>(reading->AccelerationY) * m_accelerometerScaleFactor;
            }
        }

        // Clamp input.
        //combinedTiltX = max(-1, min(1, combinedTiltX));
        //combinedTiltY = max(-1, min(1, combinedTiltY));

        //if (m_gameState != GameState::PreGameCountdown &&
        //    m_gameState != GameState::InGameActive &&
        //    m_gameState != GameState::InGamePaused)
        //{
        //    // Ignore tilt when the menu is active.
        //    combinedTiltX = 0.0f;
        //    combinedTiltY = 0.0f;
        //}

		if (this->m_keyEscapePressed)
		{
			this->m_keyEscapePressed = false;
		}

		if (this->m_keyEnterPressed)
		{
			this->m_keyEnterPressed = false;
		}

		if (this->m_keySpacePressed)
		{
			this->m_keySpacePressed = false;
		}
#pragma endregion



#pragma region Update Audio

        if (!m_audio->HasEngineExperiencedCriticalError())
        {
            //if (m_gameState == GameState::InGameActive)
            //{
            //    float wallDistances[8];
            //    int returnedCount = m_physics.GetRoomDimensions(wallDistances, ARRAYSIZE(wallDistances));
            //    assert(returnedCount == ARRAYSIZE(wallDistances));
            //    m_audio.SetRoomSize(m_physics.GetRoomSize(), wallDistances);
            //    CollisionInfo ci = m_physics.GetCollisionInfo();

            //    // Calculate roll sound, and pitch according to velocity.
            //    XMFLOAT3 velocity = m_physics.GetVelocity();
            //    XMFLOAT3 position = m_physics.GetPosition();
            //    float volumeX = abs(velocity.x) / 200;
            //    if (volumeX > 1.0) volumeX = 1.0;
            //    if (volumeX < 0.0) volumeX = 0.0;
            //    float volumeY = abs(velocity.y) / 200;
            //    if (volumeY > 1.0) volumeY = 1.0;
            //    if (volumeY < 0.0) volumeY = 0.0;
            //    float volume = max(volumeX, volumeY);

            //    // Pitch of the rolling sound ranges from .85 to 1.05f,
            //    // increasing logarithmically.
            //    float pitch = .85f + (volume * volume / 5.0f);

            //    // Play the roll sound only if the marble is actually rolling.
            //    if (ci.isRollingOnFloor && volume > 0)
            //    {
            //        if (!m_audio.IsSoundEffectStarted(RollingEvent))
            //        {
            //            m_audio.PlaySoundEffect(RollingEvent);
            //        }

            //        // Update the volume and pitch by the velocity.
            //        m_audio.SetSoundEffectVolume(RollingEvent, volume);
            //        m_audio.SetSoundEffectPitch(RollingEvent, pitch);

            //        // The rolling sound has at most 8000Hz sounds, so we linearly
            //        // ramp up the low-pass filter the faster we go.
            //        // We also reduce the Q-value of the filter, starting with a
            //        // relatively broad cutoff and get progressively tighter.
            //        m_audio.SetSoundEffectFilter(
            //            RollingEvent,
            //            600.0f + 8000.0f * volume,
            //            XAUDIO2_MAX_FILTER_ONEOVERQ - volume*volume
            //            );
            //    }
            //    else
            //    {
            //        m_audio.SetSoundEffectVolume(RollingEvent, 0);
            //    }

            //    if (ci.elasticCollision && ci.maxCollisionSpeed > 10)
            //    {
            //        m_audio.PlaySoundEffect(CollisionEvent);

            //        float collisionVolume = ci.maxCollisionSpeed / 150.0f;
            //        collisionVolume = min(collisionVolume * collisionVolume, 1.0f);
            //        m_audio.SetSoundEffectVolume(CollisionEvent, collisionVolume);
            //    }
            //}
            //else
            //{
            //    m_audio.SetSoundEffectVolume(RollingEvent, 0);
            //}
        }
#pragma endregion
    });
}

void blooDotMain::SwallowKeyPress()
{
	this->m_keyEnterActive = false;
	this->m_keyEnterPressed = false;
}

void blooDotMain::SelectMainMenu(bool moveUp, bool moveDown)
{
	auto didMove = false;
	if (moveUp)
	{
		for (auto iter = this->m_mainMenuButtons.crbegin(); std::next(iter) != this->m_mainMenuButtons.crend(); ++iter)
		{
			if ((*iter)->GetSelected())
			{
				(*iter)->SetSelected(false);
				(*(++iter))->SetSelected(true);
				didMove = true;
				break;
			}
		}
	}
	else if (moveDown)
	{
		for (auto iter = this->m_mainMenuButtons.cbegin(); std::next(iter) != this->m_mainMenuButtons.cend(); ++iter)
		{
			if ((*iter)->GetSelected())
			{
				(*iter)->SetSelected(false);
				(*(++iter))->SetSelected(true);
				didMove = true;
				break;
			}
		}
	}

	if (didMove)
	{
		this->m_audio->PlaySoundEffect(SoundEvent::MenuChangeEvent);
	}
	else if (moveUp || moveDown)
	{
		this->m_audio->PlaySoundEffect(SoundEvent::MenuTiltEvent);
	}
}

UIElement blooDotMain::DetectMenuItemSelected()
{
	auto currentlySelected = UserInterface::GetInstance().GetSelectedButton();
	if (currentlySelected != blooDot::UIElement::None)
	{
		this->m_audio->PlaySoundEffect(SoundEvent::MenuSelectedEvent);
	}
	
	return currentlySelected;
}

UIElement blooDotMain::DetectMenuItemPressed()
{
	auto lastPressed = UserInterface::GetInstance().PopPressed();
	if (lastPressed != blooDot::UIElement::None)
	{
		this->m_audio->PlaySoundEffect(SoundEvent::MenuSelectedEvent);
	}

	return lastPressed;
}

void blooDotMain::OnActionStartLocalGame()
{
	/* 1. do we need a controller setup?
	 * if not, start right away */
	if (this->HaveAtLeastOneGamePad())
	{
		this->SetGameState(GameState::ControllerSelection);
	}
	else 
	{
		this->OnActionEnterLevel();
	}
}

void blooDotMain::OnActionStartNetworkGame()
{

}

void blooDotMain::OnActionEnterLevel()
{
	auto levelName = ref new Platform::String(L"Grassmere-1");
	this->m_worldScreen = std::unique_ptr<WorldScreenBase>(new WorldScreen());
	this->m_currentLevel = std::make_shared<Level>(levelName, D2D1::SizeU(50, 30), 720, 720);
	this->m_currentLevel->Initialize(this->m_deviceResources, this->m_Brushes);
	this->m_currentLevel->DesignLoadFromFile(L"Media\\Levels\\grassmere.bloodot");
	this->m_currentLevel->SetupRuntimeState();
	this->m_worldScreen->EnterLevel(m_currentLevel);
	this->m_worldScreen->Initialize(this->m_audio, this->m_deviceResources, this->m_Brushes);
	this->SetGameState(GameState::InGameActive);
}

void blooDotMain::OnActionEnterLevelEditor()
{
	auto levelName = ref new Platform::String(L"Gartenwelt-1");
	this->m_worldScreen = std::unique_ptr<WorldScreenBase>(new LevelEditor());
	this->m_currentLevel = std::make_shared<Level>(levelName, D2D1::SizeU(50, 30), 720, 720);
	this->m_currentLevel->SetDesignTime();
	this->m_currentLevel->Initialize(this->m_deviceResources, this->m_Brushes);
	this->m_worldScreen->EnterLevel(this->m_currentLevel);
	this->m_worldScreen->Initialize(this->m_audio, this->m_deviceResources, this->m_Brushes);

#ifdef _DEBUG
	auto newObject = m_currentLevel->GetBlocksAt(353, 361, true);
	newObject->Instantiate(this->m_currentLevel->shared_from_this(), m_currentLevel->GetDing(Dings::DingIDs::Mauer), ClumsyPacking::ConfigurationFromNeighbors(Facings::Shy));
#endif 

	this->SetGameState(GameState::LevelEditor);
}

void blooDotMain::OnActionTerminate()
{
	this->m_audio->SuspendSfx();
	this->m_audio->SuspendMusic();
	this->m_triggerSuicide = true;
}

void blooDotMain::OnActionSaveLevel(bool forcePrompt)
{
	if (this->m_currentLevel != nullptr)
	{
		if (!forcePrompt && this->m_currentLevel->HasSaveFileNameBeenSpecifiedBefore())
		{
			this->SaveLevelInternal();
		}
		else
		{
			auto savePicker = ref new Windows::Storage::Pickers::FileSavePicker();
			savePicker->DefaultFileExtension = L".bloodot";
			savePicker->SuggestedStartLocation = Windows::Storage::Pickers::PickerLocationId::ComputerFolder;
			auto blooDotExtensions = ref new Platform::Collections::Vector<Platform::String^>();
			blooDotExtensions->Append(L".bloodot");
			savePicker->FileTypeChoices->Insert(L"blooDot Level File", blooDotExtensions);
			savePicker->SuggestedFileName = L"New Document";
			::create_task(savePicker->PickSaveFileAsync()).then([this](Windows::Storage::StorageFile^ destFile)
			{
				if (destFile)
				{
					this->m_knownLevelSaveTarget = destFile;
					this->SaveLevelInternal();
				}
			});
		}
	}
}

void blooDotMain::SaveLevelInternal()
{
	auto tempFolder = Windows::Storage::ApplicationData::Current->TemporaryFolder;
	auto fullTempFile = Platform::String::Concat(Platform::String::Concat(tempFolder->Path, L"\\"), this->m_knownLevelSaveTarget->Name);
	this->m_currentLevel->DesignSaveToFile(fullTempFile);
	auto openFileTask = tempFolder->GetFileAsync(this->m_knownLevelSaveTarget->Name);
	::create_task(openFileTask).then([this](Windows::Storage::StorageFile^ tempFile)
	{
		auto moveFileTask = tempFile->CopyAndReplaceAsync(this->m_knownLevelSaveTarget);
		::create_task(moveFileTask).then([this]()
		{
			this->m_audio->PlaySoundEffect(SoundEvent::CheckpointEvent);
		});
	});
}

void blooDotMain::OnActionLoadLevel()
{
	Windows::Storage::Pickers::FileOpenPicker^ openPicker = ref new Windows::Storage::Pickers::FileOpenPicker();
	openPicker->ViewMode = Windows::Storage::Pickers::PickerViewMode::Thumbnail;
	openPicker->SuggestedStartLocation = Windows::Storage::Pickers::PickerLocationId::ComputerFolder;
	openPicker->FileTypeFilter->Append(L".bloodot");
	create_task(openPicker->PickSingleFileAsync()).then([this](Windows::Storage::StorageFile^ sourceFile)
	{
		if (sourceFile)
		{
			Windows::Storage::StorageFolder^ targetFolder = Windows::Storage::ApplicationData::Current->TemporaryFolder;
			auto copyFileTask = sourceFile->CopyAsync(targetFolder, sourceFile->Name, Windows::Storage::NameCollisionOption::ReplaceExisting);
			create_task(copyFileTask).then([this, sourceFile](Windows::Storage::StorageFile^ copiedFile)
			{
				auto newLevel = this->m_worldScreen->LoadAndEnterLevel(copiedFile->Path);
				if (newLevel != nullptr)
				{					
					this->m_currentLevel = newLevel;
					this->m_knownLevelSaveTarget = sourceFile;
				}
			});
		}
	});
}

void blooDotMain::OnActionImportLevel()
{
	Windows::Storage::Pickers::FileOpenPicker^ importPicker = ref new Windows::Storage::Pickers::FileOpenPicker();
	importPicker->ViewMode = Windows::Storage::Pickers::PickerViewMode::Thumbnail;
	importPicker->SuggestedStartLocation = Windows::Storage::Pickers::PickerLocationId::ComputerFolder;
	importPicker->FileTypeFilter->Append(L".bloodot");
	create_task(importPicker->PickSingleFileAsync()).then([this](Windows::Storage::StorageFile^ importFile)
	{
		if (importFile)
		{
			Windows::Storage::StorageFolder^ targetFolder = Windows::Storage::ApplicationData::Current->TemporaryFolder;
			auto copyFileTask = importFile->CopyAsync(targetFolder, importFile->Name, Windows::Storage::NameCollisionOption::ReplaceExisting);
			create_task(copyFileTask).then([this, importFile](Windows::Storage::StorageFile^ copiedFile)
			{
				auto levelEditor = dynamic_cast<LevelEditor*>(this->m_worldScreen.get());
				if (levelEditor != nullptr)
				{
					auto importedLevel = levelEditor->LoadLevel(copiedFile->Path);
					if (importedLevel != nullptr)
					{
						levelEditor->ImportIntoCurrentLevel(importedLevel);
					}
				}
			});
		}
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
		break;

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
	{
		m_touches[id] = PointToTouch(point, m_deviceResources->GetLogicalSize());
	}
}

void blooDotMain::PointerMove(int id, Windows::Foundation::Point point)
{
	//this->m_pointerPosition = PointToTouch(point, m_deviceResources->GetLogicalSize());
	this->m_pointerPosition = XMFLOAT2(point.X, point.Y);
}

void blooDotMain::MouseWheeled(int pointerID, int detentCount)
{
	if (this->m_gameState == GameState::MainMenu)
	{
		if (detentCount < 0 && !this->m_keyDownArrowPressed)
		{
			this->m_keyDownArrowPressed = true;
		}
		else if (detentCount > 0 && !this->m_keyUpArrowPressed)
		{
			this->m_keyUpArrowPressed = true;
		}
	}
	else if (m_gameState == GameState::LevelEditor || m_gameState == GameState::InGameActive)
	{
		this->m_worldScreen->SetControl(detentCount, this->m_shiftKeyActive);
	}
}

void blooDotMain::RemoveTouch(int id)
{
    m_touches.erase(id);
}

void blooDotMain::KeyDown(Windows::System::VirtualKey key)
{
	if (key == Windows::System::VirtualKey::Shift)
	{
		m_shiftKeyActive = true;
	}
	else if (key == Windows::System::VirtualKey::Control)
	{
		m_ctrlKeyActive = true;
	}
	else if (key == Windows::System::VirtualKey::S && this->m_ctrlKeyActive)
	{
		m_keySaveActive = true;
	}
	else if (key == Windows::System::VirtualKey::A && this->m_ctrlKeyActive)
	{
		m_keySaveAsActive = true;
	}
	else if (key == Windows::System::VirtualKey::A)
	{
		this->m_keyLeftFineActive = true;
	}
	else if (key == Windows::System::VirtualKey::D)
	{
		this->m_keyRightFineActive = true;
	}
	else if (key == Windows::System::VirtualKey::S)
	{
		this->m_keyDownFineActive = true;
	}
	else if (key == Windows::System::VirtualKey::W)
	{
		this->m_keyUpFineActive = true;
	}
	else if (key == Windows::System::VirtualKey::L && this->m_ctrlKeyActive)
	{
		m_keyLoadActive = true;
	}
	else if (key == Windows::System::VirtualKey::I && this->m_ctrlKeyActive)
	{
		m_keyImportActive = true;
	}
	else if (key == Windows::System::VirtualKey::Home)
    {
        m_homeKeyActive = true;
    }
	else if (key == Windows::System::VirtualKey::Enter)
	{
		m_keyEnterActive = true;
	}
	else if (key == Windows::System::VirtualKey::Space)
	{
		m_keySpaceActive = true;
	}
	else if (key == Windows::System::VirtualKey::Escape)
	{
		m_keyEscapeActive = true;
	}
	else if (key == Windows::System::VirtualKey::P)
	{
		m_keyPlaceActive = true;
	}
	else if (key == Windows::System::VirtualKey::O)
	{
		m_keyObliterateActive = true;
	}
	else if (key == Windows::System::VirtualKey::Subtract)
	{
		m_keyMinusActive = true;
	}
	else if (key == Windows::System::VirtualKey::Add)
	{
		m_keyPlusActive = true;
	}
	else if (key == Windows::System::VirtualKey::M)
	{
		m_keyMusicActive = true;
	}
	else if (key == Windows::System::VirtualKey::Insert)
	{
		m_keyInsertActive = true;
	}
	else if (key == Windows::System::VirtualKey::Delete)
	{
		m_keyDeleteActive = true;
	}
	else if (key == Windows::System::VirtualKey::G)
	{
		m_keyGridActive = true;
	}
	else if (key == Windows::System::VirtualKey::F6)
	{
		this->m_keyTogglePaneActive = true;
	}
	else if (key == Windows::System::VirtualKey::F7)
	{
		m_keyDingSheetActive = true;
	}
	else if (key == Windows::System::VirtualKey::R)
	{
		m_keyRotateActive = true;
	}
	else if (key == Windows::System::VirtualKey::Left)
	{
		m_keyLeftArrowPressed = true;
	}
	else if (key == Windows::System::VirtualKey::Right)
	{
		m_keyRightArrowPressed = true;
	}
	else if (key == Windows::System::VirtualKey::Up)
	{
		m_keyUpArrowPressed = true;
	}
	else if (key == Windows::System::VirtualKey::Down)
	{
		m_keyDownArrowPressed = true;
	}
}

void blooDotMain::KeyUp(Windows::System::VirtualKey key)
{
	/* move this away from here as soon as "press any key" logic is done,
	 * or just have it depend on a timer */
	if (this->m_deferredResourcesReadyPending)
	{
		this->m_deferredResourcesReadyPending = false;
		this->m_deferredResourcesReady = true;
		this->SwallowKeyPress();
		this->SetGameState(GameState::MainMenu);
		this->m_loadScreen->ResourceLoadingCompleted();
		return;
	}
	
	if (key == Windows::System::VirtualKey::Home)
    {
        if (this->m_homeKeyActive)
        {
			this->m_homeKeyPressed = true;
			this->m_homeKeyActive = false;
        }
    }
	else if (key == Windows::System::VirtualKey::Enter)
	{
		if (this->m_keyEnterActive)
		{
			this->m_keyEnterPressed = true;
			this->m_keyEnterActive = false;
		}
	}
	else if (key == Windows::System::VirtualKey::Space)
	{
		if (this->m_keySpaceActive)
		{
			this->m_keySpacePressed = true;
			this->m_keySpaceActive = false;
		}
	}
	else if (key == Windows::System::VirtualKey::Escape)
	{
		if (this->m_keyEscapeActive)
		{
			this->m_keyEscapePressed = true;
			this->m_keyEscapeActive = false;
		}
	}
	else if (key == Windows::System::VirtualKey::M)
	{
		if (this->m_keyMusicActive)
		{
			this->m_keyMusicPressed = true;
			this->m_keyMusicActive = false;
		}
	}
	else if (key == Windows::System::VirtualKey::P)
	{
		if (this->m_keyPlaceActive)
		{
			this->m_keyPlacePressed = true;
			this->m_keyPlaceActive = false;
		}
	}
	else if (key == Windows::System::VirtualKey::O)
	{
		if (this->m_keyObliterateActive)
		{
			this->m_keyObliteratePressed = true;
			this->m_keyObliterateActive = false;
		}
	}
	else if (key == Windows::System::VirtualKey::Subtract)
	{
		if (this->m_keyMinusActive)
		{
			this->m_keyMinusPressed = true;
			this->m_keyMinusActive = false;
		}
	}
	else if (key == Windows::System::VirtualKey::Add)
	{
		if (this->m_keyPlusActive)
		{
			this->m_keyPlusPressed = true;
			this->m_keyPlusActive = false;
		}
	}
	else if (key == Windows::System::VirtualKey::Insert)
	{
		if (this->m_keyInsertActive)
		{
			this->m_keyInsertPressed = true;
			this->m_keyInsertActive = false;
		}
	}
	else if (key == Windows::System::VirtualKey::Delete)
	{
		if (this->m_keyDeleteActive)
		{
			this->m_keyDeletePressed = true;
			this->m_keyDeleteActive = false;
		}
	}
	else if (key == Windows::System::VirtualKey::G)
	{
		if (this->m_keyGridActive)
		{
			this->m_keyGridPressed = true;
			this->m_keyGridActive = false;
		}
	}
	else if (key == Windows::System::VirtualKey::F6)
	{
		if (this->m_keyTogglePaneActive)
		{
			this->m_keyTogglePanePressed = true;
			this->m_keyTogglePaneActive = false;
		}
	}
	else if (key == Windows::System::VirtualKey::F7)
	{
		if (this->m_keyDingSheetActive)
		{
			this->m_keyDingSheetPressed = true;
			this->m_keyDingSheetActive = false;
		}

		this->m_keyRightFineActive = false;
	}
	else if (key == Windows::System::VirtualKey::W)
	{
		this->m_keyUpFineActive = false;
	}
	else if (key == Windows::System::VirtualKey::R)
	{
		if (this->m_keyRotateActive)
		{
			this->m_keyRotatePressed = true;
			this->m_keyRotateActive = false;
		}
	}
	else if (key == Windows::System::VirtualKey::Scroll)
	{
		if (!this->m_keyScrollLockStateChanged)
		{
			auto hostWindow = m_deviceResources->GetWindow();
			auto vrkeyState = hostWindow->GetKeyState(key);
			bool newState = static_cast<bool>(vrkeyState & Windows::UI::Core::CoreVirtualKeyStates::Locked);
			if (this->m_keyScrollLockState != newState)
			{
				this->m_keyScrollLockState = newState;
				this->m_keyScrollLockStateChanged = true;
			}
		}
	}
	else if (key == Windows::System::VirtualKey::S)
	{
		if (this->m_keySaveActive)
		{
			this->m_keySavePressed = true;
			this->m_keySaveActive = false;
		}

		this->m_keyDownFineActive = false;
	}
	else if (key == Windows::System::VirtualKey::A)
	{
		if (this->m_keySaveAsActive)
		{
			this->m_keySaveAsPressed = true;
			this->m_keySaveAsActive = false;
		}

		this->m_keyLeftFineActive = false;
	}
	else if (key == Windows::System::VirtualKey::L)
	{
		if (this->m_keyLoadActive)
		{
			this->m_keyLoadPressed = true;
			this->m_keyLoadActive = false;
		}
	}
	else if (key == Windows::System::VirtualKey::I)
	{
		if (this->m_keyImportActive)
		{
			this->m_keyImportPressed = true;
			this->m_keyImportActive = false;
		}
	}
	else if (key == Windows::System::VirtualKey::Shift)
	{
		this->m_shiftKeyActive = false;
	}
	else if (key == Windows::System::VirtualKey::Control)
	{
		this->m_ctrlKeyActive = false;
	}
	else if (key == Windows::System::VirtualKey::Left)
	{
		this->m_keyLeftArrowPressed = false;
	}
	else if (key == Windows::System::VirtualKey::Right)
	{
		this->m_keyRightArrowPressed = false;
	}
	else if (key == Windows::System::VirtualKey::Up)
	{
		this->m_keyUpArrowPressed = false;
	}
	else if (key == Windows::System::VirtualKey::Down)
	{
		this->m_keyDownArrowPressed = false;
	}
}

void blooDotMain::OnSuspending()
{
	this->SaveState();
	this->m_audio->SuspendSfx();
	this->m_audio->SuspendMusic();
}

void blooDotMain::OnResuming()
{
	this->m_audio->ResumeMusic();
	this->m_audio->ResumeSfx();
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
                m_audio->ResumeMusic();
				m_audio->ResumeSfx();
				if ((m_gameState == GameState::InGamePaused) && lostFocusPause)
                {
                    SetGameState(GameState::InGameActive);
                }			
				else if ((m_gameState == GameState::LevelEditor) && !this->m_levelEditorHUD.IsVisible())
				{
					this->m_levelEditorHUD.SetVisible(true);
				}
			}
            else
            {
                m_audio->SuspendSfx();
				m_audio->SuspendMusic();
				if (m_gameState == GameState::InGameActive)
                {
                    //SetGameState(GameState::InGamePaused);
                    //lostFocusPause = true;
                    //SaveState();
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

// Notifies renderers that device resources need to be released.
void blooDotMain::OnDeviceLost()
{
    m_loadScreen->ReleaseDeviceDependentResources();
	m_worldScreen->ReleaseDeviceDependentResources();
    UserInterface::ReleaseDeviceDependentResources();
    m_inputLayout.Reset();
	m_deferredResourcesReadyPending = false;
    m_deferredResourcesReady = false;
}

// Notifies renderers that device resources may now be re-created.
void blooDotMain::OnDeviceRestored()
{
	//this->m_worldScreen->CreateDeviceDependentResources();
	this->m_loadScreen->Initialize(this->m_deviceResources, this->m_audio);
	this->m_worldScreen->Initialize(this->m_audio, this->m_deviceResources, this->m_Brushes);
    UserInterface::GetInstance().Initialize(
		this->m_deviceResources->GetD2DDevice(),
		this->m_deviceResources->GetD2DDeviceContext(),
		this->m_deviceResources->GetWicImagingFactory(),
		this->m_deviceResources->GetDWriteFactory(),
		this->m_deviceResources->GetDWriteFactory3(),
		this->m_fontCollection
    );

	this->CreateWindowSizeDependentResources();
	this->LoadDeferredResources(true, true);
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
	if (this->HaveAtLeastOneGamePad())
	{
		gamepad = this->m_myGamepads->GetAt(m_myGamepads->Size - 1);
	}

	return gamepad;
}

bool blooDot::blooDotMain::HaveAtLeastOneGamePad()
{
	return this->m_myGamepads->Size > 0;
}
