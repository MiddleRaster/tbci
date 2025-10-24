#include "CppUnitTest.h"

#include "ExampleUsage.h"

namespace ExampleUsageOfTBCI
{
    using namespace Microsoft::VisualStudio::CppUnitTestFramework;

    TEST_CLASS(ExampleUsageTests)
    {
        TEST_METHOD(OriginalUnmockedVersion)
        {
            Assert::IsTrue(ProductionCode::Foo());
        }

        TEST_METHOD(RefactoredForTBCI)
        {
            struct TestBase
            {
                struct IWantToMockThis
                {
                    bool ImagineThisIsReallySlow() { return false; }
                };
            };
            Assert::IsFalse(ProductionCodeT<TestBase>::Foo());
        }
    };
}