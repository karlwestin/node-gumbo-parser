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
  name: (string) Currently figuring this out

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

### Build and test:
```
node-gyp configure
node-gyp build
npm test
```
