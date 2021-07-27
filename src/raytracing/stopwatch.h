#pragma once
#include <chrono>
#include <iostream>

struct NoCopy
{
    NoCopy() = default;
    NoCopy(NoCopy const&) = delete;
    NoCopy& operator=(NoCopy const&) = delete;
};

struct StopWatch : NoCopy
{
    StopWatch(char const* n)
        : start { std::chrono::high_resolution_clock::now() }
        , name { n }
    {
    }

    ~StopWatch()
    {
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - start;
        std::cerr << name << "<" << elapsed.count() << ">\n";
    }

    std::chrono::high_resolution_clock::time_point start;
    char const* name;
};