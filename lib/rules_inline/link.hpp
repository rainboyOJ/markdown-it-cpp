#pragma once
#include <string>

#include "../preset/token.hpp"
#include "./state_inline.hpp"

namespace markdownItCpp {

bool link(StateInline& state,bool silent) {
    auto end   = state.posMax;
    auto start = state.pos;
    std::string_view label;
    std::string href,title;
    bool parseReference = true;

    if( state.src[start] != '[') return false;
    int labelStart = start+1;
    int labelEnd = parseLinkLabel(state.src, start, end);
    if( labelEnd < -1 ) return false; //没有找到

    int pos = labelEnd+1;
    if( pos < end && state.src[pos] == '('){ // inline_link

        parseReference = false;
        pos++;
        // [link](  <href>  "title"  )
        //          ^^^^^^ parsing link destination
        pos = skipSpace(state.src, pos);
        if( pos >= end) return false;

        auto res = parseLinkDestination(state.src, pos, state.posMax);
        if( res.ok ){ //解析成功
            href = std::move(res.str);
            pos = res.pos;
            // [link](  <href>  "title"  )
            //                ^^ skipping these spaces
            int posBack = pos;
            pos = skipSpace(state.src, pos);
            res = parseLinkTitle(state.src, pos, end);
            if( pos < end && pos != posBack && res.ok){
                title = std::move(res.str);
                pos = res.pos;
                pos = skipSpace(state.src, pos);
            }
        }
        if( pos >= end || state.src[pos] != ')' )
            parseReference = true;
        pos++;
    }

    if(parseReference){
        if( pos < end && state.src[pos] == '['){
            auto start = pos;
            pos = parseLinkLabel(state.src, pos, end);
            if( pos >= 0)
                label = state.slice(start, pos++);
            else
                pos = labelEnd +1;
        }
        else {
            pos = labelEnd +1;
        }
        if( label.length() == 0 )
            label = state.slice(labelStart, labelEnd);
        auto item =  state.md.references.find(std::string(label));
        if(item == state.md.references.end()){
            return false;
        }
        href = item->second.href;
        title = item->second.title;
    }

    if( !silent ){
        state.pos = labelStart;
        state.posMax = labelEnd;
        auto & token = state.push(link_open_type,a_tag,1);
        token.attrPush("href", href);
        if( title.length() != 0)
            token.attrPush("title", title);
        state.md.inlineTokenize(state);
        state.push(link_close_type,a_tag,-1);
    }

    state.pos = pos;
    state.posMax = end;
    return true;

}

}
