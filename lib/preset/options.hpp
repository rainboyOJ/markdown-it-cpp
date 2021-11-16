
#pragma once
namespace markdownItCpp {
struct optionsType {
    
  bool html;
  // - `false`. Set `true` to enable HTML tags in source. Be careful!
  //That's not safe! You may need external sanitizer to protect output from XSS.
  //It's better to extend features via plugins, instead of enabling HTML.
  bool xhtmlOut;
  //- `false`. Set `true` to add '/' when closing single tags
    //(`<br />`). This is needed only for full CommonMark compatibility. In real
    //world you will need HTML output.
  bool breaks{false}; // `false`. Set `true` to convert `\n` in paragraphs into `<br>`.
  std::string langPrefix{"language-"};// - `language-`. CSS language class prefix for fenced blocks.
    //Can be useful for external highlighters.

  bool linkify; // - `false`. Set `true` to autoconvert URL-like text to links.
  bool typographer;  //- `false`. Set `true` to enable [some language-neutral
    //replacement](https://github.com/markdown-it/markdown-it/blob/master/lib/rules_core/replacements.js) +
    //quotes beautification (smartquotes).
  std::string quotes; // - `“”‘’`, String or Array. Double + single quotes replacement
    //pairs, when typographer enabled and smartquotes on. For example, you can
    //use `'«»„“'` for Russian, `'„“‚‘'` for German, and
    //`['«\xA0', '\xA0»', '‹\xA0', '\xA0›']` for French (including nbsp).
  //highlight__ - `null`. Highlighter function for fenced code blocks.
    //Highlighter `function (str, lang)` should return escaped HTML. It can also
    //return empty string if the source was not changed and should be escaped
    //externaly. If result starts with <pre... internal wrapper is skipped.
    int maxNesting;
};

} // end namespace markdownItCpp
