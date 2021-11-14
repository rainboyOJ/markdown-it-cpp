

#pragma once

#include "token.hpp"
#include "MarkdownIt_base.hpp"
#include "define.hpp"

namespace markdownItCpp {


//template<typename MarkdownIt>
class state_base {
public:

    state_base(std::string_view src, 
            MarkdownIt_base& md,
            ENV env,
            TokenArrayRef tokens
            )
        :src{src},md{md},env{env},tokens{tokens}
    {}
public:

    virtual Token push(std::string type,std::string tag,int nesting){ return {};}
    virtual bool isEmpty(int line){ return true;}
    virtual int skipEmptyLines(int from){ return 1;}
    virtual int skipSpaces(int pos){ return 1;}
    virtual int skipSpacesBack(int pos,int _min){return 1;}
    virtual int skipChars(int pos,char code ){ return 1;}
    virtual int skipCharsBack(int pos,char code,int _min){ return 1;}

    virtual std::string getLines(int begin,int end,int indent,bool keepLastLF){ return "";}
    //virtual void Token() {}

    // state_block
    using Array  = std::vector<int> ;
    Array bMarks;   //line beign
    Array eMarks;   //line end
    Array tShift;   //第一个非空白字符 offset tabs not expand
    Array sCount; //第一个非空白字符 offset tabs expand
    Array bsCount;
    int blkIndent{0};
    int line{0};
    int lineMax{0};
    bool tight{false};
    int ddIndent{-1};
    int listIndent{-1};
    std::string parentType{ "root"};
    int level{0};
    std::string result;
    MarkdownIt_base& md;
    std::string src;
    ENV env;
    TokenArrayRef tokens;

    //state_core
    bool inlineMode{false};
};

} // end namespace markdownItCpp
