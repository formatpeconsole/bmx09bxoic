#pragma once
#include <functional>

template<typename T, typename U, typename V>
class RAIIItemSet
{
public:
    RAIIItemSet(T* i, U s1, V s2) : item(i), state1(s1), state2(s2) { *item = state1; }
    ~RAIIItemSet() { *item = state2; }

private:
    T* item;
    U state1;
    V state2;
};

class RAIICallbacks
{
public:
    RAIICallbacks(std::function<void()>&& f1, std::function<void()>&& f2)
        : fn1(std::move(f1)), fn2(std::move(f2)) 
    {
        fn1();
    }

    ~RAIICallbacks()
    { 
        fn2(); 
    }

private:
    std::function<void()> fn1;
    std::function<void()> fn2;
};

#define SCOPE_SET(item, state1, state2) RAIIItemSet<decltype(item), decltype(state1), decltype(state2)> set##item{&item, state1, state2};
#define SCOPE_CALLBACKS(name, fn1, fn2) RAIICallbacks callback##name{fn1, fn2};