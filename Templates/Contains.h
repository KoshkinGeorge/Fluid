#pragma once

#include <concepts>

template <typename First, typename Second, typename ...Args>
class contains
{
public:
    static const bool value = std::is_same_v<First, Second> ||
    contains<First, Args...>::value;
};

template <typename First, typename Second>
class contains<First, Second>
{
public:
    static const bool value = std::is_same_v<First, Second>;
};

template <typename Head, typename ...Tail>
bool contains_v = contains<Head, Tail...>::value;


template <typename Head, typename ...Tail>
concept Contains = contains_v<Head, Tail...>;