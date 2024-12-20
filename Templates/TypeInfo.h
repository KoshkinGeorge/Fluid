#pragma once

#include "./Types.h"
#include "Contains.h"

template <typename T>
class TypeInfo
{
public:
    static std::string name()
    {
        return typeid(T).name();
    }
};

template <size_t N, size_t M>
class TypeInfo<Fixed<N, M>>
{
public:
    static std::string name()
    {
        std::ostringstream oss;
        oss << "FIXED(" << N << "," << M << ')';
        return oss.str();
    }
};

template <size_t N, size_t M>
class TypeInfo<FastFixed<N, M>>
{
public:
    static std::string name()
    {
        std::ostringstream oss;
        oss << "FAST_FIXED(" << N << "," << M << ')';
        return oss.str();
    }
};

template <>
class TypeInfo<Float>
{
public:
    static std::string name()
    {
        return "FLOAT";
    }
};

template <>
class TypeInfo<Double>
{
public:
    static std::string name()
    {
        return "DOUBLE";
    }
};