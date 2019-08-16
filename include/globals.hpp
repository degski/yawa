
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

#include <array>
#include <charconv>
#include <filesystem>
#include <fstream>
#include <sax/iostream.hpp>
#include <sstream>
#include <string>

namespace fs = std::filesystem;
/*
#include <cereal/cereal.hpp>

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/xml.hpp>

#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>
*/

#include <fmt/core.h>

#include "geo.hpp"

#include "api.hpp"

#if _WIN32
#    pragma comment( lib, "ws2_32.lib" )
#    pragma comment( lib, "Crypt32.lib" )
#    pragma comment( lib, "winmm.lib" )
#    pragma comment( lib, "wldap32.lib" )
#    if defined( _DEBUG )
#        pragma comment( lib, "zlibd.lib" )
#        pragma comment( lib, "libcurl-d.lib" )
#        pragma comment( lib, "fmtd.lib" )
#        pragma comment( lib, "cpprest_2_10d.lib" )
#    else
#        pragma comment( lib, "zlib.lib" )
#        pragma comment( lib, "libcurl.lib" )
#        pragma comment( lib, "fmt.lib" )
#        pragma comment( lib, "cpprest_2_10.lib" )
#    endif
#    pragma comment( lib, "libcurlpp.lib" )
#    pragma comment( lib, "tz.lib" )
#endif

#if 0
#    if _WIN32
#        pragma comment( lib, "Bcrypt.lib" )
#        pragma comment( lib, "Winhttp.lib" )
#        pragma comment( lib, "Crypt32.lib" )
#        pragma comment( lib, "ssleay32.lib" )
#        pragma comment( lib, "libeay32.lib" )
#        pragma comment( lib, "brotlicommon-static.lib" )
#        pragma comment( lib, "brotlidec-static.lib" )
#        pragma comment( lib, "brotlienc-static.lib" )
#        if defined( _DEBUG )
#            pragma comment( lib, "zlibd.lib" )
#            pragma comment( lib, "boost_system-vc140-mt-gd.lib" )
#            pragma comment( lib, "boost_date_time-vc140-mt-gd.lib" )
#            pragma comment( lib, "boost_thread-vc140-mt-gd.lib" )
#            pragma comment( lib, "boost_regex-vc140-mt-gd.lib" )
#            pragma comment( lib, "cpprest_2_10d.lib" )
#        else
#            pragma comment( lib, "zlib.lib" )
#            pragma comment( lib, "boost_system-vc140-mt.lib" )
#            pragma comment( lib, "boost_date_time-vc140-mt.lib" )
#            pragma comment( lib, "boost_thread-vc140-mt.lib" )
#            pragma comment( lib, "boost_regex-vc140-mt.lib" )
#            pragma comment( lib, "cpprest_2_10.lib" )
#        endif
#    endif
#endif

[[nodiscard]] fs::path appDataPath ( std::string && place_ ) noexcept;
[[nodiscard]] fs::path getExePath ( ) noexcept;

inline fs::path const g_app_data_path = appDataPath ( "yawa" );
inline fs::path const g_app_path      = getExePath ( );

inline fs::path const g_geo_path  = g_app_data_path / "places.json";
inline fs::path const g_auth_path = g_app_data_path / "keys.json";

inline constexpr int g_indent = 4;

inline geo_t g_geo;
inline auth_t g_auth;

inline DisplayData g_data;

inline bool is_read ( char const file_[] ) noexcept {
    return ( fs::status ( file_ ).permissions ( ) & fs::perms::owner_read ) != fs::perms::none;
}
inline bool is_write ( char const file_[] ) noexcept {
    return ( fs::status ( file_ ).permissions ( ) & fs::perms::owner_write ) != fs::perms::none;
}

[[nodiscard]] inline std::string to_string ( int const v_ ) noexcept { return fmt::format ( "{0:}", v_ ); }
[[nodiscard]] inline std::string to_string ( float const v_ ) noexcept { return fmt::format ( "{0:f}", v_ ); }

