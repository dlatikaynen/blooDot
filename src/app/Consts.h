#pragma once

namespace blooDot
{
	class Consts sealed
	{
	public:
		static const float SQUARE_WIDTH;
		static const float SQUARE_HEIGHT;
		static const float GOLDEN_RATIO;
		static const float INVERSE_GOLDEN_RATIO;
		static const float DIALOG_PADDING;
		static const float LASERBULLET_RADIUS;
	};

	enum class UIElement
	{
		None = 0,
		HighscoreTable = 1,
		LevelEditorHUD = 2,
		StartGameButton = 3,
		HighScoreButton = 4,
		PreGameCountdownTimer = 5,
		NerdStatsDisplay = 6,
		InGameStopwatchTimer = 7,
		CheckPointText = 8,
		PausedText = 9,
		ResultsText = 10,
		LocalGameButton = 11,
		NetworkGameButton = 12,
		WorldBuilderButton = 13,
		ConfigurationButton = 14,
		HelpAboutButton = 15,
		ExterminateButton = 16,
		ControllerSetup = 17,
		DingSheetDialog = 18 
	};
}
