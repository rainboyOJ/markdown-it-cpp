//Token class
#pragma once

#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <any>
#include <string_view>

//using namespace std::literals;
using std::literals::string_view_literals::operator""sv;
namespace markdownItCpp {
class Token {
public:
    Token() = default;
    Token(std::string type,std::string tag,int nesting)
        :type{std::move(type)},tag{std::move(tag)},nesting{nesting}
    {}

    int attrIndex(std::string_view name){
        if( attrs.empty() ) return -1;
        for(int i = 0;i<attrs.size();++i){
            if( attrs[i].first == name) return i;
        }
        return -1;
    }
    void attrPush(std::string name,std::string value){
        attrs.emplace_back(std::move(name),std::move(value));
    }

    void attrSet(std::string name,std::string value){
        int idx = attrIndex(name);
        if( idx < 0)
            attrPush(name, value);
        else
            attrs[idx] = std::make_pair(
                    std::move(name),
                    std::move(value)
                    );
    }

    std::string_view attrGet(std::string_view name){
        int idx = attrIndex(name);
        if( idx < 0) return ""sv;
        return attrs[idx].second;
    }
    void attrJoin(std::string name,std::string value){
        int idx = attrIndex(name);
        if( idx < 0)
            attrPush(name, value);
        else
            attrs[idx].second.append(" " + value);
    }

public:
    std::string type;
    std::string tag;
    std::vector<std::pair<std::string, std::string>> attrs;
    std::pair<int,int> map;
    int nesting;
    int level{0};
    std::vector<Token> children;
    std::string content;
    std::string markup;
    std::string info;
    std::any meta;
    bool block{false};
    bool hidden{false};

};

using TokenArray = std::vector<Token>;
using TokenArrayRef = std::vector<Token> &;

} // end namespace markdownItCpp

