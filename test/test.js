var gumbo = require("../node-gumbo");
var reader = require("./reader");
var assert = require("assert");

reader("/test.html", function(text) {
  console.log("Running: normal");

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
});

reader("/parse-error.html", function(text) {
  console.log("Running: stopOnFirstError");

  var output = gumbo(text, { stopOnFirstError: true });
  assert.equal(output.root, undefined);
  console.log("stopped on error when option is passed");

  var output2 = gumbo(text);
  assert.notEqual(output2.root, undefined);
  console.log("didn't stop on error by default");

  console.log("...done!");
});

reader("/legacy-doctype.html", function(text) {
  console.log("Running: legacy doctypes");
  var output = gumbo(text).document; 

  assert(output.systemIdentifier, "system parameter is parsed");
  assert(output.publicIdentifier, "public parameter is parsed");

  console.log("added systemIdentifier and publicIdentifier");
  console.log("...done!");
});

reader("/unicode.html", function(text) {
  console.log("Running: unicode");
  var output = gumbo(text);
  assert.equal(output.root.attributes[0].value, "Héɭｌｏ, ɰòｒｌᏧ");

  console.log("Handles non-ascii characters");
  console.log("...done!");
});
