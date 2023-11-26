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
	DingProps GetDesignLayer();
	bool SetDesignLayer(DingProps layer);
	bool DesignLayerUp();
	bool DesignLayerDown();
	bool GetGridLock();
	bool SetGridlock(bool gridLock);
	bool ToggleGridLock();
	bool GetCoalesce();
	bool SetCoalesce(bool coalesce);
	bool ToggleCoalesce();
	bool Redraw(int ofPlayerIndex);
	void Render();
	void Teardown();

	bool _InitializeCreatorModeTools();
	bool _DrawLetterboxBackdrops(int height);
	bool _RedrawDesignLayersTool();
}