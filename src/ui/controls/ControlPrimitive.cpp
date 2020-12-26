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
	ElementBase::Directions drawSides,
	D2D1_POINT_2F scrollDirectionVector
)
{
	if (drawSides != ElementBase::Directions::NONE)
	{
		auto borderBrush = brushRegistry->WannaHave(d2dContext, UserInterface::Color(D2D1::ColorF::WhiteSmoke, 196));
		Microsoft::WRL::ComPtr<ID2D1Brush> backBrush;
		auto indicateDirection = scrollDirectionVector.x != 0.f || scrollDirectionVector.y != 0.f;
		if (indicateDirection)
		{
			/* flexing skill, filling opacity follows the scroll direction */
			auto gradientBrush = brushRegistry->Rather(d2dContext, UserInterface::Color(D2D1::ColorF::WhiteSmoke, 48), 148);
			D2D1_POINT_2F startPoint;
			D2D1_POINT_2F endinPoint;
			ControlPrimitive::ProjectLineOnFrame(clientArea, scrollDirectionVector, &startPoint, &endinPoint);
			gradientBrush->SetStartPoint(startPoint);
			gradientBrush->SetEndPoint(endinPoint);
			gradientBrush->QueryInterface<ID2D1Brush>(&backBrush);
#ifdef _DEBUG
			d2dContext->DrawLine(startPoint, endinPoint, borderBrush.Get());
#endif
		}
		else
		{
			backBrush = brushRegistry->WannaHave(d2dContext, UserInterface::Color(D2D1::ColorF::WhiteSmoke, 48));
		}

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
			DX::ThrowIfFailed
			(
				secondarySink->Close()
			);

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

void ControlPrimitive::ProjectLineOnFrame(D2D1_RECT_F frame, D2D1_POINT_2F linedirection, D2D1_POINT_2F* intersection1, D2D1_POINT_2F* intersection2)
{
	D2D1_POINT_2F i1, i2;
	auto a = -linedirection.y;
	auto b = linedirection.x;
	bool done = false;
	/* center the frame and keep offsets */
	auto halfWidth = (frame.right - frame.left) / 2.f;
	auto halfHeight = (frame.bottom - frame.top) / 2.f;
	auto centeredFrame = D2D1::RectF(-halfWidth, -halfHeight, halfWidth, halfHeight);
	/* solve the equations;
	 * do we have a point in the west? */
	if (b == 0.f)
	{
		/* horizontal edge case */
		i1.x = centeredFrame.left;
		i2.x = centeredFrame.right;
		i1.y = i2.y = 0;
	}
	else if (a == 0.f)
	{
		/* vertical case */
		i1.y = centeredFrame.top;
		i2.y = centeredFrame.bottom;
		i1.x = i2.x = 0;
	}

	if(b != 0.f)
	{
		auto westY = -(a * centeredFrame.left) / b;
		if (westY >= centeredFrame.top && westY <= centeredFrame.bottom)
		{
			i1.x = centeredFrame.left;
			i1.y = westY;
			i2.x = centeredFrame.right;
			i2.y = -westY;
			done = true;
		}
	}
	
	if (!done && a != 0.f)
	{
		auto southX = -(b * centeredFrame.bottom) / a;
		if (southX >= centeredFrame.left && southX <= centeredFrame.right)
		{
			i1.x = -southX;
			i1.y = centeredFrame.top;
			i2.x = southX;
			i2.y = centeredFrame.bottom;
		}
		else
		{
			assert(false);
		}
	}

	/* deliver in caller's coordinate system */
	(*intersection1).x = frame.left + halfWidth + i1.x;
	(*intersection1).y = frame.top + halfHeight + i1.y;
	(*intersection2).x = frame.left + halfWidth + i2.x;
	(*intersection2).y = frame.top + halfHeight + i2.y;
}
