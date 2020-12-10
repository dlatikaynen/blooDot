#include "..\..\PreCompiledHeaders.h"
#include "..\UserInterface.h"
#include "..\..\dx\DirectXHelper.h"
#include "..\..\algo\Topology.h"

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
		auto backBrush = brushRegistry->Rather(d2dContext, UserInterface::Color(D2D1::ColorF::WhiteSmoke, 48), 148);
		/* prepare all possible points, this is trivial */
		constexpr float nudge = 3.f;
		const float inset = blooDot::Consts::SQUARE_WIDTH * blooDot::Consts::INVERSE_GOLDEN_RATIO;
		const D2D1_POINT_2F points[] =
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

		auto openFigure = false;
		auto coalescingCase = ElementBase::EdgeCoalescingCaseFrom(drawSides);
		switch (coalescingCase)
		{
		case ElementBase::EdgeCoalescingCases::Plenty:
			ControlPrimitive::ConstructWedge(borderSink, points, &openFigure, std::size(blooDot::Topology::D2EdgePlenty), blooDot::Topology::D2EdgePlenty);
			break;

		case ElementBase::EdgeCoalescingCases::TripletNES:
			ControlPrimitive::ConstructWedge(borderSink, points, &openFigure, std::size(blooDot::Topology::D2TripletNES), blooDot::Topology::D2TripletNES);
			break;

		case ElementBase::EdgeCoalescingCases::TripletNWS:
			ControlPrimitive::ConstructWedge(borderSink, points, &openFigure, std::size(blooDot::Topology::D2TripletNWS), blooDot::Topology::D2TripletNWS);
			break;

		case ElementBase::EdgeCoalescingCases::TripletWNE:
			ControlPrimitive::ConstructWedge(borderSink, points, &openFigure, std::size(blooDot::Topology::D2TripletWNE), blooDot::Topology::D2TripletWNE);
			break;

		case ElementBase::EdgeCoalescingCases::TripletWSE:
			ControlPrimitive::ConstructWedge(borderSink, points, &openFigure, std::size(blooDot::Topology::D2TripletWSE), blooDot::Topology::D2TripletWSE);
			break;

		case ElementBase::EdgeCoalescingCases::TwinSW:
			ControlPrimitive::ConstructWedge(borderSink, points, &openFigure, std::size(blooDot::Topology::D2TwinSW), blooDot::Topology::D2TwinSW);
			break;

		case ElementBase::EdgeCoalescingCases::TwinSE:
			ControlPrimitive::ConstructWedge(borderSink, points, &openFigure, std::size(blooDot::Topology::D2TwinSE), blooDot::Topology::D2TwinSE);
			break;

		case ElementBase::EdgeCoalescingCases::TwinNW:
			ControlPrimitive::ConstructWedge(borderSink, points, &openFigure, std::size(blooDot::Topology::D2TwinNW), blooDot::Topology::D2TwinNW);
			break;

		case ElementBase::EdgeCoalescingCases::TwinNE:
			ControlPrimitive::ConstructWedge(borderSink, points, &openFigure, std::size(blooDot::Topology::D2TwinNE), blooDot::Topology::D2TwinNE);
			break;

		default:
			if (coalescingCase & ElementBase::EdgeCoalescingCases::SingleEast)
			{
				ControlPrimitive::ConstructWedge(borderSink, points, &openFigure, std::size(blooDot::Topology::D2SingleEast), blooDot::Topology::D2SingleEast);
			}

			if (coalescingCase & ElementBase::EdgeCoalescingCases::SingleWest)
			{
				ControlPrimitive::ConstructWedge(borderSink, points, &openFigure, std::size(blooDot::Topology::D2SingleWest), blooDot::Topology::D2SingleWest);
			}

			if (coalescingCase & ElementBase::EdgeCoalescingCases::SingleSouth)
			{
				ControlPrimitive::ConstructWedge(borderSink, points, &openFigure, std::size(blooDot::Topology::D2SingleSouth), blooDot::Topology::D2SingleSouth);
			}

			if (coalescingCase & ElementBase::EdgeCoalescingCases::SingleNorth)
			{
				ControlPrimitive::ConstructWedge(borderSink, points, &openFigure, std::size(blooDot::Topology::D2SingleNorth), blooDot::Topology::D2SingleNorth);
			}

			break;
		}

		borderSink->EndFigure(D2D1_FIGURE_END::D2D1_FIGURE_END_CLOSED);
		DX::ThrowIfFailed
		(
			borderSink->Close()
		);

		d2dContext->PushAxisAlignedClip(clientArea, D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

		/* the plenty case is the only topology with a hole */
		if (coalescingCase == ElementBase::EdgeCoalescingCases::Plenty)
		{
			Microsoft::WRL::ComPtr<ID2D1PathGeometry> secondaryGeometry;
			Microsoft::WRL::ComPtr<ID2D1GeometrySink> secondarySink;
			DX::ThrowIfFailed
			(
				d2dFactory->CreatePathGeometry(secondaryGeometry.GetAddressOf())
			);

			DX::ThrowIfFailed
			(
				secondaryGeometry->Open(secondarySink.GetAddressOf())
			);

			borderGeometry->Outline(nullptr, secondarySink.Get());
			borderGeometry.Reset();
			DX::ThrowIfFailed
			(
				secondarySink->Close()
			);

			/* as a flex, filling opacity follows the scroll direction */
			backBrush->SetStartPoint(D2D1::Point2F(0, 0));
			backBrush->SetEndPoint(D2D1::Point2F(640, 480));
			d2dContext->FillGeometry(secondaryGeometry.Get(), backBrush.Get());
			d2dContext->DrawGeometry(secondaryGeometry.Get(), borderBrush.Get());
		}
		else
		{
			d2dContext->FillGeometry(borderGeometry.Get(), backBrush.Get());
			d2dContext->DrawGeometry(borderGeometry.Get(), borderBrush.Get());
		}

		/* realize the bars */
		d2dContext->PopAxisAlignedClip();
	}
}

void ControlPrimitive::ConstructWedge(
	Microsoft::WRL::ComPtr<ID2D1GeometrySink> borderSink,
	const D2D1_POINT_2F* points,
	bool* openFigure,
	const size_t numPoints,
	const int* pointIndices
)
{
	if (*openFigure)
	{
		borderSink->EndFigure(D2D1_FIGURE_END::D2D1_FIGURE_END_CLOSED);
	}
	else
	{
		*openFigure = true;
	}

	borderSink->BeginFigure(points[pointIndices[0]], D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_FILLED);
	for (auto i = 1; i < numPoints; ++i)
	{
		borderSink->AddLine(points[pointIndices[i]]);
	}
}
