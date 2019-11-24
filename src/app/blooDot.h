#pragma once

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
#include "..\ui\SampleOverlay.h"
#include "..\ui\LoadScreen.h"
#include "..\ui\WorldScreenBase.h"
#include "..\algo\ClumsyPacking.h"
#include "StepTimer.h"

using namespace DirectX;

// Describes the constant buffer that draws the meshes.
struct ConstantBuffer
{
    XMFLOAT4X4 model;
    XMFLOAT4X4 view;
    XMFLOAT4X4 projection;

    XMFLOAT3 marblePosition;
    float marbleRadius;
    float lightStrength;
};

enum class GameState
{
	Initial,
	LoadScreen,
	LevelEditor,
    MainMenu,
    HighScoreDisplay,
    PreGameCountdown,
    InGameActive,
    InGamePaused,
    PostGameResults,
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

        // IDeviceNotify
        virtual void OnDeviceLost();
        virtual void OnDeviceRestored();
        
        void CreateWindowSizeDependentResources();
        void Update();
        bool Render();

        void LoadDeferredResources(bool delay, bool deviceOnly);

        void OnSuspending();
        void OnResuming();

        void AddTouch(int id, Windows::Foundation::Point point);
        void UpdateTouch(int id, Windows::Foundation::Point point);
		void PointerMove(int id, Windows::Foundation::Point point);
		void RemoveTouch(int id);
        void KeyDown(Windows::System::VirtualKey key);
        void KeyUp(Windows::System::VirtualKey key);
        void OnFocusChange(bool active);
        bool IsDeferredLoadReady() {return m_deferredResourcesReady;}

	protected:
		void OnActionLoadLevel();
		void OnActionSaveLevel(bool forcePrompt);

    private:
		static const int							FPSSampleSize = 10;
		const float									m_deadzoneRadius = 0.1f;
		const float									m_deadzoneSquared = m_deadzoneRadius * m_deadzoneRadius;
		const float									m_controllerScaleFactor = 8.0f;
		const float									m_touchScaleFactor = 2.0f;
		const float									m_accelerometerScaleFactor = 3.5f;

		// Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources>		m_deviceResources;
        std::unique_ptr<SampleOverlay>				m_sampleOverlay;
        DX::StepTimer								m_timer;
        std::unique_ptr<LoadScreen>					m_loadScreen;
		std::unique_ptr<WorldScreenBase>			m_worldScreen;
		Level*										m_currentLevel;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
        Microsoft::WRL::ComPtr<ID3D11VertexShader>  m_vertexShader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
        Microsoft::WRL::ComPtr<ID3D11SamplerState>	m_sampler;
        Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBuffer;
        Microsoft::WRL::ComPtr<ID3D11BlendState>	m_blendState;
        Windows::Devices::Sensors::Accelerometer^	m_accelerometer;
        std::unique_ptr<Camera>						m_camera;

        ConstantBuffer		m_mazeConstantBufferData;
        Mesh				m_mazeMesh;
        ConstantBuffer		m_marbleConstantBufferData;
        Mesh				m_marbleMesh;
        unsigned int		m_vertexStride;
        float				m_lightStrength;
        float				m_targetLightStrength;
        Collision			m_collision;
        Physics				m_physics;
        Audio				m_audio;
        GameState			m_gameState;
        bool				m_resetCamera;
        bool				m_resetMarbleRotation;
        typedef std::vector<XMFLOAT3> Checkpoints;
        Checkpoints			m_checkpoints;
        size_t				m_currentCheckpoint;
        TextButton			m_startGameButton;
        TextButton			m_highScoreButton;
        HighScoreTable		m_highScoreTable;
        CountdownTimer		m_preGameCountdownTimer;
        StopwatchTimer		m_inGameStopwatchTimer;
		NerdStatsDisplay	m_nerdStatsDisplay;
		LevelEditorHUD		m_levelEditorHUD;
        TextElement			m_checkpointText;
        TextButton			m_pausedText;
        TextElement			m_resultsText;
        HighScoreEntry		m_newHighScore;
		typedef std::map<int, XMFLOAT2> TouchMap;
        TouchMap			m_touches;
		XMFLOAT2			m_pointerPosition;
        typedef std::queue<D2D1_POINT_2F> PointQueue;
        PointQueue			m_pointQueue;
        bool				m_pauseKeyActive;
        bool				m_pauseKeyPressed;
		bool				m_shiftKeyActive;
		bool				m_homeKeyActive;
        bool				m_homeKeyPressed;
		bool				m_keyRightPressed;
		bool				m_keyLeftPressed;
		bool				m_keyUpPressed;
		bool				m_keyDownPressed;
		bool				m_keyInsertActive;
		bool				m_keyInsertPressed;
		bool				m_keyDeleteActive;
		bool				m_keyDeletePressed;
		bool				m_keySaveActive;
		bool				m_keySavePressed;
		bool				m_keySaveAsActive;
		bool				m_keySaveAsPressed;
		bool				m_keyLoadActive;
		bool				m_keyLoadPressed;
		bool				m_keyMinusActive;
		bool				m_keyMinusPressed;
		bool				m_keyPlusActive;
		bool				m_keyPlusPressed;
		bool				m_keyMusicActive;
		bool				m_keyMusicPressed;
		bool				m_windowActive;
		bool				m_deferredResourcesReadyPending;
		bool				m_deferredResourcesReady;
        PersistentState^	m_persistentState;
		float				m_FPS[FPSSampleSize];
		int					m_FPSCircular;
		int					m_FPSWatermark;

		// Input
		Platform::Collections::Vector<Windows::Gaming::Input::Gamepad^>^	m_myGamepads;
		Windows::Gaming::Input::Gamepad^									m_gamepad;
		Windows::Gaming::Input::GamepadReading								m_newReading;
		Windows::Gaming::Input::GamepadReading								m_oldReading;
		bool																m_currentGamepadNeedsRefresh;

        HRESULT ExtractTrianglesFromMesh(
            Mesh& mesh,
            const char* meshName,
            std::vector<Triangle>& triangles
            );

        void ResetCheckpoints();
        CheckpointState UpdateCheckpoints();

		void ComputeFPS(float timeDelta);
		int QueryFPS();

        void SetGameState(GameState nextState);
        void SaveState();
        void LoadState();

		bool ButtonJustPressed(Windows::Gaming::Input::GamepadButtons selection);
		bool ButtonJustReleased(Windows::Gaming::Input::GamepadButtons selection);
		Windows::Gaming::Input::Gamepad^ GetLastGamepad();
		void LogMessage(Platform::Object^ obj);
	};

	template <class T> void SafeRelease(T **ppT)
	{
		if (*ppT)
		{
			(*ppT)->Release();
			*ppT = NULL;
		}
	}
}