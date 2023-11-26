#include "playerstate.h"
#include "settings.h"
#include "drawing.h"
#include "resutil.h"
#include "collision.h"
#include "constants.h"
#include "scripture.h"
#include "xlations.h"
#include "dialogcontrols.h"
#include "enums.h"
#pragma once

namespace blooDot::Hud
{
	bool Initialize();
	bool SetDesignLayer(DingProps layer);
	bool Redraw(int ofPlayerIndex);
	void Render();
	void Teardown();

	bool _InitializeCreatorModeTools();
	bool _DrawLetterboxBackdrops(int height);
	bool _RedrawDesignLayersTool();
}