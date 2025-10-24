#include "CppUnitTest.h"

#include "MockingCApi.h"

namespace ExampleUsageOfTBCI
{
    using namespace Microsoft::VisualStudio::CppUnitTestFramework;

    TEST_CLASS(MockingCApiTests)
    {
        TEST_CLASS_INITIALIZE(Init) { ::CoInitialize(NULL); }
        TEST_CLASS_CLEANUP(Cleanup) { ::CoUninitialize(); }

        TEST_METHOD(OriginalCode)
        {
            TheProductionCode obj;
            IUnknown* punk = nullptr;
            HRESULT hr = obj.GetSomeCOMInterface(punk);
            if (punk)
                punk->Release(); // if only I could use an ATL smart pointer here...

            Assert::IsNotNull(punk);
            Assert::AreEqual(S_OK, hr);
        }

        TEST_METHOD(RefactoredForTBCI)
        {
            struct TestBase
            {
                HRESULT CoCreateInstance(...) { return E_FAIL; } // note use of ...
            };
            IUnknown* punk = nullptr;
            Assert::AreEqual(E_FAIL, TheProductionCodeT<TestBase>().GetSomeCOMInterface(punk));
            Assert::IsNull(punk);
        }
    };

}
