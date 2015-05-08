var extend = require("util")._extend;
var bind = require("./build/Release/binding");

module.exports = function Gumbo(text, config) {
  config = extend(config || {});

  // Work for old API;
  if(config.fragment === true) {
    config.fragmentContext = "body";
    config.fragmentNamespace = "html";
    delete config.fragment;
  }

  if(config.fragmentContext && !config.fragmentNamespace) {
    config.fragmentNamespace = "html";
  } else if (config.fragmentNamespace) {
    config.fragmentNamespace = config.fragmentNamespace.toLowerCase();
  }

  // Sanity check on namespace
  if("fragmentNamespace" in config &&
    ["svg", "html", "mathml"].indexOf(config.fragmentNamespace) === -1) {
    throw new Error("Invalid namespace: Valid namespaces are 'svg', 'html', 'mathml'");
  }

  var parsedDocument = bind.gumbo.call(this, text, config);

  if(config.fragmentContext) {
    return parsedDocument.root;
  }

  // extract the root tag from document resp
  try {
    parsedDocument.root = parsedDocument.document.childNodes[0];
  } catch(e) {
    // TODO: Say smth smart here
  }

  return parsedDocument;
};
