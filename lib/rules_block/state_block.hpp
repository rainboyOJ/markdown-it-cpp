
#pragma once
#include <string>
#include <sstream>

#include "../define.hpp"
#include "../token.hpp"
#include "../common/utils.hpp"
#include <cassert>

namespace markdownItCpp {

//BlockState parentType

struct LineInfo {
    int bMarks;   //每一行开始 在原src的位置
    int eMarks;   //每一行开始 在原src的位置
    int tShift;   //第一个非空白字符 offset tabs not expand
    int sCount;   //第一个非空白字符 offset tabs expand
    int bsCount;  //都是0

    LineInfo() = default;
    LineInfo(int bMarks,int eMarks,int tShift,int sCount,int bsCount=0)
        :bMarks{bMarks},   //每一行开始 在原src的位置
        eMarks{eMarks},   //每一行开始 在原src的位置
        tShift{tShift},   //第一个非空白字符 offset tabs not expand
        sCount{sCount},   //第一个非空白字符 offset tabs expand
        bsCount{bsCount}  //都是0
    {}

    inline int first_nospace_pos(){ return bMarks + tShift; }

    //template<int MAX=4>
    inline bool line_sCount_less(int MAX=4){ return sCount < MAX; }

    inline bool isEmpty() { return bMarks + tShift >= eMarks; }

    std::pair<int, int> getBeginEnd() { return {first_nospace_pos(),eMarks}; }



};

//typedef long long ll;
//template<typename MarkdownIt>
//class StateBlock : public state_base<MarkdownIt> {
class StateBlock {
public:

    StateBlock(std::string_view src,
                MarkdownIt_base& md,
                ENV env,
                TokenArrayRef tokens
    ) :src{src},md{md},env{env},tokens{tokens}
    {
        bool indent_found = false;
        int ch,start,pos,indent,offset,len = src.length();
        start = pos = indent = offset = 0;
        for(; pos < len; ++pos){
            auto ch = src[pos];

            if( !indent_found ) {
                if(isSpace(ch)){
                    indent++;
                    if(ch == '\t')
                        offset += 4 - offset % 4;
                    else
                        offset++;
                    continue;
                }
                else indent_found = true;
            }

            if( ch == '\n' || pos == len - 1){
                if( ch != '\n') pos++;
                //bMarks.push_back(start);
                //eMarks.push_back(pos);
                //tShift.push_back(indent);
                //sCount.push_back(offset);
                //bsCount.push_back(0);
                lineInfo.emplace_back(start,pos,indent,offset,0);

                indent_found = false;
                indent = 0;
                offset = 0;
                start = pos+1;
            }
        }
        //bMarks.push_back(len);
        //eMarks.push_back(len);
        //tShift.push_back(0);
        //sCount.push_back(0);
        //bsCount.push_back(0);
        //last Fake Line
        lineInfo.emplace_back(len,len,0,0,0);

        lineMax = lineInfo.size()-1; //don't count last fake line

    }
    void push(Token & t)  {
        tokens.push_back(t);
    }

    class Token& push(std::string_view type,std::string_view tag,int nesting)  {
        class Token token(type, tag, nesting);
        token.block = true;
        if( nesting < 0) level--; // closing
        token.level = level;
        if( nesting > 0) level++; // opening
        tokens.push_back(token);
        return tokens.back();
    }

    class Token& tokens_back(){
        return tokens.back();
    }

    //template<int start,int end = 0>
    //void push_with_map(std::string_view type,std::string_view tag,int nesting){
        //push(type,tag,nesting);
        //tokens_back().map = {start,end};
    //}

    int skipEmptyLines(int from) {
        for( ; from < lineMax ; from++)
            if( !lineInfo[from].isEmpty()) break;
        return from;
    }
    int skipSpaces(int pos) {
        for (; pos < src.length(); pos++){
            if( !isSpace(src[pos])) break;
        }
        return pos;
    }
    // 在min位置之后的最小是space的位置
    int skipSpacesBack(int pos,int _min) {
        if( pos <=_min) return pos;
        while(pos > _min )
            if( !isSpace(src[--pos]))
                return pos+1;
        return pos;
    }


