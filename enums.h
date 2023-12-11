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
	SFX_CRICKET_FRANZ,
	SFX_FLOOR,
	SFX_WALLS,
	SFX_ROOOF
};

enum UserInterfaceLanguages : char {
	UIL_AMERICAN,
	UIL_FINNISH,
	UIL_GERMAN,
	UIL_UKRAINIAN
};

enum InputDeviceChoices : char {
	IDC_KYBD,
	IDC_CONTROLLER1,
	IDC_CONTROLLER2,
	IDC_CONTROLLER3,
	IDC_CONTROLLER4
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
	MMI_NEWMULTI,
	MMI_CREATORMODE,
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

enum CreatorModeMenuItems {
	CMMI_BACK,
	CMMI_LAST_LOCATION,
	CMMI_TELEPORT
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

enum ControlsSettingsMenuItems {
	CSMI_CANCEL,
	CSMI_INPUT_DEVICES
};

enum DialogMenuResult {
	DMR_NONE,
	DMR_CANCEL,
	DMR_LAUNCH_MAKER
};

enum ControlHighlight {
	CH_NONE,
	CH_MAINMENU,
	CH_INGAME,
	CH_CREATORMODE
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

inline InputDeviceChoices operator--(InputDeviceChoices& uil)
{
	return static_cast<InputDeviceChoices>(static_cast<int>(uil) - 1);
}

inline InputDeviceChoices operator++(InputDeviceChoices& uil)
{
	return static_cast<InputDeviceChoices>(static_cast<int>(uil) + 1);
}