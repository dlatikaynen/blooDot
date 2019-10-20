#pragma once

#include "..\dx\DirectXHelper.h"
#include "..\dx\DeviceResources.h"
#include "..\dx\BrushRegistry.h"

#include <stdlib.h>

class Dings
{
public:
	Dings(int dingID, Platform::String^ dingName, BrushRegistry drawBrushes);

	int					ID();
	Platform::String^	Name();
	virtual void		Draw(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo, int canvasX, int canvasY);

protected:
	int					m_ID;
	Platform::String^	m_Name;
	BrushRegistry		m_Brushes;
};

class Mauer : public Dings 
{
public:
	Mauer(BrushRegistry drawBrushes);
	void Draw(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo, int canvasX, int canvasY);
};