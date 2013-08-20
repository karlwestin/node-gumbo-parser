var fs = require("fs");

module.exports = function(filename, callback) {
  function tryParse(err, text) {
    if(err) {
      throw err;
    }
    callback(text);
  }

  // API changed between 0.8 and 0.10
  var args = [__dirname + filename];
  if(fs.readFile.length == 2) {
    args.push("utf-8", tryParse);
  } else {
    args.push({ encoding: "utf-8" }, tryParse);
  }

  fs.readFile.apply(fs, args);
};
