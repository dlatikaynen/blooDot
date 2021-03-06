﻿#pragma once

#include "..\io\BasicLoader.h"
#include "..\io\PersistentState.h"
#include "..\dx\DeviceResources.h"
#include "..\dx\Camera.h"
#include "..\dx\Mesh.h"
#include "..\dx\Primitives.h"
#include "..\physics\Collision.h"
#include "..\physics\Physics.h"
#include "..\sound\Audio.h"
#include "..\ui\UserInterface.h"
#include "..\ui\LoadScreen.h"
#include "..\ui\playground\WorldScreenBase.h"
#include "..\ui\DialogDingSheet.h"
#include "..\algo\ClumsyPacking.h"
#include "StepTimer.h"

using namespace DirectX;

enum class GameState
{
	Initial,
	LoadScreen,
	LevelEditor,
    MainMenu,
	ControllerSelection,
    HighScoreDisplay,
    PreGameCountdown,
    InGameActive,
    InGamePaused,
    PostGameResults
};

struct ConstantBuffer
{
	XMFLOAT4X4 model;
	XMFLOAT4X4 view;
	XMFLOAT4X4 projection;
	XMFLOAT3 marblePosition;
	float marbleRadius;
	float lightStrength;
};

enum class CheckpointState
{
    None, // No checkpoint hit.
    Save, // A save-game checkpoint was hit.
    Goal, // The end of game "goal" point was hit.
};

