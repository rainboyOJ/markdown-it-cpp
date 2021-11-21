
#pragma once
#include <string_view>
using std::literals::string_view_literals::operator""sv;

namespace markdownItCpp {

    // common
    constexpr auto inline_type     = "inline"sv;
    constexpr auto emptyLine       = ""sv;
    constexpr auto parentType_root = "root"sv;
    constexpr auto codeblock_type  = "code_block"sv;
    constexpr auto code_type       = "code"sv;

    //paragraph
    constexpr auto paragraph_open_type = "paragraph_open"sv;
    constexpr auto paragraph_close_type = "paragraph_close"sv;
    constexpr auto paragraph_tag= "p"sv;

    //table
    constexpr auto table_open_type  = "table_open"sv;
    constexpr auto table_close_type = "table_close"sv;
    constexpr auto table_type       = "table"sv;

    constexpr auto table_thead_open_type  = "thead_open"sv;
    constexpr auto table_thead_close_type = "thead_close"sv;
    constexpr auto table_thead_type       = "thead"sv;

    constexpr auto tbody_open_type  = "tbody_open"sv;
    constexpr auto tbody_close_type = "tbody_close"sv;
    constexpr auto tbody_type       = "tbody"sv;

    constexpr auto tr_open_type  = "tr_open"sv;
    constexpr auto tr_close_type = "tr_close"sv;
    constexpr auto tr_type       = "tr"sv;

    constexpr auto th_open_type  = "th_open"sv;
    constexpr auto th_close_type = "th_close"sv;
    constexpr auto th_type       = "th"sv;

    constexpr auto td_open_type  = "td_open"sv;
    constexpr auto td_close_type = "td_close"sv;
    constexpr auto td_type       = "td"sv;

    //inline


    // char
    constexpr uint32_t GREATER_CHAR = 0x3E; // >
    constexpr uint32_t SPACE_CHAR   = 0x20; // ' '
    constexpr uint32_t TAB_CHAR     = 0x09; // '\t'
}
