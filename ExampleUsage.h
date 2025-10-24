#pragma once
#ifndef EXAMPLEUSAGE_H
#define EXAMPLEUSAGE_H

// the dependency
struct IWantToMockThis
{
    bool ImagineThisIsReallySlow() { return true; }
};

/* the original production code before TBCI
struct ProductionCode
{
    static bool Foo()
    {
        IWantToMockThis obj;
        return obj.ImagineThisIsReallySlow();
    }
};
*/

struct Empty
{
    using IWantToMockThis = ::IWantToMockThis;
};

template<typename Base>
struct ProductionCodeT : private Base
{
    using typename Base::IWantToMockThis;

    static bool Foo()
    {
        IWantToMockThis obj;
        return obj.ImagineThisIsReallySlow();
    }
};

using ProductionCode = ProductionCodeT<Empty>; // so all existing client code still compiles

#endif
