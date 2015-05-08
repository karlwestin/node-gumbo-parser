var gumbo = require("../node-gumbo");
var reader = require("./reader");
var assert = require("assert");

reader("/test.html", function(text) {
  console.log("Running: normal");

  var output = gumbo(text);
  var root = output.root;

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

reader("/custom-tags.html", function(text) {
  console.log("Running: custom tags");
  var root = gumbo(text).root;

  assert.equal(root.childNodes[1].childNodes[1].tagName, "background");
  assert(/^<background/.test(root.childNodes[1].childNodes[1].originalTag));

  console.log("Handles custom tags");
  console.log("...done!");
});

reader("/fragment.html", function(text) {
  console.log("Running: fragment parsing");
  var fragment = gumbo(text, { fragment: true });

  assert.equal(fragment.childNodes.length, 4);
  assert.equal(fragment.childNodes[0].tagName, "form");
  assert.equal(fragment.childNodes[2].tagName, "br");

  console.log("handles fragment");

  var fragment2 = gumbo("<h1>Hi</h1><br>", { fragmentContext: "div" });

  assert.equal(fragment2.childNodes.length, 2);
  assert.equal(fragment2.childNodes[0].tagName, "h1");
  assert.equal(fragment2.childNodes[1].tagName, "br");

  console.log("handles fragment context");

  var fragment3 = gumbo("<title>Hi</title><desc>Greeting</desc>", { fragmentContext: "g", fragmentNamespace: "svg" });

  assert.equal(fragment3.childNodes.length, 2);
  assert.equal(fragment3.childNodes[0].tagNamespace, "SVG");
  assert.equal(fragment3.childNodes[0].tagName, "title");
  assert.equal(fragment3.childNodes[1].tagName, "desc");

  console.log("handles fragment namespace");

  console.log("...done");
});

reader("/cdata.html", function(text) {
  console.log("Running: CDATA parsing");
  var output = gumbo(text);

  var invalidCDATA = output.root.childNodes[2].childNodes[1].childNodes[0];
  assert.equal(invalidCDATA.nodeType, 8);
  assert.equal(invalidCDATA.nodeName, "#comment");
  console.log("handles in-html cdata as comments");

  var validCDATA = output.root.childNodes[2].childNodes[3].childNodes[1].childNodes[0];
  assert.equal(validCDATA.nodeType, 3);
  assert.equal(validCDATA.nodeName, "#cdata-section");
  console.log("handles in-mathml cdata as mathml");

  console.log("...done");
});
