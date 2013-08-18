var bind = require("./build/Release/binding");

module.exports = function Gumbo() {
  var ret = bind.apply(this, arguments);

  // extract the root tag from document resp
  try {
    ret.root = ret.document.childNodes[0];
  } catch(e) {
    // TODO: Say smth smart here
  }

  return ret;
};
