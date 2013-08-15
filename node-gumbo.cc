#include <v8.h>
#include <node.h>
#include "deps/gumbo-parser/src/gumbo.h"

using namespace v8;

Local<Object> get_attribute(GumboAttribute* attr) {
  Local<Object> obj = Object::New();
  obj->Set(String::NewSymbol("nodeType"), Integer::New(2));
  obj->Set(String::NewSymbol(attr->name), String::New(attr->value));
  return obj;
}

Local<Object> recursive_search(GumboNode* node) {
  Local<Object> obj = Object::New();

  switch(node->type) {
    case GUMBO_NODE_TEXT:
    case GUMBO_NODE_WHITESPACE:
    case GUMBO_NODE_CDATA:
        obj->Set(String::NewSymbol("nodeType"), Integer::New(3));
        obj->Set(String::NewSymbol("textContent"), String::New(node->v.text.text));
        return obj;
        break;


    case GUMBO_NODE_DOCUMENT:
        obj->Set(String::NewSymbol("nodeType"), Integer::New(9));
        return obj;
        break;

    case GUMBO_NODE_COMMENT:
        obj->Set(String::NewSymbol("nodeType"), Integer::New(8));
        obj->Set(String::NewSymbol("textContent"), String::New(node->v.text.text));
        obj->Set(String::NewSymbol("nodeValue"), String::New(node->v.text.text));
        return obj;
        break;

    case GUMBO_NODE_ELEMENT:
        obj->Set(String::NewSymbol("nodeType"), Integer::New(1));
        obj->Set(String::NewSymbol("tagName"), 
                String::New(gumbo_normalized_tagname(node->v.element.tag)));

  }

  GumboVector* children = &node->v.element.children;
  Local<Array> childNodes = Array::New(children->length);
  if(children->length > 0) {
    for (int i = 0; i < children->length; ++i) {
        childNodes->Set(i, recursive_search(static_cast<GumboNode*>(children->data[i])));
    }
  }
  obj->Set(String::NewSymbol("childNodes"), childNodes);

  GumboVector* attributes = &node->v.element.attributes;
  Local<Array> attrs = Array::New(attributes->length);
  if(attributes->length > 0) {
    for (int i = 0; i < attributes->length; ++i) {
        attrs->Set(i, get_attribute(static_cast<GumboAttribute*>(attributes->data[i])));
    }
  }
  obj->Set(String::NewSymbol("attributes"), attrs);
  
  return obj;
}

Handle<Value> Method(const Arguments& args) {
    HandleScope scope;

    Local<Value> str = args[0];

    if(!str->IsString() ) {
      return v8::ThrowException(v8::String::New("Gumbo argument needs to be a string"));
    }

    v8::String::AsciiValue string(str);
    char *str2 = (char *) malloc(string.length() + 1);
    strcpy(str2, *string);

    GumboOutput* output = gumbo_parse(str2);

    // root points to html tag
    // document points to document
    GumboNode* root = output->root;
    Local<Object> ret = recursive_search(root);

    gumbo_destroy_output(&kGumboDefaultOptions, output);
    return scope.Close(ret);
} 

void init(Handle<Object> target) {
    NODE_SET_METHOD(target, "gumbo", Method);
}

NODE_MODULE(binding, init);
