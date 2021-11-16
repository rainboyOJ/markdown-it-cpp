#pragma once
#include <string>
#include <sstream>

//#include "../MarkdownIt.hpp"
#include "../define.hpp"
#include "../token.hpp"
#include "../common/utils.hpp"

namespace markdownItCpp {

class StateCore {
public:
    StateCore(std::string& src,MarkdownIt_base& md,ENV env,
            TokenArrayRef tokens
            )
      :src{src},md{md},env{env},tokens{tokens}
    {}

    MarkdownIt_base& md;
    std::string& src;
    ENV env;
    TokenArrayRef tokens;
    bool inlineMode{false};
};

}
