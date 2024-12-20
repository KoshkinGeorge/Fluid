#pragma once

#include <iostream>

#include "./TypeInfo.h"
#include "./Imitation.h"

/*
This Compile-Time Switch construction won't work is the macroses TYPES and SIZES are not defined or defined incorrectly
*/

#define FIXED(N, M) Fixed<N, M>
#define FAST_FIXED(N, M) FastFixed<N, M>
#define FLOAT Float
#define DOUBLE Double
#define S(N, M) Window(N, M)


struct Window
{
public:
    size_t rows;
    size_t cols;
};

template <typename P_type, typename V_type, typename VF_type,
Window head, Window ...tail>
class SizeSwitch
{
private:
    std::vector<std::vector<char>> field;
public:
    SizeSwitch(std::vector<std::vector<char>> &&_field):
    field(std::move(_field)) {}

    void match_size(size_t rows, size_t cols)
    {
        if (rows == head.rows && cols == head.cols)
        {
            Imitation<P_type, V_type, VF_type, head.rows, head.cols> i(std::move(field));
            i.imitate();
        }
        else if constexpr (sizeof...(tail) == 0)
        {
            Imitation<P_type, V_type, VF_type> i(std::move(field));
            i.imitate();
        }
        else
        {
            SizeSwitch<P_type, V_type, VF_type, tail...>(std::move(field)).match_size(rows, cols);
        }
    }
};




template <size_t ArgsLeft, typename Head, typename ...Tail>
class TypeSwitch;

template <typename Head, typename ...Tail>
class TypeSwitch<1, Head, Tail...>
{
private:
    size_t height, width;
    std::vector<std::vector<char>> field;
public:
    TypeSwitch(size_t _height, size_t _width, std::vector<std::vector<char>> &&_field): height(_height), width(_width), field(std::move(_field)) {}

    template <typename P_type, typename V_type>
    void match_type(const std::string &type3)
    {
        if (TypeInfo<Head>::name() == type3)
        {
            #ifdef DEBUG
            std::cout << "\t\tVF_type = " << type3 << '\n';
            std::cout << "\t\t\t\t I got 3 types: " << TypeInfo<P_type>::name() << ' ' << TypeInfo<V_type>::name() << ' ' << type3 << '\n' ;
            #endif
            // found such types
            #ifdef SIZES
            SizeSwitch<P_type, V_type, Head, SIZES>(std::move(field)).match_size(height, width);
            #else
            std::cerr << "SIZES macros is not found\n";
            #endif
        }
        else if constexpr (sizeof...(Tail) == 0)
        {
            #ifdef DEBUG
            std::cout << "VF_type: " << TypeInfo<Head>::name() << " - not what i am looking for\n";
            #endif
            std::cerr << "Type " << type3 << " was not listed in pre-compile type list\n";
        }
        else
        {
            #ifdef DEBUG
            std::cout << "VF_type: " << TypeInfo<Head>::name() << " - not what i am looking for\n";
            #endif
            TypeSwitch<1, Tail...>(height, width, std::move(field)).template match_type<P_type, V_type>(type3);
            return;
        }
    }
};

template <typename Head, typename ...Tail>
class TypeSwitch<2, Head, Tail...>
{
private:
    size_t height, width;
    std::vector<std::vector<char>> field;
public:
    TypeSwitch(size_t _height, size_t _width, std::vector<std::vector<char>> &&_field): height(_height), width(_width), field(std::move(_field)) {}

    template <typename P_type>
    void match_type(const std::string &type2, const std::string &type3)
    {
        if (TypeInfo<Head>::name() == type2)
        {
            #ifdef DEBUG
            std::cout << "\t\tV_type = " << type2 << '\n';
            #endif 
            TypeSwitch<1, TYPES>(height, width, std::move(field)).template match_type<P_type, Head>(type3);
            return;
        }
        else if constexpr (sizeof...(Tail) == 0)
        {
            #ifdef DEBUG
            std::cout << "V_type: " << TypeInfo<Head>::name() << " - not what i am looking for\n";
            #endif
            std::cerr << "Type " << type2 << " was not listed in pre-compile type list\n";
        }
        else
        {
            #ifdef DEBUG
            std::cout << "V_type: " << TypeInfo<Head>::name() << " - not what i am looking for\n";
            #endif
            TypeSwitch<2, Tail...>(height, width, std::move(field)).template match_type<P_type>(type2, type3);
            return;
        }
    }
};

template <typename Head, typename ...Tail>
class TypeSwitch<3, Head, Tail...>
{
private:
    size_t height, width;
    std::vector<std::vector<char>> field;
public:
    TypeSwitch(size_t _height, size_t _width, std::vector<std::vector<char>> &&_field): height(_height), width(_width), field(std::move(_field)) {}

    void match_type(const std::string &type1, const std::string &type2,
    const std::string &type3)
    {
        if (TypeInfo<Head>::name() == type1)
        {
            #ifdef DEBUG
            std::cout << "\t\tP_type = " << type1 << '\n'; 
            #endif
            #ifdef TYPES
            TypeSwitch<2, TYPES>(height, width, std::move(field)).match_type<Head>(type2, type3);
            return;
            #else
            std::cerr << "TYPES macros is not found\n";
            #endif
        }
        else if constexpr (sizeof...(Tail) == 0)
        {
            #ifdef DEBUG
            std::cout << "P_type: " << TypeInfo<Head>::name() << " - not what i am looking for\n";
            #endif
            std::cerr << "Type " << type1 << " was not listed in pre-compile type list\n";
        }
        else
        {
            #ifdef DEBUG
            std::cout << "P_type: " << TypeInfo<Head>::name() << " - not what i am looking for\n";
            #endif
            TypeSwitch<3, Tail...>(height, width, std::move(field)).match_type(type1, type2, type3);
            return;
        }
    }
};

class Binder
{
public:
    void bind(const std::string &type1, const std::string &type2, const std::string &type3, size_t height, size_t width, 
    std::vector<std::vector<char>> &&field)
    {
        #ifdef TYPES
        TypeSwitch<3, TYPES> ts(height, width, std::move(field));
        ts.match_type(type1, type2, type3);
        #endif
    }
};