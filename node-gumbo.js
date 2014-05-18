var bind = require("./build/Release/binding");

/*
 * very simplified fragment parser
 * basically looks for a string position,
 * and return the values that actually comes from the document
 */
function getFragment(nodes) {
  return nodes.reduce(function(sum, node) {
    // startPos is an object if it exists
    if(node.startPos) {
      sum.push(node);
    } else {
      return getFragment(node.childNodes);
    }

    return sum;

  }, []);
}

module.exports = function Gumbo(text, config) {
  config = config || {};
  var parsedDocument = bind.gumbo.call(this, text, config);

  // extract the root tag from document resp
  try {
    parsedDocument.root = parsedDocument.document.childNodes[0];
  } catch(e) {
    // TODO: Say smth smart here
  }

  if(!config.fragment) {
    return parsedDocument;
  }

  return {
    childNodes: getFragment(parsedDocument.document.childNodes)
  };
};
