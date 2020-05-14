#include "..\PreCompiledHeaders.h"
#include "UserInterface.h"
#include "..\dx\DirectXHelper.h"
#include "DialogDingSheet.h"

DialogDingSheet::DialogDingSheet()
{
	this->der_boese_kurtx = 0;
	this->der_boese_kurty = 0;
}

void DialogDingSheet::Initialize()
{
	auto d2dContext = UserInterface::GetD2DContext();
	DX::ThrowIfFailed(d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Crimson), &this->m_boundsBrush));
	DialogOverlay::Initialize();
}

void DialogDingSheet::SetContent(std::shared_ptr<Level> levelInfo)
{	
	this->m_LevelInfo = levelInfo;
	this->m_dingSheetBitmap = levelInfo->GetDingSheetBmp();
	this->SetClientareaSize(this->m_dingSheetBitmap->GetSize());
}

void DialogDingSheet::Update(float timeTotal, float timeDelta)
{
	DialogOverlay::Update(timeTotal, timeDelta);
	this->der_boese_kurtx += 16;
	this->der_boese_kurty -= 7;
	if (this->der_boese_kurtx > 1000)
	{
		this->der_boese_kurtx = 0;

	}

	if (this->der_boese_kurty < 0) {
		this->der_boese_kurty = 768;

	}
}

void DialogDingSheet::RenderClientarea(ID2D1DeviceContext* d2dContext)
{
	DialogOverlay::RenderClientarea(d2dContext);
	auto dingSheetSize = this->m_dingSheetBitmap->GetSize();
	auto sourceRect = D2D1::RectF(0.0F, 0.0F, dingSheetSize.width - 1.0F, dingSheetSize.height - 1.0F);
	d2dContext->DrawBitmap(
		this->m_dingSheetBitmap,
		this->m_clientArea,
		1.0f,
		D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		sourceRect
	);

	/* bother to draw the mobs' bounding boxes, that is,
	 * bounding geometries of all dings that have nontrivial outer rim bounds */
	auto dingIter = Dings::DingIDs::Mauer;
	while (dingIter != Dings::DingIDs::Void)
	{
		auto thisDing = this->m_LevelInfo->GetDing(dingIter);
		if (thisDing != nullptr && !thisDing->BoundingIsDefaultRect())
		{
			auto sheetPlacement = thisDing->GetSheetPlacement(Facings::North);
			auto boundingGeometry = thisDing->GetBoundingInfo();
			auto outerRim = D2D1::RectF(
				this->m_clientArea.left + (sheetPlacement.x * blooDot::Consts::SQUARE_WIDTH) + boundingGeometry->OuterRim.left,
				this->m_clientArea.top + (sheetPlacement.y * blooDot::Consts::SQUARE_HEIGHT) + boundingGeometry->OuterRim.top,
				this->m_clientArea.left + (sheetPlacement.x * blooDot::Consts::SQUARE_WIDTH) + boundingGeometry->OuterRim.right,
				this->m_clientArea.top + (sheetPlacement.y * blooDot::Consts::SQUARE_HEIGHT) + boundingGeometry->OuterRim.bottom
			);

			d2dContext->DrawRectangle(outerRim, this->m_boundsBrush.Get(), 0.7F);
			for (auto iter = boundingGeometry->Geometries.begin(); iter != boundingGeometry->Geometries.end(); ++iter)
			{
				switch (iter->Shape)
				{
				case BoundingGeometry::Primitive::Rectangle:
					auto innerBound = D2D1::RectF(
						this->m_clientArea.left + (sheetPlacement.x * blooDot::Consts::SQUARE_WIDTH) + iter->Bounds.left,
						this->m_clientArea.top + (sheetPlacement.y * blooDot::Consts::SQUARE_HEIGHT) + iter->Bounds.top,
						this->m_clientArea.left + (sheetPlacement.x * blooDot::Consts::SQUARE_WIDTH) + iter->Bounds.right,
						this->m_clientArea.top + (sheetPlacement.y * blooDot::Consts::SQUARE_HEIGHT) + iter->Bounds.bottom
					);

					d2dContext->DrawRectangle(innerBound, this->m_boundsBrush.Get(), 0.7F);
					break;

				case BoundingGeometry::Primitive::Circle:
					auto radiusX = (iter->Bounds.right - iter->Bounds.left) / 2.0F;
					auto radiusY = (iter->Bounds.bottom - iter->Bounds.top) / 2.0F;
					auto circularBound = D2D1::Ellipse(
						D2D1::Point2F(
							this->m_clientArea.left + (sheetPlacement.x * blooDot::Consts::SQUARE_WIDTH) + iter->Bounds.left + radiusX,
							this->m_clientArea.top + (sheetPlacement.y * blooDot::Consts::SQUARE_HEIGHT) + iter->Bounds.top + radiusY
						),
						radiusX,
						radiusY
					);

					d2dContext->DrawEllipse(circularBound, this->m_boundsBrush.Get(), 0.7F);
					break;
				}
			}
		}

		dingIter = this->m_LevelInfo->GetNextDingID(dingIter);
		if (dingIter == Dings::DingIDs::Mauer)
		{
			break;
		}
	} 	

	auto Kurt = m_chromeBrush->GetColor();


	m_chromeBrush->SetColor(D2D1::ColorF(D2D1::ColorF::DarkBlue));
	d2dContext->DrawRectangle(D2D1::RectF(500, 440, 640, 700), m_chromeBrush.Get(), 30);
	m_chromeBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Crimson));
	d2dContext->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(der_boese_kurtx, der_boese_kurty), 100, 100), m_chromeBrush.Get(), 70);
	m_chromeBrush->SetColor(Kurt);
	d2dContext->DrawRectangle(D2D1::RectF(450, 440, 600, 650), m_chromeBrush.Get(), 50);
	m_chromeBrush->SetColor(D2D1::ColorF(D2D1::ColorF::LimeGreen));








}

void DialogDingSheet::ReleaseDeviceDependentResources()
{
	this->m_boundsBrush.Reset();
	DialogOverlay::ReleaseDeviceDependentResources();
}
