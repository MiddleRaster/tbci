# tbci
## Test Base Class Injection

or, How to mock ***anything*** in C++.

I often need to mock up the slow parts of my code, make nondeterministic code deterministric, mock system calls or just mock out a dependency. But I hate to change my code drastically just to make it testable. And that's where TBCI comes in.

Wouldn't it be great if we could write code and tests like this?
```cpp
// the dependency
struct IWantToMockThis
{
    bool ImagineThisIsReallySlow() { return true; }
};

// the source code
struct ProductionCode
{
    static bool Foo()
    {
        IWantToMockThis obj;
        return obj.ImagineThisIsReallySlow();
    }
};

// the test code
TEST_CLASS(ExampleUsageTests)
{
    TEST_METHOD(OriginalUnmockedVersion)
    {
        Assert::IsTrue(ProductionCode::Foo());
    }
    TEST_METHOD(UsingTBCI)
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
```
TBCI allows you to do so. If you'd like to build and step through this example yourself, see these files: [ExampleUsage.h](ExampleUsage.h) and [ExampleUsage.cpp](ExampleUsage.cpp)


### How TBCI works
TBCI works because of how name resolution works in C++. If you have a type in your class and you need to mock/fake/stub it, with judicious placement of a new definition, you can change its definition.
The C++ compiler does name resolution in the following order:
1. First, it looks for a type with the same name as the type you want to mock (e.g., ```IWantToMockThis``` from above), defined nested in the class itself.  If it finds one, it stops searching.
2. However, if there's no such type, it looks in any base classes (and their bases), and if it finds one it stops looking.
3. But if it still hasn't found one, it starts looking in the current namespace, and then other namespaces you've specified through any ```using namespace``` statements.
4. The final place it looks is in the global namespace.  If it still hasn't found one, you'll get an error about a symbol not found or similar.

Now, TBCI uses #2. We mock up a type and put it in a base class and make our class derive off of it. To make that work requires a little refactoring, but not too much. Take the ```ProductionCode``` struct above and refactor it like this:
```cpp
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
```

So far, nothing has really changed:  all the clients can continue to call ```ProductionCode::Foo()``` and everything will compile just like before.

However, in our test code, we can replace the base class, ```Empty```, with our own new test base class, ```TestBase``` and put our newly defined mock in there. Then we pass ```TestBase``` as the template parameter to ```ProductionCodeT``` and it becomes the base class, in which, when the compiler is doing a name resolution search, it will find our mock and it stops looking further (i.e., in any namespaces or deeper base classes).  Voila; our mock is used.


One further note:  when compiling a class template, the compiler does two passes.  In the first pass, it "binds" to any non-dependent names it finds.  So, in our original code, ```IWantToMockThis``` is a non-dependent name and it's found and bound during the first pass. The second pass binds any names that do depend on the template parameter type, and this is what we actually want.  So, we need to change the non-dependent name into a dependent name.  And that's done with ```using typename Base::IWantToMockThis;``` and for that line to compile means we need to pull a definition of ```IWantToMockThis``` into the default base class, ```Empty```, which is what ```using IWantToMockThis = ::IWantToMockThis;``` does.

These two ```using``` statements are the cost of TBCI and they're not even executable; they're just directives to the compiler on where the definitions live.

This technique works not only on types used as automatic variables, but also return types, data-member types, argument types, you name it.  However, to mock a C function is similar, but slightly different.


### Mocking C APIs:

The same concept applies to mocking C APIs, but rather than needing a ```using typename Base::IWantToMockThis;```, we need a ```using Base::SomeAPICall;``` (N.B.:  note the lack of ```typename```).
Also, rather than pulling the definition of the type into the base class, we need a little forwarding function. It's a one-liner, but those get tedious to write, so I've supplied a macro that forwards
to any C call in the global namespace, like this:

```#define MOCKABLE_FUNCTION(NAME) template <typename... Args> static inline decltype(auto) NAME(Args&&... args) { return ::NAME(std::forward<Args>(args)...); }```

Of course, you can write them by hand, if you prefer. One final note: since C++ matches functions by name, rather than by signature, you can even write your test code like this:
```cpp
   struct TestBase
   {
       HRESULT CoCreateInstance(...) { return E_FAIL; } // note use of ...
   };
```
and the compiler will use it preferentially over a better match signature-wise, since it finds it in the base class first, before finding it in the global namespace.

Here are the relevant examples:  [MockingCApi.h](MockingCApi.h) and [MockingCApi.cpp](MockingCApi.cpp).  And the ```MOCKABLE_FUNCTION``` macro header:  [MockableFunction.h](MockableFunction.h).


### Final words

You can find my materials from when I gave presentation at Agile 2012, [here](https://www.agilealliance.org/wp-content/uploads/2016/01/PrintOuts.cpp_.pdf). They contain a few more examples, kernel mode, for instance.


An objection to this technique is that you'd have to move all of this templated code into the headers. This is true, but lots of code written nowadays is header-only. I personally have been writing all my production code header-only since 2003 (way before I invented TBCI), but for a completely different reason:  in order to get DAG-only code. You can read about that [here](https://middleraster.github.io/DAG/HeaderOnlyNoForwardDeclarations.html).


So to recap:  I like to mock this way because I've touched my code and architecture only lightly:  I templatize my class with a base class that has a one-liner or two, and provide a ```using``` statement so that all existing clients of my code still compile.

That should do it.  Happy mocking!
