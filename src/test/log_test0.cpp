#include <string>
#include <iostream>
#include "format.h"
#include <stdio.h>
#include <vector>
#include <memory>
/*
int main()
{
    std::string str = fmt::format("Debug: The answer is {}.", 42);
    fmt::memory_buffer out;
    fmt::format_to(out, "for a moment, {} happend.\n", "nothing");
    fmt::format_to(out, "newLine\n");
    //out.data();
    fmt::print(" memory_buffer size: {}, capacity: {}\n", out.size(), out.capacity());
    if(out.size() > 30)
    {
        
    }
    printf("%s", out.data());
    
    std::cout << str << std::endl;
    fmt::print("Don't {}\n", "panic");
    return 0;
}*/



template <typename S, typename... Args>
void logfun(const S& format_str, Args&&... args)
{
    
    fmt::format(format_str, args...);
}


int main()
{
    std::vector<std::string> vstr(100);
    std::string str = fmt::format("Debug: The answer is {}.\n", 42);
    
    vstr[0] = std::move(str);
    fmt::print(vstr.front());
    fmt::print(str);
    std::shared_ptr<std::string> spStr = std::make_shared<std::string>("23523\n");

    std::cout << *spStr;
    logfun("feefwe{}\n", 32);
    
    return 0;
}