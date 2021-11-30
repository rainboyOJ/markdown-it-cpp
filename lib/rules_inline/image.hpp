#pragma once
#include <string>

#include "../preset/token.hpp"
#include "./state_inline.hpp"

namespace markdownItCpp {

bool image(StateInline& state,bool silent) {
    auto end   = state.posMax;
    auto start = state.pos;
    std::string_view label;
    std::string href,title;
    bool parseReference = true;

    if( state.src[start++] != '!') return false;
    if( state.src[start++] != '[') return false;

    int labelStart = start;
    int labelEnd = parseLinkLabel(state.src, labelStart, end);
    if( labelEnd < 0 ) return false;
    label = state.slice(start, labelEnd);

    int pos = labelEnd + 1;

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
                // [link](  <href>  "title"  )
                //                         ^^ skipping these spaces
                pos = skipSpace(state.src, pos);
            }
            if( pos >= end || state.src[pos] !=')'){
                parseReference = true;
            }
            pos++; //过滤 )
        }
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
        auto & token = state.push(image_type ,img_tag,0);
        token.attrPush("src", href);
        if( title.length() != 0)
            token.attrPush("title", title);
        if( label.length() !=0)
            token.attrPush("alt", std::string(label));
        //state.md.inlineTokenize(state);
    }
    state.pos = pos;
    state.posMax = end;
    return true;
}

}
