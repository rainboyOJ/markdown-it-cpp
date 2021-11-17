
#pragma once
#include <string_view>
using std::literals::string_view_literals::operator""sv;

namespace markdownItCpp {
    //paragraph
    constexpr auto paragraph_open_type = "paragraph_open"sv;
    constexpr auto paragraph_close_type = "paragraph_close"sv;
    constexpr auto paragraph_tag= "p"sv;

    //markup
    constexpr auto blockquotes_markup = ">"sv;
}
