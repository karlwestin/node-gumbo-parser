var gumbo = require("../node-gumbo");
var fs = require("fs");
var assert = require("assert");

function tryParse(err, text) {
  if(err) {
    throw err;
  }
  console.log("Running tests...");

  var output = gumbo(text);

  assert(!!output, "should return a value");

  assert.equal(output.tagName, "html");
  console.log("Outputs an object");
  assert.equal(output.tagName, "html");
  console.log("Parses html tag");
  assert.equal(output.childNodes[0].tagName, "head");
  console.log("Parses head as a child of html");
  assert.equal(output.childNodes.length, 5);
  assert(!!output.childNodes[4].attributes[0].bgcolor, "should parse attrs");
  console.log("Parses attributes");

  console.log("...done!");

  console.log(output);
}
fs.readFile(__dirname + "/test.html", { encoding: "utf-8" }, tryParse);
