# tbci
## Test Base Class Injection

or, How to mock ***anything*** in C++.

I often need to mock up the slow parts of my code, code with random numbers, system calls or similar. But I hate to change my code drastically just to make it testable. And that's where TBCI comes in.

Here's an example [ExampleUsage.h](ExampleUsage.h) and [ExampleUsage.cpp](ExampleUsage.cpp)

### Why it works:
TBCI works because of how name resolution works in C++.  First I'll describe how name resolution works in classes, and then I'll describe how class templates are slightly different.

Suppose you have a method named ```Foo()``` that calls a method on a class object ```Bar().DoSomething()``` (it's the ```Bar::DoSomething``` method that we need to mock up).

Now, when the compiler sees a ```Bar``` type, it says to itself, "Is there a class definition of ```Bar``` nested inside my current class?"  If there is, it uses that type.
But if there isn't, it next looks in any base classes to see if there is a ```Bar``` defined there, and if so, it uses that.
If it still hasn't found one in the base classes, it looks in the current namespace, and failing that, its final attempt is in the global namespace.

Now, template class's name resolution is similar with one exception. The compiler does two-phase lookup in templates; first it looks through all the template code and if it sees a name that is *not* dependent on any template parameter types, it "binds" to it.
If, on the other hand, a type *does* depend on a template parameter type, then it waits for the second phase to bind to it, the second phase being when the template parameter types are known.

To make sure our mocked up type gets picked up, we need to add a ```using typename Base::IWantToMockThis;``` to our template class and make sure the definition is pulled into the base class via ```using IWantToMockThis = ::IWantToMockThis;```.

That'll do it. It works not only on automatic variables, but also on arguments, return values, data-members and C calls.

### Mocking C APIs:

The same concept applies to mocking C APIs, but rather than needing a ```using typename Base::IWantToMockThis;```, we need a ```using SomeAPICall;``` (N.B.:  note the lack of ```typename```).
Also, rather than pulling the definition of the type into the base class, we need a little forwarding function. It's a one-liner, but those get tedious to write, so I've supplied a macro that forwards
to any C call in the global namespace, like this:

```#define MOCKABLE_FUNCTION(NAME) template <typename... Args> static inline decltype(auto) NAME(Args&&... args) { return ::NAME(std::forward<Args>(args)...); }```

Of course, you can write them by hand, if you prefer. One final note, since C++ matches functions by name, rather than by signature, you can write your test code like this:
```
   struct TestBase
   {
       HRESULT CoCreateInstance(...) { return E_FAIL; } // note use of ...
   };
```
and the compiler will use it preferentially over a better match signature-wise, if it finds it in the base class.

Here are the relevant examples:  [MockingCApi.h](MockingCApi.h) and [MockingCApi.cpp](MockingCApi.cpp).  And the ```MOCKABLE_FUNCTION``` macro header:  [MockableFunction.h](MockableFunction.h).


That should do it.  Happy mocking!
