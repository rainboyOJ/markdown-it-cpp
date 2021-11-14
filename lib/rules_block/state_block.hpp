
#pragma once
#include <string>
#include <sstream>

#include "../define.hpp"
#include "../token.hpp"
#include "../common/utils.hpp"

namespace markdownItCpp {

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
            ch = static_cast<int>(src[pos]);

            if( !indent_found ) {
                if(isSpace(ch)){
                    indent++;
                    if(ch == '\t')
                        offset += 4 - offset % 4;
                    else
                        offset++;
                    continue;
                }
            }
            else
                indent_found = true;

            if( ch == '\n' || pos == len - 1){
                if( ch != '\n') pos++;
                bMarks.push_back(start);
                eMarks.push_back(pos);
                tShift.push_back(indent);
                sCount.push_back(offset);
                bsCount.push_back(0);

                indent_found = false;
                indent = 0;
                offset = 0;
                start = pos+1;
            }
        }
        //last Fake Line
        bMarks.push_back(len);
        eMarks.push_back(len);
        tShift.push_back(0);
        sCount.push_back(0);
        bsCount.push_back(0);

        lineMax = bMarks.size()-1; //don't count last fake line

    }

    class Token push(std::string type,std::string tag,int nesting)  {
        class Token token(type, tag, nesting);
        token.block = true;
        if( nesting < 0) level--; // closing
        token.level = level;
        if( nesting > 0) level++; // opening
        tokens.push_back(token);
        return token;
    }

    bool isEmpty(int line) {
        return bMarks[line] + tShift[line] >= eMarks[line];
    }
    int skipEmptyLines(int from) {
        for( ; from < lineMax ; from++)
            if( !isEmpty(line)) break;
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
    int skipChars(int pos,char code ) {
        for (; pos < src.length(); pos++){
            if( src[pos] != code ) break;
        }
        return pos;
    }
    int skipCharsBack(int pos,char code,int _min) {
        if( pos <=_min) return pos;
        while(pos > _min )
            if( src[--pos] != code )
                return pos+1;
        return pos;
    }
    std::string getLines(int begin,int end,int indent,bool keepLastLF)  {
        if( begin >= end) return "";
        std::stringstream ss;
        int  lineIndent, ch, first, last, lineStart,
             line = begin;
        for(int i=0; line < end;++i,++line){
            lineIndent = 0;
            lineStart = first = bMarks[line];
            //如何确定last的位置?
            if( line +1 < end || keepLastLF )
                last = eMarks[line] +1;
            else
                last = eMarks[line];
            //计算这一行的indent
            while ( first < last && lineIndent < indent  ) {
                auto ch = static_cast<int>(src[first]);
                if( isSpace(ch)){
                    if( ch == '\t')
                        lineIndent += 4 - (lineIndent + bsCount[line]) % 4;
                    else
                        lineIndent++;
                }
                else if( first - lineStart < tShift[line])
                    lineIndent++;
                else break;
                first++;
            }
            // \t\tfoobar with index=2
            // -> '  \tfoobar'
            if( lineIndent > indent){
                for(int j=1;j<=lineIndent-indent;++j) ss << " ";
            }
            for(int j=first;j<=last;++j)        ss << src[j];
        }
        return ss.str();
    }
    //void Token() ;

public:

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
    std::string_view src;
    ENV env;
    TokenArrayRef tokens;

    //state_core
    bool inlineMode{false};

};

} // end namespace markdownItCpk

