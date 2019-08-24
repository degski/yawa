
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

#include <nlohmann/json.hpp>

// for convenience.
using json = nlohmann::json;

struct location_t {
    std::string lat, lng;
};

inline void to_json ( json & j_, location_t const & l_ ) { j_ = json{ { "lat", l_.lat }, { "lng", l_.lng } }; }
inline void from_json ( json const & j_, location_t & l_ ) {
    j_.at ( "lat" ).get_to ( l_.lat );
    j_.at ( "lng" ).get_to ( l_.lng );
}

struct place_t {
    location_t location;
    std::string elevation;
    std::string place, country;
    std::string place_country; // denver_united_states
};

inline void to_json ( json & j_, place_t const & p_ ) {
    j_ = json{ { "location", p_.location },
               { "elevation", p_.elevation },
               { "place", p_.place },
               { "country", p_.country },
               { "place_country", p_.place_country } };
}
inline void from_json ( json const & j_, place_t & p_ ) {
    j_.at ( "location" ).get_to ( p_.location );
    j_.at ( "elevation" ).get_to ( p_.elevation );
    j_.at ( "place" ).get_to ( p_.place );
    j_.at ( "country" ).get_to ( p_.country );
    j_.at ( "place_country" ).get_to ( p_.place_country );
}

struct geo_t {
    std::string current;
    std::map<std::string, place_t> places;
};

inline void to_json ( json & j_, geo_t const & r_ ) {
    j_[ "current" ] = r_.current;
    j_[ "places" ]  = r_.places;
}
inline void from_json ( json const & j_, geo_t & r_ ) {
    j_.at ( "current" ).get_to ( r_.current );
    j_.at ( "places" ).get_to ( r_.places );
}

using auth_t = std::map<std::string, std::string>;

void save_geo ( );
void load_geo ( );

void save_auth ( );
void load_auth ( );

[[nodiscard]] std::string load_file ( std::wstring const & filename );

[[nodiscard]] std::string to_place_country_string ( std::string const & place_country_ );

[[nodiscard]] place_t const & place_data ( std::string const & place_, std::string const & country_ );

// Date in format '2019-08-18'.
void astro ( std::string const & date_ );
