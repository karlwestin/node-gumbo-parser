# Gumbo Parser

(npm install not fixed on windows yet)

Using [google's gumbo parser](https://github.com/google/gumbo-parser) to parse HTML in node.

```js
var gumbo = require("gumbo-parser");
var tree = gumbo(htmlstring);
```

### Usage
There's only one method: `gumbo(htmlstring)`. 

You can also pass in the options

```js
gumbo(htmlstring, {
  // The tab-stop size, for computing positions in source code that uses tabs.
  // default: 8
  tabStop: 8,
  // Whether or not to stop parsing when the first error is encountered.
  // default: false
  stopOnFirstError: true,

  // experimental: fragment parsing
  fragment: true
});
```

returns:

```js
{
  document: {
    // the document element (see below)
  },

  root: {
    // the html element (se 'Element' below)
  }
}
```

```
if you use fragment parsing:
{
  childNodes: [
    list
  ]
}

Element:
  nodeName (string) (same as tagname)
  nodeType (number) 1
  tagName (string)  (normalized to lowercase)
  originalTag (string) original text from tag
  originalTagEnd (string) original closing tag from original text, if there was one
  children (array) -> replicating childNodes rather than children,
                      ie all text / comment children are included
  tagNamespace (string) "HTML", "SVG" or "MATHML"
  attributes (array)
  startPos (position) -> if element is inserted by parser, this value is undefined
  endPos (position)

TextNode:
  nodeName (string) #text or #cdata-section
  nodeType (number) 3
  textContent (string)
  startPos (position)

Document:
  nodeName (string) #document
  nodeType (number) 9
  children (array)
  hasDoctype true/false
  name: (string)            -> see below
  publicIdentifier (string)       "
  systemIdentifier (string)       "

CommentNode
  nodeName (string) #comment
  nodeType (number) 8
  textContent (string) content comment
  nodeValue (string) same as textcontent

Attribute
  name: attribute name
  value: attribute value (currently always string, doh)
  nodeType: (number) 2
  nameStart: (position)
  nameEnd: (position)
  valueStart: (position)
  valueEnd: (position)

Position
  line:   number
  column: number
  offset: number

```

### About html doctypes

An html document will always have the `document.name` "html".
If the document has anything else in the type, for example this html4 doctype:

```html
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
        "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
```

the first part within quotation marks will end up in the `document.publicIdentifier`,
and the second part will be in `document.systemIdentifier`. You can read more about this here: [http://www.whatwg.org/specs/web-apps/current-work/multipage/syntax.html#syntax-doctype](http://www.whatwg.org/specs/web-apps/current-work/multipage/syntax.html#syntax-doctype).

### Untrusted content

If you plan on using gumbo-parser to clean user input, the gumbo parser is one of the most well-tested and audited parsers available. 
[Please read this comment from the gumbo-parsers authors.](https://github.com/google/gumbo-parser/issues/53#issuecomment-43395597)

### Build and test:

```bash
node-gyp configure
node-gyp build
npm test
```

## Changes
**0.1.12** io.js support!
           Thanks a lot to [MicroMike](https://github.com/mike820324)
            (unfortunately, this seems to have ended 0.8 support)

**0.1.11** Upgrade C lib
           Uses version 0.9.2, performance improvements, duplicate attributes, semicolon fix,
           [See all changes here](https://github.com/google/gumbo-parser/blob/master/CHANGES.md)  

**0.1.10** Visual Studio bugfix
           Thanks [takenspc](https://github.com/takenspc)

**0.1.9**  Experimental fragment parsing
           Expose node positions from the parser, which also enables the user
             to see if an element is inserted by the parser or was in the text
           Update gumbo parser to a more secure version
           Update statement about security

**0.1.8**  Fix for BSD build problem

**0.1.7**  Fixes for build on snow leopard

**0.1.6**  Adding originalTag, originalTagName and tagNamespace
           if the tag is unknown, parse originalTag and set in as tag

**0.1.5**  Updating the gumbo-parser to the latest version. This includes some security fixes, and if you use this
           for user content, please update.

**0.1.4**  Temporary workaround for the latest changes in node 0.11, thanks [Daniel](https://github.com/TheHydroImpulse)

**0.1.3**  Fixes utf-8 bug, thanks [Yonatan](https://github.com/yonatan)

**0.1.2**  Taking the (optional) options argument
           providing publicIdentifier and systemIdentifer for the doctype

**0.1.1**  Fix build on node 0.8

**0.1.0**  Passing { document: document, root: root } instead of only root
