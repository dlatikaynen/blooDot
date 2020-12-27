#include "pch.h"
#include "CppUnitTest.h"
#include "..\src\algo\Topology.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace blooDot_test
{
    TEST_CLASS(TestTopology)
    {
    public:
        TEST_METHOD(TestTopologyEdges)
        {
            Assert::AreEqual<int>(blooDot::Topology::D2EdgePlenty[0], 6, L"Topology broken", LINE_INFO());
        }
    };
}