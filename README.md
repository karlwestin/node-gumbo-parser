# Gumbo Parser

## npm install is currently only works on mac
(I expect to fix linux on the wknd)

Using [google's gumbo parser](https://github.com/google/gumbo-parser) to parse HTML in node.

```
var gumbo = require("gumbo-parser");
var tree = gumbo(htmlstring);
```

### Nodetypes:
Currently i only output the values i get from gumbo itself

**Element (3)** nodeType, tagName, children, attributes  
**TextNode (1)** nodeType, textContent  
**Document (0)** nodeType  
**Comment (8)** nodeType, textContent, nodeValue  

### Build and test:
```
node-gyp configure
node-gyp build
npm test
```
