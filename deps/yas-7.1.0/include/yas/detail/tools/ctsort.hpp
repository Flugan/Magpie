
// Copyright (c) 2010-2021 niXman (github dot nixman at pm dot me). All
// rights reserved.
//
// This file is part of YAS(https://github.com/niXman/yas) project.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//
//
// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#ifndef __yas__detail__tools__ctsort_hpp
#define __yas__detail__tools__ctsort_hpp

#include <type_traits>
#include <tuple>

namespace yas {
namespace detail {

/***************************************************************************/

template<typename, typename>
struct concat;

template<typename... Xs, typename... Ys>
struct concat<std::tuple<Xs...>, std::tuple<Ys...>> {
    using type = std::tuple<Xs..., Ys...>;
};

template<typename X, typename TPL>
struct split_accum_fst {
    using type = std::tuple<
        typename concat<
             std::tuple<X>
            ,typename std::tuple_element<0, TPL>::type
        >::type
        ,typename std::tuple_element<1,TPL>::type
    >;
};

template<typename, typename>
struct split_gather {};

template<
     typename X
    ,typename... Xs
    ,typename ignore1
    ,typename ignore2
    ,typename... Ys
>
struct split_gather<std::tuple<X, Xs...>, std::tuple<ignore1, ignore2, Ys...>>  {
    using type = typename split_accum_fst<
         X
        ,typename split_gather<
             std::tuple<Xs...>
            ,std::tuple<Ys...>
        >::type
    >::type;
};

template<typename ... Xs, typename ignore>
struct split_gather<std::tuple<Xs...>, ignore>  {
    using type = std::tuple<std::tuple<>, std::tuple<Xs...>>;
};

template<typename SEQ>
struct split {
    using type = typename split_gather<SEQ, SEQ>::type;
};

template<
     template<typename, typename> class PRED
    ,typename SEQ1
    ,typename SEQ2
>
struct merge {};

template<template<typename, typename> class PRED, typename SEQ>
struct merge<PRED, SEQ, std::tuple<>> {
    using type = SEQ;
};

template<template<typename, typename> class PRED, typename SEQ>
struct merge<PRED, std::tuple<>, SEQ> {
    using type = SEQ;
};

template<
     template<typename, typename> class PRED
    ,typename X
    ,typename... Xs
    ,typename Y
    ,typename... Ys
>
struct merge<PRED, std::tuple<X, Xs...>, std::tuple<Y, Ys...>>  {
    template<bool cond, typename T1, typename T2>
    struct merge_case { //case true
        using type = T1;
    };

    template<typename T1, typename T2>
    struct merge_case<0, T1, T2> { //case false
        using type = T2;
    };

    using type = typename merge_case<
         PRED<X, Y>::value
        ,typename concat<
             std::tuple<X>
            ,typename merge<
                 PRED
                ,std::tuple<Xs...>
                ,std::tuple<Y, Ys...>
            >::type
        >::type
        ,typename concat<
             std::tuple<Y>
            ,typename merge<
                 PRED
                ,std::tuple<X, Xs...>
                ,std::tuple<Ys...>
            >::type
        >::type
    >::type;
};

template<template<typename, typename> class PRED, typename SEQ>
struct mergesort {
    using type = typename merge<PRED,
        typename mergesort<
            PRED
            ,typename std::tuple_element<
                0
                ,typename split<SEQ>::type
            >::type
        >::type
        ,typename mergesort<
            PRED
            ,typename std::tuple_element<
                1
                ,typename split<SEQ>::type
            >::type
        >::type
    >::type;
};

template <template<typename, typename> class PRED, typename X>
struct mergesort<PRED, std::tuple<X>> {
    using type = std::tuple<X>;
};

template <template<typename, typename> class PRED>
struct mergesort<PRED, std::tuple<>> {
    using type = std::tuple<>;
};

template<typename lhs, typename rhs>
struct predic_less {
    enum { value = lhs::first_type::value < rhs::first_type::value };
};

/***************************************************************************/

} // ns detail
} // ns yas

#endif // __yas__detail__tools__ctsort_hpp