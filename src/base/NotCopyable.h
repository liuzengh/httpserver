// delete 关键词
// default 关键词 
/*
C++ 的类有四类特殊成员函数，它们分别是：默认构造函数、析构函数、拷贝构造函数以及拷贝赋值运算符。
这些类的特殊成员函数负责创建、初始化、销毁，或者拷贝类的对象。
如果程序员没有显式地为一个类定义某个特殊成员函数，而又需要用到该特殊成员函数时，则编译器会隐式的为这个类生成一个默认的特殊成员函数。
C++11 标准引入了一个新特性：defaulted 函数。程序员只需在函数声明后加上“=default;”，
就可将该函数声明为 defaulted 函数，编译器将为显式声明的 defaulted 函数自动生成函数体。
Defaulted 函数特性仅适用于类的特殊成员函数，且该特殊成员函数没有默认参数。
*/
#pragma once
class NotCopyable
{
    public:
        NotCopyable(const NotCopyable&) = delete;
        const NotCopyable& operator=(const NotCopyable&) = delete;
        
    protected:
        NotCopyable() = default;
        ~NotCopyable() = default;

};