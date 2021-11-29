//Render class
#pragma once

#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <any>
#include <string_view>
#include <exception>
#include <functional>
#include <unordered_map>

#include "define.hpp"
#include "common/utils.hpp"


//using std::literals::string_view_literals::operator""sv;

namespace markdownItCpp {

namespace DEFAULT_RENDER_RULE {

template<typename Render>
std::string render_text(TokenArrayRef toks,int idx,optionsType&,ENV&,const Render *){
    return escapeHtml(toks[idx].content);
}

template<typename Render>
std::string hardbreak(TokenArrayRef toks,int idx,optionsType& opt,ENV&,const Render *){
    return opt.xhtmlOut ? "<br />\n" : "<br>\n";
}
template<typename Render>
std::string softbreak(TokenArrayRef toks,int idx,optionsType& opt,ENV&,const Render *){
    return opt.breaks 
        ?  (opt.xhtmlOut ? "<br />\n" : "<br>\n" )
        : "\n";
}

template<typename Render>
std::string code_block(TokenArrayRef toks,int idx,optionsType& opt,ENV&,const Render * self){
    auto &tok = toks[idx];
    return std::string("<pre") + self->renderAttrs(tok) + "><code>" +
          escapeHtml(tok.content) +
          "</code></pre>\n";
}

template<typename Render>
std::string fence(TokenArrayRef toks,int idx,optionsType& opt,ENV&,const Render * self){
    auto &tok = toks[idx];
    if( tok.info.length()  > 0){ //if language exists
        auto i = tok.attrIndex("class");
        if(  i == -1){ // not found
            tok.attrPush("class", opt.langPrefix + tok.info);
        }
        else{
            tok.attrs[i].second += " ";
            tok.attrs[i].second += opt.langPrefix;
            tok.attrs[i].second += tok.info;
        }
    }

    return  std::string("<pre><code") + self->renderAttrs(tok) + ">"
        + escapeHtml(tok.content)
        + "</code></pre>\n";
}


template<typename Render>
std::string code_inline(TokenArrayRef toks,int idx,optionsType& opt,ENV&,const Render * self){
    auto &tok = toks[idx];
    return std::string("<code") + self->renderAttrs(tok) + ">" +
          escapeHtml(tok.content) +
          "</code>";
}

} // end of DEFAULT_RENDER_RULE


class Render {
public:
    Render(){
        //rules["text"] = render_text;
        rules.emplace("text",       DEFAULT_RENDER_RULE::render_text<Render>);
        rules.emplace("hardbreak",  DEFAULT_RENDER_RULE::hardbreak<Render>);
        rules.emplace("softbreak",  DEFAULT_RENDER_RULE::softbreak<Render>);
        rules.emplace("code_block", DEFAULT_RENDER_RULE::code_block<Render>);
        rules.emplace("fence",      DEFAULT_RENDER_RULE::fence<Render>);
        rules.emplace("code_inline",DEFAULT_RENDER_RULE::code_inline<Render>);

    }

    // 渲染tokens里的attrs属性
    std::string renderAttrs(Token & tok) const{
        if( tok.attrs.size() == 0) return "";
        std::string result{};
        for (const auto& [name,attr] : tok.attrs) {
            result += " ";
            result += escapeHtml(name);
            result += "=\"";
            result += escapeHtml(attr); result += "\"";
        }
        return result;
    }

    //renderToken 的作用就是渲染开标签或者闭合标签，内部还会调用 renderAttrs 来生成 attributes。
    std::string renderToken(TokenArrayRef& toks,int idx,optionsType& opt) const{
        auto & tok = toks[idx];
        if( tok.hidden ) return "";
        std::string result{};
        // 在block 前与前一个hidden 之间加入一个换行
        if( tok.block && tok.nesting != -1 && idx && toks[idx-1].hidden )
            result += '\n';
        // 加 token name <img
        result += (tok.nesting == -1 ? "</" : "<") + tok.tag_str();
        // 加 token attr <img src="foo"
        result += renderAttrs(tok);

        // Add a slash for self-closing tags, e.g. `<img src="foo" /`
        if( tok.nesting == 0 && opt.xhtmlOut )
            result += " /";

        // Check if we need to add a newline after this tag
        bool needLF = false;
        if( tok.block ){
            needLF = true;
            if( tok.nesting == 1){
                if( idx +1 < toks.size() ){ //不是最后一个token
                    auto& nextToken = toks[idx+1];
                    if( nextToken.type == "inline" || nextToken.hidden)
                        needLF = false;
                    else if( nextToken.nesting == -1 && nextToken.tag == tok.tag)

                        // Opening tag + closing tag of the same type. E.g. `<li></li>`.
                        needLF = false;

                }
            }
        }
        result += needLF ? ">\n" : ">";
        return result;
    }

    /**
     * 渲染 inline Token
     */
    std::string renderInline(TokenArrayRef& toks,optionsType& opt,ENV& env){
        std::string result{};
        for(int i=0;i<toks.size();++i){
            auto type = toks[i].type_str();
            if( rules.count(type) != 0 )
                result += rules[type](toks,i,opt,env,this);
            else
                result += renderToken(toks, i, opt);
        }
        return result;
    }

    std::string renderInlineAsText(TokenArrayRef& toks,int idx,optionsType& opt){

    }


    std::string render(TokenArrayRef& toks,optionsType& opt,ENV env){
        std::string result{};
        for(int i = 0 ;i < toks.size() ; ++i){
            auto type = toks[i].type_str();
            if( type == "inline")
                result += renderInline(toks[i].children,opt,env);
            else if( rules.count(type) != 0)
                result += rules[type](toks,i,opt,env,this);
            else 
                result += renderToken(toks,i,opt);
        }
        return result;
    }
    //
    default_render_ruleFn<Render>& getRules(std::string_view name){
        return rules[std::string(name)];
    }
    
    std::unordered_map<std::string, default_render_ruleFn<Render>> rules;
    
};

}
