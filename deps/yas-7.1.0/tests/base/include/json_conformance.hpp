
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

#ifndef __yas__tests__base__include__json_conformance_hpp
#define __yas__tests__base__include__json_conformance_hpp

#include "../externals/nlohmann_json.hpp"

#include <string>
#include <vector>
#include <array>
#include <fstream>

/***************************************************************************/

struct canada_t {
    canada_t() = default;

    std::string type;
    struct feature {
        std::string type;

        struct property {
            std::string name;

            template<typename Archive>
            Archive& serialize(Archive &ar) {
                auto o = YAS_OBJECT(nullptr, name);
                ar & o;

                return ar;
            }

            friend bool operator== (const property &l, const property &r) {
                return l.name == r.name;
            }
        };
        property properties;

        struct geometry {
            std::string type;
            std::vector<std::vector<std::array<double, 2>>> coordinates;

            template<typename Archive>
            Archive& serialize(Archive &ar) {
                auto o = YAS_OBJECT(
                     nullptr
                    ,type
                    ,coordinates
                );
                ar & o;

                return ar;
            }

            friend bool operator== (const geometry &l, const geometry &r) {
                return l.type == r.type && l.coordinates == r.coordinates;
            }
        };
        geometry geometry;

        template<typename Archive>
        Archive& serialize(Archive &ar) {
            auto o = YAS_OBJECT(
                 nullptr
                ,type
                ,properties
                ,geometry
            );
            ar & o;

            return ar;
        }

        friend bool operator== (const feature &l, const feature &r) {
            return l.type == r.type && l.properties == r.properties && l.geometry == r.geometry;
        }
    };

    std::vector<feature> features;

    template<typename Archive>
    Archive& serialize(Archive &ar) {
        auto o = YAS_OBJECT(
             nullptr
            ,type
            ,features
        );
        ar & o;

        return ar;
    }
};

bool operator== (const canada_t &l, const canada_t &r) {
    return l.type == r.type && l.features == r.features;
}
bool operator!= (const canada_t &l, const canada_t &r) { return !(l == r); }

/***************************************************************************/

template<typename archive_traits>
bool json_conformance_test(std::ostream &log, const char *artype, const char *testname) {
    {
        static const char *max_fname = "../json_data/canada-max.json";
        static const char *min_fname = "../json_data/canada-min.json";
        canada_t canada_max = canada_t(), canada_min = canada_t();

        yas::load<yas::file|yas::json>(max_fname, canada_max);
        yas::load<yas::file|yas::json|yas::compacted>(min_fname, canada_min);

        YAS_TEST_REPORT_IF(
             canada_max != canada_min
            ,log, artype, testname, return false
        );

        std::ifstream file(max_fname);
        if ( file ) {
        } else {
            YAS_TEST_REPORT(log, artype, testname)

            return false;
        }
        nlohmann::json j;
        file >> j;

        YAS_TEST_REPORT_IF(j.at("type") != canada_min.type, log, artype, testname, return false)

        const auto &f = j.at("features").at(0);
        const auto &cf = canada_min.features;

        YAS_TEST_REPORT_IF(
             f.at("type") != cf.at(0).type
            ,log, artype, testname, return false
        )

        YAS_TEST_REPORT_IF(
             f.at("properties").at("name") != cf.at(0).properties.name
            ,log, artype, testname, return false
        )

        YAS_TEST_REPORT_IF(
             f.at("geometry").at("type") != cf.at(0).geometry.type
            ,log, artype, testname, return false
        )

        const auto &jcoo = f.at("geometry").at("coordinates");
        const auto &ccoo = cf.at(0).geometry.coordinates;
        YAS_TEST_REPORT_IF(
             jcoo.size() != ccoo.size()
            ,log, artype, testname, return false
        )

        for ( std::size_t ii = 0; ii < ccoo.size(); ++ii ) {
            const auto &ccoov = ccoo.at(ii);
            const auto &jcoov = jcoo.at(ii);
            YAS_TEST_REPORT_IF(
                 jcoov.size() != ccoov.size()
                ,log, artype, testname, return false
            )
            for ( std::size_t jj = 0; jj < ccoov.size(); ++jj ) {
                const auto &l = jcoov.at(jj).at(0);
                const auto &r = ccoov.at(jj).at(0);
                YAS_TEST_REPORT_IF(
                     l != r
                    ,log, artype, testname, return false
                )
            }
        }
    }

    return true;
}

/***************************************************************************/

#endif // __yas__tests__base__include__json_conformance_hpp
