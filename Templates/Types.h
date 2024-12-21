#pragma once
#include <iostream>
#include <sstream>
#include <bits/stdc++.h>
#include <climits>
#include <concepts>


// float - base type from c++
// Float - one of abstract types for imitate task
// FLOAT - how user defines it
// don`t confuse
using Float = float;

// double - base type from c++
// Double - one of abstract types for imitate task
// DOUBLE - how user defines it
// don`t confuse
using Double = double;


template <size_t N, size_t M, bool IsFast>
class BaseFixed {
public:
    using value_type = std::conditional_t<IsFast, 

                    // If it is fast version
                    std::conditional_t<N <= 8, int_fast8_t, 
                    std::conditional_t<N <= 16, int_fast16_t, 
                    std::conditional_t<N <= 32, int_fast32_t, int_fast64_t>>>,

                    // If it is not
                    std::conditional_t<N == 8, int8_t,  
                    std::conditional_t<N == 16, int16_t, 
                    std::conditional_t<N == 32, int32_t, int64_t>>>>;

public:
    static constexpr BaseFixed from_raw(value_type x) {
        BaseFixed ret;
        ret.v = x;
        return ret;
    } 

    value_type v;

    // bool operator< (const BaseFixed& other) const { return v <  other.v; }
    // bool operator> (const BaseFixed& other) const { return v >  other.v; }
    // bool operator<=(const BaseFixed& other) const { return v <= other.v; }
    // bool operator>=(const BaseFixed& other) const { return v >= other.v; }
    // bool operator==(const BaseFixed& other) const { return v == other.v; }
    // bool operator!=(const BaseFixed& other) const { return v != other.v; }

public:
    double to_double() const
    {
        return v / (double) (static_cast<int64_t>(1) << M);
    }

public:
    template <std::integral U>
    BaseFixed(U val): v(static_cast<value_type>(val) << M) {}

    BaseFixed(float val): v((static_cast<int64_t>(1) << M) * val) {}
    BaseFixed(double val): v((static_cast<int64_t>(1) << M) * val) {}

    template <size_t N1, size_t M1, bool IsFast1>
    BaseFixed(const BaseFixed<N1, M1, IsFast1> &bf): BaseFixed(bf.to_double()) { }
    
    BaseFixed(): BaseFixed(0) {}

    // BaseFixed operator+(const BaseFixed &b) {
    //     return BaseFixed::from_raw(v + b.v);
    // }

    // BaseFixed operator-(const BaseFixed &b) {
    //     return BaseFixed::from_raw(v - b.v);
    // }

    // BaseFixed operator*(BaseFixed b)
    // {
    //     return BaseFixed(to_double() * b.to_double());
    // }

    // BaseFixed operator/(BaseFixed b) {
    //     return BaseFixed(to_double() / b.to_double());
    // }

    BaseFixed &operator+=(BaseFixed b) {
        v += b.v;
        return *this;
    }

    BaseFixed &operator-=(BaseFixed b) {
        v -= b.v;
        return *this;
    }

    BaseFixed &operator*=(BaseFixed b) {
        v = (static_cast<int64_t>(v) * b.v) >> M;
        return *this;
    }

    BaseFixed &operator/=(BaseFixed b) {
        v = (static_cast<int64_t>(v) << M) / b.v;
        return *this;
    }

    BaseFixed operator-() {
        return BaseFixed::from_raw(-v);
    }

    operator double() const
    {
        return to_double();
    }

    friend std::ostream &operator<<(std::ostream &os, const BaseFixed &x)
    {
        return os << x.to_double();
    }

    friend bool min(const BaseFixed &lhs, const Double &rhs)
    {
        return lhs.to_double() < rhs;
    }
};

template<typename T, typename U>
void swap(T &lhs, U &rhs)
{
    U temp = rhs;
    rhs = U(lhs);
    lhs = T(temp);
}

template <size_t N, size_t M, bool IsFast>
BaseFixed<N, M, IsFast> abs(BaseFixed<N, M, IsFast> x) {
    if (x.v < 0) {
        x.v = -x.v;
    }
    return x;
}

template <size_t N, size_t M>
requires (M <= N && ( (N == 8) || (N == 16) || (N == 32) || (N == 64) ))
using Fixed = BaseFixed<N, M, false>; 

template <size_t N, size_t M> 
requires (M <= N && ( N <= 64 )) 
using FastFixed = BaseFixed<N, M, true>; 

