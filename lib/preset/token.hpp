
#pragma once
#include <string_view>
using std::literals::string_view_literals::operator""sv;

namespace markdownItCpp {
    //parentType
    constexpr auto parentType_root       = "root"sv;
    constexpr auto parentType_paragraph  = "paragraph"sv;
    constexpr auto reference_paragraph  = "reference"sv;
    constexpr auto parentType_blockquote = "blockquote"sv;
    constexpr auto parentType_list = "list"sv;

    //markup
    constexpr auto blockquote_markup = ">"sv;
    constexpr auto blockquote_open   = "blockquote_open"sv;
    constexpr auto blockquote_close  = "blockquote_close"sv;
    constexpr auto blockquote_tag  = "blockquote"sv;


    // common
    constexpr auto inline_type     = "inline"sv;
    constexpr auto emptyLine       = ""sv;
    constexpr auto codeblock_type  = "code_block"sv;
    constexpr auto code_type       = "code"sv;
    constexpr auto fenc_type       = "fence"sv;
    constexpr auto hr_type       = "hr"sv;

    //paragraph
    constexpr auto paragraph_open_type  = "paragraph_open"sv;
    constexpr auto paragraph_close_type = "paragraph_close"sv;
    constexpr auto paragraph_tag        = "p"sv;

    //table
    constexpr auto table_open_type        = "table_open"sv;
    constexpr auto table_close_type       = "table_close"sv;
    constexpr auto table_type             = "table"sv;

    constexpr auto table_thead_open_type  = "thead_open"sv;
    constexpr auto table_thead_close_type = "thead_close"sv;
    constexpr auto table_thead_type       = "thead"sv;

    constexpr auto tbody_open_type        = "tbody_open"sv;
    constexpr auto tbody_close_type       = "tbody_close"sv;
    constexpr auto tbody_type             = "tbody"sv;

    constexpr auto tr_open_type           = "tr_open"sv;
    constexpr auto tr_close_type          = "tr_close"sv;
    constexpr auto tr_type                = "tr"sv;

    constexpr auto th_open_type           = "th_open"sv;
    constexpr auto th_close_type          = "th_close"sv;
    constexpr auto th_type                = "th"sv;

    constexpr auto td_open_type           = "td_open"sv;
    constexpr auto td_close_type          = "td_close"sv;
    constexpr auto td_type                = "td"sv;


    //list
    constexpr auto ordered_list_open_type  = "ordered_list_open"sv;
    constexpr auto ordered_list_close_type = "ordered_list_close"sv;
    constexpr auto bullet_list_open_type   = "bullet_list_open"sv;
    constexpr auto bullet_list_close_type  = "bullet_list_close"sv;
    constexpr auto list_item_open_type     = "list_item_open"sv;
    constexpr auto list_item_close_type    = "list_item_close"sv;
    constexpr auto ol_type                 = "ol"sv;
    constexpr auto ul_type                 = "ul"sv;
    constexpr auto li_type                 = "li"sv;

    //inline


    // char
    constexpr uint32_t LESS_CHAR          = 0x3C; // '< '
    constexpr uint32_t GREATER_CHAR       = 0x3E; // >
    constexpr uint32_t SPACE_CHAR         = 0x20; // ' '
    constexpr uint32_t TAB_CHAR           = 0x09; // '\t'
    constexpr uint32_t TILDE_CHAR         = 0x7e; // '~'
    constexpr uint32_t GRAVE_CHAR         = 0x60; // '`'
    constexpr uint32_t pip_char           = '|';
    constexpr uint32_t backslash_char     = '\\';
    constexpr uint32_t minus_char         = '-';
    constexpr uint32_t colon_char         = ':';
    constexpr uint32_t STAR_CHAR          = 0x2A; // '*'
    constexpr uint32_t UNDERSCORE_CHAR    = 0x5F; // '_'
    constexpr uint32_t QUOTE_CHAR         = 0x22; // ' " '
    constexpr uint32_t SINGLE_QUOTE_CHAR  = 0x27; // ' ' '
    constexpr uint32_t OPEN_CHAR          = 0x28; // ' ( '
    constexpr uint32_t CLOSE_CHAR         = 0x29; // ' ) '
    constexpr uint32_t OPEN_BRACKET_CHAR  = 0x5b; // ' [ '
    constexpr uint32_t BACKSLASH_CHAR     = 0x5C; // ' \ '
    constexpr uint32_t CLOSE_BRACKET_CHAR = 0x5d; // ' ] '
    constexpr uint32_t newliner_char = '\r';
    constexpr uint32_t newlinen_char = '\n';
    constexpr uint32_t NULL_char= '\0';

}
