
// MIT License
//
// Copyright (c) 2019 degski
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <map>
#include <string>

#include "globals.hpp"

struct location_t {
    std::string lat, lng;

    template<class Archive>
    void serialize ( Archive & ar_ ) {
        ar_ ( CEREAL_NVP ( lat ), CEREAL_NVP ( lng ) );
    }
};

struct place_t {
    location_t location;
    std::string elevation;
    std::string place, country;

    template<class Archive>
    void serialize ( Archive & ar_ ) {
        ar_ ( CEREAL_NVP ( location ), CEREAL_NVP ( elevation ), CEREAL_NVP ( place ), CEREAL_NVP ( country ) );
    }
};

inline std::map<std::string, place_t> g_references;
inline std::map<std::string, std::string> g_authenticate;

void save_places ( );
void load_places ( );

void save_keys ( );
void load_keys ( );

[[nodiscard]] std::string load_file ( std::string const & filename );

// Returns string, with spaces replaced by '+'.
[[nodiscard]] std::string to_query_string ( std::string const & str_ );
// Returns concatenated (with '+') string of name and country, with spaces replaced by '+'.
[[nodiscard]] std::string to_query_string ( std::string const & place_, std::string const & country_ );

[[nodiscard]] std::string darksky_forcast_query_string ( location_t const & loc_ );
[[nodiscard]] std::string apixu_forcast_query_string ( location_t const & loc_ );

[[nodiscard]] place_t const & place_data ( std::string const & place_, std::string const & country_ );
