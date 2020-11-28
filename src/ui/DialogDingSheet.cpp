#include "..\PreCompiledHeaders.h"
#include "UserInterface.h"
#include "..\dx\DirectXHelper.h"
#include "DialogDingSheet.h"

DialogDingSheet::DialogDingSheet()
{
	this->m_currentPane = Pane::Dings;
}

void DialogDingSheet::Initialize(std::shared_ptr<BrushRegistry> brushRegistry)
{
	auto d2dContext = UserInterface::GetD2DContextWrapped();
	DialogOverlay::Initialize(brushRegistry);
	this->m_boundsBrush = this->m_brushRegistry->WannaHave(d2dContext, UserInterface::Color(D2D1::ColorF::Crimson));
}

Platform::String^ DialogDingSheet::StaticCaption()
{
	return this->m_currentPane == Pane::Mobs
		? L"Mobs Sheet"
		: L"Ding Sheet";
}

void DialogDingSheet::SetContent(std::shared_ptr<Level> levelInfo)
{	
	this->m_LevelInfo = levelInfo;
	this->m_dingSheetBitmap = this->m_currentPane == Pane::Mobs ? levelInfo->GetMobsSheetBmp() : levelInfo->GetDingSheetBmp();
	auto d2dContext = UserInterface::GetD2DContextWrapped();
	auto viewportSize = d2dContext->GetSize();
	this->m_clientAreaContent = this->m_dingSheetBitmap->GetSize();
	auto windowClientSize = UserInterface::Snug2Max(viewportSize, this->m_clientAreaContent);
	this->SetClientareaSize(windowClientSize);
}

void DialogDingSheet::Update(float timeTotal, float timeDelta)
{
	DialogOverlay::Update(timeTotal, timeDelta);
	switch (this->DequeDialogCommand())
	{
		case blooDot::DialogCommand::NextPane:
		case blooDot::DialogCommand::PreviousPane:
			this->TogglePane();
			break;
	}
}

void DialogDingSheet::TogglePane()
{
	this->m_currentPane = this->m_currentPane == Pane::Mobs ? Pane::Dings : Pane::Mobs;
	this->SetCaption(this->StaticCaption());
	this->SetContent(this->m_LevelInfo);
}

void DialogDingSheet::RenderClientarea(ID2D1DeviceContext* d2dContext)
{
	DialogOverlay::RenderClientarea(d2dContext);
	auto sourceRect = D2D1::RectF(
		this->m_clientAreaScrollOffset.x,
		this->m_clientAreaScrollOffset.y,
		this->m_clientAreaScrollOffset.x + (this->m_clientArea.right - this->m_clientArea.left),
		this->m_clientAreaScrollOffset.y + (this->m_clientArea.bottom - this->m_clientArea.top)
	);

	d2dContext->DrawBitmap(
		this->m_dingSheetBitmap,
		this->m_clientArea,
		1.0f,
		D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		sourceRect
	);

	/* bother to draw the weirdo dings' bounding boxes, that is,
	 * bounding geometries of all dings that have nontrivial outer rim bounds */
	auto dingIter = Dings::DingIDs::Mauer;
	while (dingIter != Dings::DingIDs::Void)
	{
		auto thisDing = this->m_LevelInfo->GetDing(dingIter);
		auto isMobPane = this->m_currentPane == Pane::Mobs;
		if (thisDing != nullptr && !thisDing->BoundingIsDefaultRect() && thisDing->IsMob() == isMobPane)
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
}

void DialogDingSheet::ReleaseDeviceDependentResources()
{
	DialogOverlay::ReleaseDeviceDependentResources();
}
