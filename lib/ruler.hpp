//Token class
#pragma once

#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <any>
#include <string_view>
#include <exception>
#include <functional>


using std::literals::string_view_literals::operator""sv;


namespace markdownItCpp {

//template<typename MarkdownIt>
//using FN = std::function<bool(state_base&,int,int,bool)>;

template<typename FN>
class Ruler{
public:

    struct rule {
        std::string name;
        bool enable;
        FN fn;
        std::vector<std::string> alt;
    };

    int __find__(std::string_view name){
        for(int i = 0;i< __rules__.size() ; ++i)
            if( __rules__[i].name == name)
                return i;
        return -1;
    }
    void __compile__(){
        std::vector<std::string> chains{""};

        // collect unique names
        std::for_each(__rules__.begin(), __rules__.end(), 
                [&chains](rule & ru){
                    if( !ru.enable ) return;

                    for (const auto& e : ru.alt) {
                    if( std::find(chains.begin(),chains.end(),e) == chains.end())
                        chains.push_back(e);
                    }
                });
        __cache__.clear();
        for (const auto& chain : chains) {
            __cache__[chain].clear();
            for (const auto& e : __rules__) {
                if( !e.enable ) continue;
                if( chain.length() && 
                    std::find(e.alt.begin(),e.alt.end(),chain) == e.alt.end()
                        )
                    continue;
                __cache__[chain].push_back(e.fn);
            }
        }

    }

public:
    
    template<typename... STR>
    void at(std::string name,FN fn, STR... alt_names){
        int index = __find__(name);
        if( index == -1)
            throw std::string("Parser rule not found: ") + name;
        __rules__[index].fn = fn;

        if constexpr ( sizeof...(alt_names) > 0){
            __rules__[index].alt.clear();
            ( __rules__[index].alt.push_back(alt_names),...);
        }
        __cache__.clear(); //清空
    }
    
    template<typename... STR>
    void before(std::string_view beforeName,
            std::string_view ruleNmae,
            FN fn,
            STR... alt_names
            ){

        
        if constexpr ( sizeof...(alt_names) > 0){
        }

    }

    template<typename... STR>
    void after(std::string_view afterName,
            std::string_view ruleNmae,
            FN fn,
            STR... alt_names
            );

    void push(std::string_view ruleName,
            FN fn,
            std::vector<std::string> __alts
            ) {
        rule t{std::string(ruleName),true,fn,std::move(__alts)};
        __rules__.push_back(std::move(t));
        //__rules__.emplace_back(
                //ruleName,
                //true,
                //fn,
                //std::move(__alts)
                //);
        __cache__.clear();
    }

    void push(std::string_view ruleName, FN fn)
    {

        rule t{std::string(ruleName),true,fn,{}};
        __rules__.push_back(std::move(t));
        __cache__.clear();
    }
    template<typename... STR>
    void enable(STR... names,bool ignoreInvalid=false);

    template<typename... STR>
    void enableOnly(STR... names,bool ignoreInvalid=false);

    template<typename... STR>
    void disable(STR... names,bool ignoreInvalid=false);

    
    const std::vector<FN>& 
        getRules(std::string chainName){
        if( __cache__.size() == 0)
            __compile__();
        if( __cache__.find(chainName) != __cache__.end())
            return __cache__[chainName];
        else
            return none;
    }

public:
    std::vector<rule> __rules__;
    std::map<std::string,std::vector<FN>> __cache__;
    const std::vector<FN> none{};
};

} // end namespace markdownItCpp
