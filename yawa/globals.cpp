
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

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <fcntl.h>
#include <io.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <chrono>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

#include <curlpp/cURLpp.hpp>

#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include <date/date.h>
#include <date/tz.h>

#include "globals.hpp"

namespace fs = std::filesystem;

[[nodiscard]] fs::path appDataPath ( std::wstring && place_ ) noexcept {
    wchar_t * value;
    std::size_t len;
    _wdupenv_s ( &value, &len, L"USERPROFILE" );
    fs::path return_value ( std::wstring ( value ) + std::wstring ( L"\\AppData\\Roaming\\" + place_ ) );
    fs::create_directory ( return_value ); // No error if directory exists.
    return return_value;
}

[[nodiscard]] fs::path getExePath ( ) noexcept {
    TCHAR exename[ 1024 ];
    GetModuleFileName ( NULL, exename, 1'024 );
    return fs::path ( exename ).parent_path ( );
}

json load ( fs::path const & file_ ) {
    json a;
    std::ifstream i ( file_ );
    i >> a;
    i.close ( );
    return a;
}

std::string get_timestamp_iso8601 ( ) noexcept {
    auto const zt =
        date::make_zoned ( date::current_zone ( ), date::floor<std::chrono::seconds> ( std::chrono::system_clock::now ( ) ) );
    return { date::format ( "%FT%T%Ez\n", zt ) };
}

std::string get_timestamp_hours_iso8601 ( ) noexcept {
    auto const zt =
        date::make_zoned ( date::current_zone ( ), date::floor<std::chrono::hours> ( std::chrono::system_clock::now ( ) ) );
    return { date::format ( "%FT%T%Ez\n", zt ) };
}

std::string get_timestamp_utc_iso8601 ( ) noexcept {
    std::time_t rawtime = std::time ( nullptr );
    std::tm ptm;
    gmtime_s ( &ptm, &rawtime );
    std::string s ( 21, '0' );
    std::snprintf ( s.data ( ), 21, "%4i-%02i-%02iT%02i:%02i:%02iZ", ptm.tm_year + 1900, ptm.tm_mon + 1, ptm.tm_mday, ptm.tm_hour,
                    ptm.tm_min, ptm.tm_sec );
    return s;
}

std::string get_timestamp_hours_utc_iso8601 ( ) noexcept {
    std::time_t rawtime = std::time ( nullptr );
    std::tm ptm;
    gmtime_s ( &ptm, &rawtime );
    std::string s ( 21, '0' );
    std::snprintf ( s.data ( ), 21, "%4i-%02i-%02iT%02i:00:00Z", ptm.tm_year + 1900, ptm.tm_mon + 1, ptm.tm_mday, ptm.tm_hour );
    return s;
}

std::string get_timestamp ( ) noexcept {
    std::time_t rawtime = std::time ( nullptr );
    std::tm ptm;
    localtime_s ( &ptm, &rawtime );
    std::string s ( 20, '0' );
    std::snprintf ( s.data ( ), 20, "%4i-%02i-%02iT%02i:%02i:%02i", ptm.tm_year + 1900, ptm.tm_mon + 1, ptm.tm_mday, ptm.tm_hour,
                    ptm.tm_min, ptm.tm_sec );
    return s;
}

#if _WIN32
#    define timegm _mkgmtime
#endif

// 2019-08-17 to epoch.
std::time_t date_to_epoch ( std::string const & d_ ) noexcept {
    std::tm tm = {};
    std::from_chars ( d_.data ( ), d_.data ( ) + 4, tm.tm_year );
    tm.tm_year -= 1'900;
    std::from_chars ( d_.data ( ) + 5, d_.data ( ) + 7, tm.tm_mon );
    tm.tm_mon -= 1;
    std::from_chars ( d_.data ( ) + 8, d_.data ( ) + 10, tm.tm_mday );
    return timegm ( &tm );
}

int local_utc_offset_minutes ( ) noexcept {
    std::time_t t  = std::time ( nullptr );
    std::tm * locg = std::localtime ( &t );
    std::tm locl;
    std::memcpy ( &locl, locg, sizeof ( std::tm ) );
    return ( timegm ( locg ) - mktime ( &locl ) ) / 60;

    /*

    // https://stackoverflow.com/a/32433990/646940

    std::tm * loc = std::localtime ( &t );
    // save values because they could be erased by the call to gmtime.
    int loc_min   = loc->tm_min;
    int loc_hour  = loc->tm_hour;
    int loc_day   = loc->tm_mday;
    std::tm * utc = std::gmtime ( &t );
    int delta_min = loc_min - utc->tm_min;
    int delta_day = loc_day - utc->tm_mday;
    delta_min += ( loc_hour - utc->tm_hour ) * 60;
    // hack for the day because the difference actually is only 0, 1 or -1.
    if ( delta_day == 1 or delta_day < -1 )
        delta_min += 1'440;
    else if ( delta_day == -1 or delta_day > 1 )
        delta_min -= 1'440;
    return delta_min;
    */
}

#if _WIN32
#    undef timegm
#endif

json query_url ( std::string const & url_ ) {
    std::stringstream ss;
    try {
        curlpp::Easy elevation_request;
        elevation_request.setOpt<curlpp::options::WriteStream> ( &ss );
        elevation_request.setOpt<curlpp::options::Encoding> ( "deflate" );
        elevation_request.setOpt<curlpp::options::Url> ( url_ );
        elevation_request.perform ( );
    }
    catch ( curlpp::RuntimeError & e ) {
        std::cout << e.what ( ) << std::endl;
    }
    catch ( curlpp::LogicError & e ) {
        std::cout << e.what ( ) << std::endl;
    }
    return json::parse ( ss );
}

void save_to_file ( json const & j_, std::string const & name_ ) {
    std::ofstream o ( g_app_data_path / ( name_ + ".json" ) );
    o << std::setw ( g_indent ) << j_ << std::endl;
    o.flush ( );
    o.close ( );
}

void load_from_file ( json & j_, std::string const & name_ ) {
    std::ifstream i ( g_app_data_path / ( name_ + ".json" ) );
    i >> j_;
    i.close ( );
}

json load_from_file ( std::string const & name_ ) {
    json j;
    load_from_file ( j, name_ );
    return j;
}

void sleep_for_milliseconds ( std::int32_t const milliseconds_ ) noexcept {
    std::this_thread::sleep_for ( std::chrono::milliseconds ( milliseconds_ ) );
}

// https : // stackoverflow.com/questions/34842526/update-console-without-flickering-c

void cls ( ) noexcept {
    // Get the Win32 handle representing standard output.
    // This generally only has to be done once, so we make it static.
    static HANDLE const hOut = GetStdHandle ( STD_OUTPUT_HANDLE );
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD topLeft = { 0, 0 };
    // std::cout uses a buffer to batch writes to the underlying console.
    // We need to flush that to the console because we're circumventing
    // std::cout entirely; after we clear the console, we don't want
    // stale buffered text to randomly be written out.
    std::wcout.flush ( );
    // Figure out the current width and height of the console window.
    if ( not GetConsoleScreenBufferInfo ( hOut, &csbi ) )
        std::abort ( );
    DWORD length = csbi.dwSize.X * csbi.dwSize.Y;
    DWORD written;
    // Flood-fill the console with spaces to clear it.
    FillConsoleOutputCharacter ( hOut, TEXT ( ' ' ), length, topLeft, &written );
    // Reset the attributes of every character to the default.
    // This clears all background colour formatting, if any.
    FillConsoleOutputAttribute ( hOut, csbi.wAttributes, length, topLeft, &written );
    // Move the cursor back to the top left for the next sequence of writes.
    SetConsoleCursorPosition ( hOut, topLeft );
}

// x is the column, y is the row. The origin (0, 0) is top-left.
void set_cursor_position ( int x_, int y_ ) noexcept {
    static HANDLE const hOut = GetStdHandle ( STD_OUTPUT_HANDLE );
    std::wcout.flush ( );
    COORD coord = { ( SHORT ) x_, ( SHORT ) y_ };
    SetConsoleCursorPosition ( hOut, coord );
}

void write_buffer ( std::wostringstream const & outbuf_ ) noexcept {
    static HANDLE const hOut = GetStdHandle ( STD_OUTPUT_HANDLE );
    static COORD topLeft     = { 0, 0 };
    DWORD dwCharsWritten; // <-- this might not be necessary
    // You might be able to pass in NULL if you don't want to keep track of the
    // number of characters written. Some functions allow you to do this, others
    // don't. I'm not 100% sure about this one, the documentation doesn't say.
    WriteConsoleOutputCharacter ( hOut, outbuf_.str ( ).c_str ( ), ( DWORD ) outbuf_.str ( ).length ( ), topLeft, &dwCharsWritten );
}

void set_mode_unicode ( ) noexcept { _setmode ( _fileno ( stdout ), _O_U8TEXT ); }

bool hide_cursor ( ) noexcept {
    static HANDLE const hOut = GetStdHandle ( STD_OUTPUT_HANDLE );
    static CONSOLE_CURSOR_INFO const info{ 1, false };
    set_mode_unicode ( );
    return SetConsoleCursorInfo ( hOut, &info );
}
