#pragma once

enum ViewportResolutions {
	VR_TEMPLE,   //  640 x 480
	VR_MODEX,    //  320 x 240
	VR_SVGA,     //  800 x 600
	VR_HERCMONO, //  720 x 348
	VR_NOTEBOOK, // 1280 x 720
	VR_MAXOUT    // fullscreen
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

enum SettingsMenuItems {
	SMI_BACK,
	SMI_SCREENSIZE,
	SMI_CONTROLS
};

enum ScreenSettingsMenuItems {
	SSMI_CANCEL,
	SSMI_VIDEOMODE_GOD,
	SSMI_VIDEOMODE_HERC,
	SSMI_VIDEOMODE_EGA,
	SSMI_VIDEOMODE_X,
	SSMI_VIDEOMODE_SVGA,
	SSMI_VIDEOMODE_NOTEBOOK,
	SSMI_VIDEOMODE_FULLSCREEN
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

inline DingProps operator|(DingProps a, DingProps b)
{
	return static_cast<DingProps>(static_cast<int>(a) | static_cast<int>(b));
}