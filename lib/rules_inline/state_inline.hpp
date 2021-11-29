#pragma once
#include <string>
#include <sstream>

#include "../define.hpp"
#include "../token.hpp"
#include "../common/utils.hpp"


namespace markdownItCpp {

class StateInline {
public:

    StateInline(std::string_view src,MarkdownIt_base& md,ENV env,
            TokenArrayRef tokens
            )
      :src{src},md{md},env{env},tokens{tokens},
        posMax{src.length()}
    {}

    std::string_view slice(size_t start,size_t end){
        return std::string_view(&src[start],end-start);
    }

    void pushPending(){
        Token t(text_type,emptyLine,0);
        //t.content = std::move(pending);
        t.content  = md.push_content_cache(std::move(pending));
        t.level = pendingLevel;
        tokens.push_back(std::move(t));
        pending.clear();
    }
    Token& push(std::string_view type,std::string_view tag,int nesting){
        if( pending.length() ) //先把pending 加入
            pushPending();
        Token t(type,tag,nesting);
        //tokens_meta TODO

        if( nesting < 0){ //closing tag
            level--;
            //this.delimiters = this._prev_delimiters.pop();
        }

        if( nesting > 0) //open tag
        {
            level++;
        }
        pendingLevel = level;
        tokens.push_back(std::move(t));
        //tokens_meta
        return tokens.back();
    }

    int scanDelims(int start,bool canSplitWord){
        auto ch = src[start];
        auto pos = start;
        while( pos < posMax && ch == src[pos] ) pos++;
        return pos - start; //数量
    }

    inline uint32_t charCodeAt(size_t pos){
        return uCodeChar(src[pos]);
    }

    Token& tokens_back() {
        return  tokens.back();
    }



public:
    MarkdownIt_base& md;
    std::string_view src;
    ENV env;
    TokenArrayRef tokens;

    //this.tokens_meta = Array(outTokens.length);
    int pos = 0; //当前 token 的 content 的第几个字符串索引
    size_t posMax; //当前 token 的 content 的最大索引
    int level = 0; // 类似递归的级别
    std::string pending{""}; //存放一段完整的字符串，比如 **emphasis** -> emphasis
    int pendingLevel = 0;

  // Stores { start: end } pairs. Useful for backtrack
  // optimization of pairs parse (emphasis, strikes).
    std::map<int,int> cache{};

  // List of emphasis-like delimiters for current tag
  // 存放一些特殊标记的分隔符，比如 *、~ 等
    std::vector<delimiters_type> delimiters{};

  // Stack of delimiter lists for upper level tags
  //this._prev_delimiters = [];

  // backtick length => last seen position
  std::map<int,int>backticks{};
  bool backticksScanned = false;
};

}
