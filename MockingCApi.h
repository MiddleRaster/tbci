#pragma once
#ifndef MOCKINGCAPI_U
#define MOCKINGCAPI_U

#include <windows.h>

// CLSID for MSXML 6.0 DOMDocument
inline constexpr CLSID CLSID_DOMDocument60 = { 0x88D969C0,0xF192,0x11D4,{0xA6,0x5F,0x00,0x40,0x96,0x32,0x51,0xE5} };
/*
class TheProductionCode
{
public:
    HRESULT GetSomeCOMInterface(IUnknown*& punk)
    {
        return CoCreateInstance(CLSID_DOMDocument60, nullptr, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&punk);
    }
};
*/

#include "MockableFunction.h"

struct Empty
{
    MOCKABLE_FUNCTION(CoCreateInstance)
};

template<typename Base>
class TheProductionCodeT : private Base
{
    using Base::CoCreateInstance;
public:
    HRESULT GetSomeCOMInterface(IUnknown*& punk)
    {
        return CoCreateInstance(CLSID_DOMDocument60, nullptr, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&punk);
    }
};
using TheProductionCode = TheProductionCodeT<Empty>; // so all existing client code still compiles

#endif