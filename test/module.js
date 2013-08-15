var gumbo = require("../node-gumbo");
var fs = require("fs");

module.exports = function(cb) {
  function tryParse(err, text) {
    if(err) {
      throw err;
    }
    var output = gumbo(text);
    cb(output);
  }
  fs.readFile(__dirname + "/test.html", { encoding: "utf-8" }, tryParse);
};
