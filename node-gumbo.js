var bind = require("./build/Release/binding");

module.exports = function Gumbo(text, config) {
  config = config || {};
  var ret = bind.gumbo.call(this, text, config);

  // extract the root tag from document resp
  try {
    ret.root = ret.document.childNodes[0];
  } catch(e) {
    // TODO: Say smth smart here
  }

  return ret;
};