    /**
     * 起始,结束位置
     * [start,end)
     */
    std::string_view slice(size_t start,size_t end){
        return std::string_view(&src[start],end-start);
    }

    template<char ch>
    inline bool isChar(int pos){
        return src[pos] == ch; }

    int skipChars(int pos,char code ) {
        for (; pos < src.length(); pos++){
            if( src[pos] != code ) break;
        }
        return pos;
    }
    // [_min,pos)
    int skipCharsBack(int pos,char code,int _min) {
        if( pos <=_min) return pos;
        while(pos > _min )
            if( src[--pos] != code )
                return pos+1;
        return pos;
    }
    /**
     * indent 前面indent的都舍弃
     */
    std::string getLines(int begin,int end,const int indent,const bool keepLastLF)  {
        if( begin >= end) return "";
        std::stringstream ss;
        int lineIndent, ch, first, last, lineStart;
        for(; begin < end;++begin){ //一行一行遍历
            lineIndent = 0;
            first = lineInfo[begin].bMarks; //起点位置
            //如何确定last的位置?
            if( begin +1 < end || keepLastLF )
                last = lineInfo[begin].eMarks +1;
            else
                last = lineInfo[begin].eMarks; //一行的终点位置
            //计算这一行的indent
            while ( first < last && lineIndent < indent  ) {
                auto ch = static_cast<int>(src[first]);
                if( isSpace(ch)){
                    if( ch == '\t') lineIndent += 4 - (lineIndent + lineInfo[begin].bsCount) % 4;
                    else            lineIndent++;
                }
                else if( first - lineInfo[begin].bMarks < lineInfo[begin].tShift) //没有超过真正的空格
                    lineIndent++;
                else break;
                first++;
            }
            // \t\tfoobar with index=2
            // -> '  \tfoobar'
            if( lineIndent > indent){
                for(int j=1;j<=lineIndent-indent;++j) ss << " ";
            }
            for(int j=first;j<last;++j)        ss << src[j];
        }
        return ss.str();
    }

    std::string_view getLines_view(size_t begin,size_t end){
        return slice(lineInfo[begin].bMarks, lineInfo[end-1].eMarks);
    }

    //去除头部空格,与末尾的\n
    std::string_view getOneLineView_nospc(int line){
        auto pos  = lineInfo[line].first_nospace_pos();
        auto last = lineInfo[line].eMarks;
        return std::string_view(&src[pos],last-pos);
    }
    //void Token() ;
    template<int MAX_space = 4>
    inline bool isCodeBlock(int line){ return lineInfo[line].sCount - blkIndent >= MAX_space; }

    inline bool isEmpty(int line) { 
        if( line >=lineMax ) return true;
        return lineInfo[line].isEmpty(); 
    }

    inline bool nest_less_blkIndent(int line){ return lineInfo[line].sCount < blkIndent; }

    inline bool should_code_block(int line) { return lineInfo[line].sCount - blkIndent >=4; }

    inline
    auto charCodeAt(int pos){ assert(pos < src.length()); return uCodeChar(src[pos]);}

    
    inline bool isOutdented(int line) { return lineInfo[line].sCount < blkIndent;}


public:

    //using Array  = std::vector<int> ;
    //Array bMarks;   //每一行开始 在原src的位置
    //Array eMarks;   //每一行开始 在原src的位置
    //Array tShift;   //第一个非空白字符 offset tabs not expand
    //Array sCount;   //第一个非空白字符 offset tabs expand
    //Array bsCount;  //都是0
    std::vector<LineInfo> lineInfo;
    int blkIndent{0}; //需要的 block content 缩进
    int line{0};
    int lineMax{0};
    bool tight{false};
    int ddIndent{-1};
    int listIndent{-1}; //当前list block 的缩进
    std::string_view parentType{ parentType_root };
    int level{0};
    std::string result;

    MarkdownIt_base& md;
    std::string_view src;
    ENV env;
    TokenArrayRef tokens;

    //state_core
    bool inlineMode{false};

};

} // end namespace markdownItCpk

