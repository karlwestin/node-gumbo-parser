var gumbo = require("../node-gumbo");
var fs = require("fs");
var assert = require("assert");

function tryParse(err, text) {
  if(err) {
    throw err;
  }
  console.log("Running tests...");

  var output = gumbo(text);
  var root = output.root;
  console.log(output);

  assert(!!output, "should return a value");

  assert.equal(root.tagName, "html");
  console.log("roots an object");
  assert.equal(root.tagName, "html");
  console.log("Parses html tag");
  assert.equal(root.childNodes[0].tagName, "head");
  console.log("Parses head as a child of html");
  assert.equal(root.childNodes.length, 5);
  assert.equal(root.childNodes[4].attributes[0].name, "bgcolor", "should parse attrs");
  assert.equal(root.childNodes[4].attributes[0].value, "#ff0fff", "should parse attrs");
  console.log("Parses attributes");

  console.log("...done!");

}
fs.readFile(__dirname + "/test.html", { encoding: "utf-8" }, tryParse);
