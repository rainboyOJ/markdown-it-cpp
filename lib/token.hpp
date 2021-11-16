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

    friend std::ostream& operator<<(std::ostream& os,const Token & to){
        os << "type: " << to.type << std::endl;
        os << "tag: " << to.tag << std::endl;
        os << "attrs: [";
        for (const auto& e : to.attrs) 
            os << '[' << e.first << ' '<<  e.second  << ']'<< std::endl;
        os << "]" << std::endl;
        os << "map: [" << to.map.first << ' ' << to.map.second << " ]" << std::endl;
        os << "content: " << to.content << std::endl;
        os << "info: " << to.info << std::endl;
        os << "block: " << std::boolalpha  << to.block << std::endl;
        os << "hidden: " << std::boolalpha  << to.hidden << std::endl;

        os << "child :[ ";
        for (const auto& e : to.children) os << e;
        os << " ]" <<std::endl;


        os << std::endl;
        return os;
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


} // end namespace markdownItCpp

