#pragma once

enum ViewportResolutions : char {
	VR_TEMPLE,   //  640 x 480
	VR_MODEX,    //  320 x 240
	VR_HERCMONO, //  720 x 348
	VR_SVGA,     //  800 x 600
	VR_NOTEBOOK, // 1280 x 720
	VR_SQUARE,   // fullscreen aspect ratio 1
	VR_MAXOUT    // fullscreen
};

enum SoundEffect {
	SFX_BULLET_DECAY,
	SFX_BUMP,
	SFX_SELCHG,
	SFX_SELCONF,
	SFX_ASTERISK,
	SFX_CRICKET_FRITZ,
	SFX_CRICKET_FRANZ
};

enum UserInterfaceLanguages : char {
	UIL_AMERICAN,
	UIL_FINNISH,
	UIL_GERMAN,
	UIL_UKRAINIAN
};

enum FlappySituation {
	FS_BUNGHOLE,
	FS_VERTICAL,
	FS_HORIZONTAL,
	FS_QUARTERED
};

enum MainMenuItems {
	MMI_CUE,
	MMI_LOAD,
	MMI_NEWSINGLE,
	MMI_NEWMULTILOCAL,
	MMI_NEWMULTINET,
	MMI_SETTINGS,
	MMI_EXIT
};

enum LoadMenuItems {
	LMI_CANCEL,
	LMI_SAVEGAME
};

enum InGameMenuItems {
	IGMI_DISMISS,
	IGMI_SAVE,
	IGMI_INVENTORY,
	IGMI_MAP,
	IGMI_CONTROLS,
	IGMI_KNOWLEDGEBASE,
	IGMI_LEAVE
};

enum SettingsMenuItems {
	SMI_BACK,
	SMI_SCREENSIZE,
	SMI_CONTROLS,
	SMI_LANGUAGE,
	SMI_HELP,
	SMI_ABOUT
};

enum AboutMenuItems
{
	HAM_BACK
};

enum ScreenSettingsMenuItems {
	SSMI_CANCEL,
	SSMI_VIDEOMODE
};

enum MultiMonitorMenuItems {
	MMMI_CANCEL,
	MMMI_DISPLAY
};

enum LangSettingsMenuItems {
	LSMI_CANCEL,
	LSMI_USER_INTERFACE_LANGUAGE
};

enum DingProps : unsigned char
{
	Default = 0,
	Floor = 1,
	Walls = 2,
	Rooof = 4,
	Mob = 8,
	Loot = 16
};

inline constexpr DingProps operator|(DingProps a, DingProps b)
{
	return static_cast<DingProps>(static_cast<int>(a) | static_cast<int>(b));
}

inline ViewportResolutions operator--(ViewportResolutions& vpr)
{
	return static_cast<ViewportResolutions>(static_cast<int>(vpr) - 1);
}

inline ViewportResolutions operator++(ViewportResolutions& vpr)
{
	return static_cast<ViewportResolutions>(static_cast<int>(vpr) + 1);
}

inline UserInterfaceLanguages operator--(UserInterfaceLanguages& uil)
{
	return static_cast<UserInterfaceLanguages>(static_cast<int>(uil) - 1);
}

inline UserInterfaceLanguages operator++(UserInterfaceLanguages& uil)
{
	return static_cast<UserInterfaceLanguages>(static_cast<int>(uil) + 1);
}