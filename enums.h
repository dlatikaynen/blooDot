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

enum DingProps : unsigned char
{
	Floor = 1,
	Walls = 2,
	Rooof = 4
};