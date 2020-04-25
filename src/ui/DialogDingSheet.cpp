#include "..\PreCompiledHeaders.h"
#include "UserInterface.h"
#include "..\dx\DirectXHelper.h"
#include "DialogDingSheet.h"

DialogDingSheet::DialogDingSheet()
{
}

void DialogDingSheet::SetContent(std::shared_ptr<Level> levelInfo)
{	
	this->m_LevelInfo = levelInfo;
	this->m_dingSheetBitmap = levelInfo->GetDingSheetBmp();
	this->SetClientareaSize(this->m_dingSheetBitmap->GetSize());
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

			d2dContext->DrawRectangle(outerRim, this->m_chromeBrush.Get(), 0.7F);
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

					d2dContext->DrawRectangle(innerBound, this->m_chromeBrush.Get(), 0.7F);
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

					d2dContext->DrawEllipse(circularBound, this->m_chromeBrush.Get(), 0.7F);
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
}
