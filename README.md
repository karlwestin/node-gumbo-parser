# Gumbo Parser

(npm install not fixed on windows yet)

Using [google's gumbo parser](https://github.com/google/gumbo-parser) to parse HTML in node.

```
var gumbo = require("gumbo-parser");
var tree = gumbo(htmlstring);
```

### Usage
```
There's only one method:
gumbo(htmlstring) 

You can also pass in the options
gumbo(htmlstring, {
  // The tab-stop size, for computing positions in source code that uses tabs.
  // default: 8
  tabStop: 8,
  // Whether or not to stop parsing when the first error is encountered.
  // default: false
  stopOnFirstError: true
});

returns:

{
  document: {
    // the document element (see below)
  },

  root: {
    // the html element (se 'Element' below)
  }
}

Element:
  nodeName (string) (same as tagname)
  nodeType (number) 1
  tagName (string)  (normalized to lowercase)
  children (array) -> replicating childNodes rather than children,
                      ie all text / comment children are included
  attributes (array)

TextNode:
  nodeName (string) #text or #cdata-section
  nodeType (number) 3
  textContent (string)

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
```

### About html doctypes

An html document will always have the `document.name` "html".
If the document has anything else in the type, for example this html4 doctype:

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
        "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">

the first part within quotation marks will end up in the `document.publicIdentifier`,
and the second part will be in `document.systemIdentifier`. You can read more about this here: [http://www.whatwg.org/specs/web-apps/current-work/multipage/syntax.html#syntax-doctype](http://www.whatwg.org/specs/web-apps/current-work/multipage/syntax.html#syntax-doctype).

### Build and test:
```
node-gyp configure
node-gyp build
npm test
```

## Changes

**0.1.4** Temporary workaround for the latest changes in node 0.11, thanks [Daniel](https://github.com/TheHydroImpulse)

**0.1.3** Fixes utf-8 bug, thanks [Yonatan](https://github.com/yonatan)

**0.1.2** Taking the (optional) options argument
          providing publicIdentifier and systemIdentifer for the doctype

**0.1.1** Fix build on node 0.8

**0.1.0** Passing { document: document, root: root } instead of only root
