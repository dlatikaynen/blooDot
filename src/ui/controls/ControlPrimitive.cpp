#include "..\..\PreCompiledHeaders.h"
#include "..\UserInterface.h"
#include "..\..\dx\DirectXHelper.h"

#include "ControlPrimitive.h"

void ControlPrimitive::DrawScrollIndicators(
	Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2dContext,
	Microsoft::WRL::ComPtr<ID2D1Factory1> d2dFactory,
	std::shared_ptr<BrushRegistry> brushRegistry,
	D2D1_RECT_F clientArea,
	ElementBase::Directions drawSides
)
{
	if (drawSides != ElementBase::Directions::NONE)
	{
		auto borderBrush = brushRegistry->WannaHave(d2dContext, UserInterface::Color(D2D1::ColorF::WhiteSmoke, 196));
		auto backBrush = brushRegistry->WannaHave(d2dContext, UserInterface::Color(D2D1::ColorF::WhiteSmoke, 48));
		/* prepare all possible points, this is trivial */
		constexpr float nudge = 3.f;
		const float inset = blooDot::Consts::SQUARE_WIDTH * blooDot::Consts::INVERSE_GOLDEN_RATIO;
		D2D1_POINT_2F points[] =
		{
			D2D1::Point2F(clientArea.left - nudge, clientArea.top - nudge),
			D2D1::Point2F(clientArea.left + inset, clientArea.top + inset),
			D2D1::Point2F(clientArea.left - nudge, clientArea.bottom + nudge),
			D2D1::Point2F(clientArea.left + inset, clientArea.bottom - inset),
			D2D1::Point2F(clientArea.right + nudge, clientArea.bottom + nudge),
			D2D1::Point2F(clientArea.right - inset, clientArea.bottom - inset),
			D2D1::Point2F(clientArea.right + nudge, clientArea.top - nudge),
			D2D1::Point2F(clientArea.right - inset, clientArea.top + inset)
		};

		/* at four edges, we cannot have more than two 
		 * disjoint side bar geometries */
		Microsoft::WRL::ComPtr<ID2D1PathGeometry> borderGeometry;
		Microsoft::WRL::ComPtr<ID2D1GeometrySink> borderSink;
		DX::ThrowIfFailed
		(
			d2dFactory->CreatePathGeometry(borderGeometry.GetAddressOf())
		);

		DX::ThrowIfFailed
		(
			borderGeometry->Open(borderSink.GetAddressOf())
		);

		auto coalescingCase = ElementBase::EdgeCoalescingCaseFrom(drawSides);
		switch (coalescingCase)
		{
		case ElementBase::EdgeCoalescingCases::Plenty:
			borderSink->BeginFigure(points[6], D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_FILLED);
			borderSink->AddLine(points[0]);
			borderSink->AddLine(points[2]);
			borderSink->AddLine(points[4]);
			borderSink->AddLine(points[6]);
			borderSink->AddLine(points[7]);
			borderSink->AddLine(points[5]);
			borderSink->AddLine(points[3]);
			borderSink->AddLine(points[1]);
			borderSink->AddLine(points[7]);
			break;

		case ElementBase::EdgeCoalescingCases::TripletNES:
			borderSink->BeginFigure(points[2], D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_FILLED);
			borderSink->AddLine(points[4]);
			borderSink->AddLine(points[6]);
			borderSink->AddLine(points[0]);
			borderSink->AddLine(points[1]);
			borderSink->AddLine(points[7]);
			borderSink->AddLine(points[5]);
			borderSink->AddLine(points[3]);
			break;

		case ElementBase::EdgeCoalescingCases::TripletNWS:
			borderSink->BeginFigure(points[6], D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_FILLED);
			borderSink->AddLine(points[0]);
			borderSink->AddLine(points[2]);
			borderSink->AddLine(points[4]);
			borderSink->AddLine(points[5]);
			borderSink->AddLine(points[3]);
			borderSink->AddLine(points[1]);
			borderSink->AddLine(points[7]);
			break;

		case ElementBase::EdgeCoalescingCases::TripletWNE:
			borderSink->BeginFigure(points[4], D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_FILLED);
			borderSink->AddLine(points[6]);
			borderSink->AddLine(points[0]);
			borderSink->AddLine(points[2]);
			borderSink->AddLine(points[3]);
			borderSink->AddLine(points[1]);
			borderSink->AddLine(points[7]);
			borderSink->AddLine(points[5]);
			break;

		case ElementBase::EdgeCoalescingCases::TripletWSE:
			borderSink->BeginFigure(points[0], D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_FILLED);
			borderSink->AddLine(points[2]);
			borderSink->AddLine(points[4]);
			borderSink->AddLine(points[6]);
			borderSink->AddLine(points[7]);
			borderSink->AddLine(points[5]);
			borderSink->AddLine(points[3]);
			borderSink->AddLine(points[1]);
			break;

		case ElementBase::EdgeCoalescingCases::TwinSW:
			borderSink->BeginFigure(points[0], D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_FILLED);
			borderSink->AddLine(points[2]);
			borderSink->AddLine(points[4]);
			borderSink->AddLine(points[5]);
			borderSink->AddLine(points[3]);
			borderSink->AddLine(points[1]);
			break;

		case ElementBase::EdgeCoalescingCases::TwinSE:
			borderSink->BeginFigure(points[6], D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_FILLED);
			borderSink->AddLine(points[7]);
			borderSink->AddLine(points[5]);
			borderSink->AddLine(points[3]);
			borderSink->AddLine(points[2]);
			borderSink->AddLine(points[0]);
			break;

		case ElementBase::EdgeCoalescingCases::TwinNW:
			borderSink->BeginFigure(points[0], D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_FILLED);
			borderSink->AddLine(points[2]);
			borderSink->AddLine(points[3]);
			borderSink->AddLine(points[1]);
			borderSink->AddLine(points[7]);
			borderSink->AddLine(points[6]);
			break;

		case ElementBase::EdgeCoalescingCases::TwinNE:
			borderSink->BeginFigure(points[0], D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_FILLED);
			borderSink->AddLine(points[1]);
			borderSink->AddLine(points[7]);
			borderSink->AddLine(points[5]);
			borderSink->AddLine(points[4]);
			borderSink->AddLine(points[6]);
			break;

		default:
			if (coalescingCase & ElementBase::EdgeCoalescingCases::SingleEast)
			{
				borderSink->BeginFigure(points[6], D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_FILLED);
				borderSink->AddLine(points[7]);
				borderSink->AddLine(points[5]);
				borderSink->AddLine(points[4]);
			}

			if (coalescingCase & ElementBase::EdgeCoalescingCases::SingleWest)
			{
				borderSink->BeginFigure(points[0], D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_FILLED);
				borderSink->AddLine(points[2]);
				borderSink->AddLine(points[3]);
				borderSink->AddLine(points[1]);
			}

			if (coalescingCase & ElementBase::EdgeCoalescingCases::SingleSouth)
			{
				borderSink->BeginFigure(points[2], D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_FILLED);
				borderSink->AddLine(points[4]);
				borderSink->AddLine(points[5]);
				borderSink->AddLine(points[3]);
			}

			if (coalescingCase & ElementBase::EdgeCoalescingCases::SingleNorth)
			{
				borderSink->BeginFigure(points[0], D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_FILLED);
				borderSink->AddLine(points[1]);
				borderSink->AddLine(points[7]);
				borderSink->AddLine(points[6]);
			}

			break;
		}

		borderSink->EndFigure(D2D1_FIGURE_END::D2D1_FIGURE_END_CLOSED);
		DX::ThrowIfFailed
		(
			borderSink->Close()
		);

		/* the plenty case is the only topology with a hole */
		if (coalescingCase == ElementBase::EdgeCoalescingCases::Plenty)
		{
			borderGeometry->Outline(nullptr, borderSink.Get());
		}

		/* realize the bars */
		d2dContext->PushAxisAlignedClip(clientArea, D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
		d2dContext->FillGeometry(borderGeometry.Get(), backBrush.Get());
		d2dContext->DrawGeometry(borderGeometry.Get(), borderBrush.Get());	
		d2dContext->PopAxisAlignedClip();
	}	
}
