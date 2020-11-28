#include "..\PreCompiledHeaders.h"
#include "UserInterface.h"
#include "..\dx\DirectXHelper.h"

#include "DialogEmbedded.h"

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace D2D1;

DialogEmbedded::DialogEmbedded()
{
	this->m_visible = false;
}

void DialogEmbedded::Initialize(std::shared_ptr<BrushRegistry> brushRegistry)
{
	ElementBase::Initialize(brushRegistry);
	auto d2dContext = UserInterface::GetD2DContext();


}

void DialogEmbedded::ScheduleDialogCommand(blooDot::DialogCommand dialogCommand)
{
	if (this->m_pendingDialogCommand == blooDot::DialogCommand::None)
	{
		this->m_pendingDialogCommand = dialogCommand;
	}
}

blooDot::DialogCommand DialogEmbedded::DequeDialogCommand()
{
	auto pendingCommand = this->m_pendingDialogCommand;
	if (pendingCommand != blooDot::DialogCommand::None)
	{
		this->m_pendingDialogCommand = blooDot::DialogCommand::None;
	}

	return pendingCommand;
}

void DialogEmbedded::Update(float timeTotal, float timeDelta)
{
	ElementBase::Update(timeTotal, timeDelta);


}

void DialogEmbedded::Render()
{
	ElementBase::Render();
	auto d2dContext = UserInterface::GetD2DContext();
	auto screenSize = d2dContext->GetSize();
	auto screenRect = D2D1::RectF(0.0F, 0.0F, screenSize.width - 1.0F, screenSize.height - 1.0F);


}

void DialogEmbedded::ReleaseDeviceDependentResources()
{


	ElementBase::ReleaseDeviceDependentResources();
}