// Renders Direct2D and 3D content on the screen.
namespace blooDot
{
	class blooDotMain : public DX::IDeviceNotify
    {
    public:
		blooDotMain(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        ~blooDotMain();

		static const byte BLOODOTFILE_SIGNATURE[8];
		static const byte BLOODOTFILE_CONTENTTYPE_GOLANIMATION[2];
		static const byte BLOODOTFILE_CONTENTTYPE_LEVEL_DESIGN[2];

        // implementing IDeviceNotify
        virtual void OnDeviceLost();
        virtual void OnDeviceRestored();
        
        void CreateWindowSizeDependentResources();
        void Update();
        bool Render();
		bool Suicide();
        void LoadDeferredResources(bool delay, bool deviceOnly);
		bool IsDeferredLoadReady() { return m_deferredResourcesReady; }
		void OnSuspending();
        void OnResuming();
		void OnFocusChange(bool active);

		// input
		void AddTouch(int id, Windows::Foundation::Point point);
        void UpdateTouch(int id, Windows::Foundation::Point point);
		void PointerMove(int id, Windows::Foundation::Point point);
		void MouseWheeled(int pointerID, int detentCount);
		void RemoveTouch(int id);
        void KeyDown(Windows::System::VirtualKey key);
        void KeyUp(Windows::System::VirtualKey key);
		
	protected:
		void OnActionLoadLevel();
		void OnActionImportLevel();
		void OnActionSaveLevel(bool forcePrompt);

    private:
		static constexpr int						FPSSampleSize = 10;
		static constexpr float						m_deadzoneRadius = 0.1f;
		static constexpr float						m_deadzoneSquared = m_deadzoneRadius * m_deadzoneRadius;
		static constexpr float						m_controllerScaleFactor = 8.0f;
		static constexpr float						m_touchScaleFactor = 2.0f;
		static constexpr float						m_accelerometerScaleFactor = 3.5f;

		// Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources>		m_deviceResources;
		std::shared_ptr<BrushRegistry>				m_Brushes;
		DX::StepTimer								m_timer;
        std::unique_ptr<LoadScreen>					m_loadScreen;
		std::unique_ptr<WorldScreenBase>			m_worldScreen;
		std::shared_ptr<Level>						m_currentLevel;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
        Windows::Devices::Sensors::Accelerometer^	m_accelerometer;
		Windows::Storage::IStorageFile^				m_knownLevelSaveTarget;
		IDWriteFontCollection*						m_fontCollection;

        Collision				m_collision;
        Physics					m_physics;
        std::shared_ptr<Audio>	m_audio;
        GameState				m_gameState;
		typedef std::vector<TextButton*> Menubuttons;
		Menubuttons				m_mainMenuButtons;
        TextButton				m_highScoreButton;

        HighScoreTable		m_highScoreTable;
        CountdownTimer		m_preGameCountdownTimer;
        StopwatchTimer		m_inGameStopwatchTimer;
		NerdStatsDisplay	m_nerdStatsDisplay;
		LevelEditorHUD		m_levelEditorHUD;
		DialogDingSheet		m_dialogDingSheet;
		ControllerSetup		m_controllerSetup;
		TextElement			m_checkpointText;
        TextButton			m_pausedText;
        TextElement			m_resultsText;
        HighScoreEntry		m_newHighScore;
        TouchMap			m_touches;
		XMFLOAT2			m_pointerPosition;
        typedef std::queue<D2D1_POINT_2F> PointQueue;
        PointQueue			m_pointQueue;
		bool				m_triggerSuicide;
		bool				m_shiftKeyActive;
		bool				m_ctrlKeyActive;
		bool				m_homeKeyActive;
        bool				m_homeKeyPressed;
		bool				m_keyEnterActive;
		bool				m_keyEnterPressed;
		bool				m_keyEscapeActive;
		bool				m_keyEscapePressed;
		bool				m_keySpaceActive;
		bool				m_keySpacePressed;

		/* coarse (dented) motion control - corresponds to directional pad */
		bool				m_keyRightArrowPressed;
		bool				m_keyLeftArrowPressed;
		bool				m_keyDownArrowPressed;
		bool				m_keyUpArrowPressed;

		/* fine ("analog") motion control - corresponds to joystick */
		bool				m_keyRightFineActive;
		bool				m_keyLeftFineActive;
		bool				m_keyDownFineActive;
		bool				m_keyUpFineActive;

		bool				m_keyInsertActive;
		bool				m_keyInsertPressed;
		bool				m_keyGridActive;
		bool				m_keyGridPressed;
		bool				m_keyDingSheetActive;
		bool				m_keyDingSheetPressed;
		bool				m_keyTogglePaneActive;
		bool				m_keyTogglePanePressed;
		bool				m_keyDeleteActive;
		bool				m_keyDeletePressed;
		bool				m_keyRotateActive;
		bool				m_keyRotatePressed;
		bool				m_keyScrollLockStateChanged;
		bool				m_keyScrollLockState;
		bool				m_keySaveActive;
		bool				m_keySavePressed;
		bool				m_keySaveAsActive;
		bool				m_keySaveAsPressed;
		bool				m_keyLoadActive;
		bool				m_keyLoadPressed;
		bool				m_keyImportActive;
		bool				m_keyImportPressed;
		bool				m_keyMinusActive;
		bool				m_keyMinusPressed;
		bool				m_keyPlusActive;
		bool				m_keyPlusPressed;
		bool				m_keyPlaceActive;
		bool				m_keyPlacePressed;
		bool				m_keyObliterateActive;
		bool				m_keyObliteratePressed;
		bool				m_keyMusicActive;
		bool				m_keyMusicPressed;
		bool				m_windowActive;
		bool				m_deferredResourcesReadyPending;
		bool				m_deferredResourcesReady;
        PersistentState^	m_persistentState;
		float				m_FPS[FPSSampleSize];
		int					m_FPSCircular;
		int					m_FPSWatermark;

		// input
		Platform::Collections::Vector<Windows::Gaming::Input::Gamepad^>^	m_myGamepads;
		Windows::Gaming::Input::Gamepad^									m_gamepad;
		Windows::Gaming::Input::GamepadReading								m_newReading;
		Windows::Gaming::Input::GamepadReading								m_oldReading;
		bool																m_currentGamepadNeedsRefresh;

		// interaction and state
		bool ButtonJustPressed(Windows::Gaming::Input::GamepadButtons selection);
		bool ButtonJustReleased(Windows::Gaming::Input::GamepadButtons selection);
		Windows::Gaming::Input::Gamepad^ GetLastGamepad();
		bool HaveAtLeastOneGamePad();
		TextButton* CreateMainMenuButton(Platform::String^ captionText, UIElement elementKey, D2D1_RECT_F* containerRect);
		void SelectMainMenu(bool moveUp, bool MoveDown);
		UIElement DetectMenuItemSelected();
		UIElement DetectMenuItemPressed();
		void SwallowKeyPress();
		void SetGameState(GameState nextState);
		void SaveState();
		void LoadState();
		void OnActionStartLocalGame();
		void OnActionStartNetworkGame();
		void OnActionEnterLevel();
		void OnActionEnterLevelEditor();
		void SaveLevelInternal();
		void OnActionTerminate();

		// utility
		void LoadFontCollection();
		void ComputeFPS(float timeDelta);
		int QueryFPS();
		void LogMessage(Platform::Object^ obj);
	};
}