template<typename T>
[[nodiscard]] T mps_to_kmph ( T const mps_ ) noexcept {
    return mps_ * ( T{ 3'600 } / T{ 1'000 } );
}
/*
template<typename T>
void save_to_file_bin ( T const & t_, fs::path && path_, std::string && file_name_, bool const append_ = false ) noexcept {
    std::ofstream ostream ( path_ / ( file_name_ + std::string ( ".cereal" ) ),
                            append_ ? std::ios::binary | std::ios::app | std::ios::out : std::ios::binary | std::ios::out );
    {
        cereal::BinaryOutputArchive archive ( ostream );
        archive ( t_ );
    }
    ostream.flush ( );
    ostream.close ( );
}

template<typename T>
void load_from_file_bin ( T & t_, fs::path && path_, std::string && file_name_ ) noexcept {
    std::ifstream istream ( path_ / ( file_name_ + std::string ( ".cereal" ) ), std::ios::binary );
    {
        cereal::BinaryInputArchive archive ( istream );
        archive ( t_ );
    }
    istream.close ( );
}

template<typename T>
void save_to_file_xml ( std::string const & object_name_, T const & t_, fs::path && path_, std::string && file_name_,
                        bool const append_ = false ) noexcept {
    std::ofstream ostream ( path_ / ( file_name_ + std::string ( ".xmlcereal" ) ),
                            append_ ? std::ios::app | std::ios::out : std::ios::out );
    {
        cereal::XMLOutputArchive archive ( ostream );
        archive ( cereal::make_nvp ( object_name_, t_ ) );
    }
    ostream.flush ( );
    ostream.close ( );
}

template<typename T>
void load_from_file_xml ( std::string const & object_name_, T & t_, fs::path && path_, std::string && file_name_ ) noexcept {
    std::ifstream istream ( path_ / ( file_name_ + std::string ( ".xmlcereal" ) ) );
    {
        cereal::XMLInputArchive archive ( istream );
        archive ( cereal::make_nvp ( object_name_, t_ ) );
    }
    istream.close ( );
}

void save_to_file ( json const & j_, std::string const & name_ );
void load_from_file ( json & j_, std::string const & name_ );
[[nodiscard]] json load_from_file ( std::string const & name_ );

template<typename T>
void save_to_file_json ( std::string const & object_name_, T const & t_, fs::path && path_, std::string && file_name_,
                         bool const append_ = false ) noexcept {
    std::ofstream ostream ( path_ / ( file_name_ + std::string ( ".json" ) ),
                            append_ ? std::ios::app | std::ios::out : std::ios::out );
    {
        cereal::JSONOutputArchive archive ( ostream );
        archive ( cereal::make_nvp ( object_name_, t_ ) );
    }
    ostream.flush ( );
    ostream.close ( );
}

template<typename T>
void load_from_file_json ( std::string const & object_name_, T & t_, fs::path && path_, std::string && file_name_ ) noexcept {
    std::ifstream istream ( path_ / ( file_name_ + std::string ( ".json" ) ) );
    {
        cereal::JSONInputArchive archive ( istream );
        archive ( cereal::make_nvp ( object_name_, t_ ) );
    }
    istream.close ( );
}

template<typename T>
void save_to_file_json ( std::string const & object_name_, T const & t_, fs::path const & path_,
                         bool const append_ = false ) noexcept {
    std::ofstream ostream ( path_, append_ ? std::ios::app | std::ios::out : std::ios::out );
    {
        cereal::JSONOutputArchive archive ( ostream );
        archive ( cereal::make_nvp ( object_name_, t_ ) );
    }
    ostream.flush ( );
    ostream.close ( );
}

template<typename T>
void load_from_file_json ( std::string const & object_name_, T & t_, fs::path const & path_ ) noexcept {
    std::ifstream istream ( path_ );
    {
        cereal::JSONInputArchive archive ( istream );
        archive ( cereal::make_nvp ( object_name_, t_ ) );
    }
    istream.close ( );
}

template<typename T>
void save_to_file_json ( char const object_name_[], T const & t_, char const file_[], bool const append_ = false ) noexcept {
    std::ofstream ostream ( file_, append_ ? std::ios::app | std::ios::out : std::ios::out );
    {
        cereal::JSONOutputArchive archive ( ostream );
        archive ( cereal::make_nvp ( object_name_, t_ ) );
    }
    ostream.flush ( );
    ostream.close ( );
}

template<typename T>
void load_from_file_json ( char const object_name_[], T & t_, char const file_[] ) noexcept {
    std::ifstream istream ( file_ );
    {
        cereal::JSONInputArchive archive ( istream );
        archive ( cereal::make_nvp ( object_name_, t_ ) );
    }
    istream.close ( );
}
*/
json query_url ( std::string const & url_ );

[[nodiscard]] std::string get_timestamp_iso8601 ( ) noexcept;
[[nodiscard]] std::string get_timestamp_hours_iso8601 ( ) noexcept;
[[nodiscard]] std::string get_timestamp_utc_iso8601 ( ) noexcept;
[[nodiscard]] std::string get_timestamp_hours_utc_iso8601 ( ) noexcept;

[[nodiscard]] std::time_t date_to_epoch ( std::string const & d_ ) noexcept;
[[nodiscard]] int local_utc_offset_minutes ( ) noexcept;

std::string get_timestamp ( ) noexcept;

void sleep_for_milliseconds ( std::int32_t const milliseconds_ ) noexcept;

void cls ( ) noexcept;
// x is the column, y is the row. The origin (0,0) is top-left.
void set_cursor_position ( int x_, int y_ ) noexcept;

void write_buffer ( std::wostringstream const & outbuf_ ) noexcept;

void set_mode_unicode ( ) noexcept;

[[nodiscard]] bool hide_cursor ( ) noexcept;
