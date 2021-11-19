
#pragma once
#include <string_view>
using std::literals::string_view_literals::operator""sv;

namespace markdownItCpp {
    //paragraph
    constexpr auto paragraph_open_type = "paragraph_open"sv;
    constexpr auto paragraph_close_type = "paragraph_close"sv;
    constexpr auto paragraph_tag= "p"sv;



    // char
    constexpr uint32_t GREATER_CHAR = 0x3E; // >
    constexpr uint32_t SPACE_CHAR   = 0x20; // ' '
    constexpr uint32_t TAB_CHAR     = 0x09; // '\t'
}
