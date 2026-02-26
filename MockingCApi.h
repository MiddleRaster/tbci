#pragma once
#ifndef MOCKINGCAPI_U
#define MOCKINGCAPI_U

#include <windows.h>
#include <combaseapi.h>

// CLSID for Shell's File Open Dialog
inline constexpr CLSID CLSID_FileOpenDialog = { 0xDC1C5A9C,0xE88A,0x4DDE,{0xA5,0xA1,0x60,0xF8,0x2A,0x20,0xAE,0xF7} };

/* original
class TheProductionCode
{
public:
    HRESULT GetSomeCOMInterface(IUnknown*& punk)
    {
        return CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&punk);
    }
};
*/

#include "MockableFunction.h"

struct Empty
{
    Callable<::CoCreateInstance> CoCreateInstance;
    // or MOCKABLE_FUNCTION(CoCreateInstance);
};

template<typename Base>
class TheProductionCodeT : private Base
{
    using Base::CoCreateInstance;
public:
    HRESULT GetSomeCOMInterface(IUnknown*& punk)
    {
        return CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&punk);
    }
};
using TheProductionCode = TheProductionCodeT<Empty>; // so all existing client code still compiles

#endif
