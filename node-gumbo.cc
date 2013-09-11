#include <stdlib.h>
#include <cstring>
#include <v8.h>
#include <node.h>
#include "deps/gumbo-parser/src/gumbo.h"

using namespace v8;

Local<Object> recursive_search(GumboNode* node);
Local<Object> read_text(GumboNode* node);
Local<Object> read_comment(GumboNode* node);
Local<Object> read_element(GumboNode* node);
Local<Object> read_attribute(GumboAttribute* attr);
Local<Object> read_document(GumboNode* node);

Local<Object> read_attribute(GumboAttribute* attr) {
  Local<Object> obj = Object::New();
  obj->Set(String::NewSymbol("nodeType"), Integer::New(2));
  obj->Set(String::NewSymbol("name"), String::New(attr->name));
  obj->Set(String::NewSymbol("value"), String::New(attr->value));
  return obj;
}

Local<Object> read_text(GumboNode* node) {
  Local<Object> obj = Object::New();
  Local<Integer> type = Integer::New(3);
  Local<String> name;
  switch(node->type) {
    case GUMBO_NODE_TEXT:
    case GUMBO_NODE_WHITESPACE:
        name = String::New("#text");
        break;
    case GUMBO_NODE_CDATA:
        name = String::New("#cdata-section");
  }
  obj->Set(String::NewSymbol("nodeType"), type);
  obj->Set(String::NewSymbol("nodeName"), name);
  obj->Set(String::NewSymbol("textContent"), String::New(node->v.text.text));
  return obj;
}

Local<Object> read_comment(GumboNode* node) {
  Local<Object> obj = Object::New();
  obj->Set(String::NewSymbol("nodeType"), Integer::New(8));
  obj->Set(String::NewSymbol("nodeName"), String::New("#comment"));
  obj->Set(String::NewSymbol("textContent"), String::New(node->v.text.text));
  obj->Set(String::NewSymbol("nodeValue"), String::New(node->v.text.text));
  return obj;
}

Local<Object> read_element(GumboNode* node) {
  Local<Object> obj = Object::New();
  Local<String> tag = String::New(gumbo_normalized_tagname(node->v.element.tag));
  obj->Set(String::NewSymbol("nodeType"), Integer::New(1));
  obj->Set(String::NewSymbol("nodeName"), tag);
  obj->Set(String::NewSymbol("tagName"), tag);

  GumboVector* children = &node->v.element.children;
  Local<Array> childNodes = Array::New(children->length);
  if(children->length > 0) {
    for (unsigned int i = 0; i < children->length; ++i) {
        childNodes->Set(i, recursive_search(static_cast<GumboNode*>(children->data[i])));
    }
  }
  obj->Set(String::NewSymbol("childNodes"), childNodes);

  GumboVector* attributes = &node->v.element.attributes;
  Local<Array> attrs = Array::New(attributes->length);
  if(attributes->length > 0) {
    for (unsigned int i = 0; i < attributes->length; ++i) {
        attrs->Set(i, read_attribute(static_cast<GumboAttribute*>(attributes->data[i])));
    }
  }
  obj->Set(String::NewSymbol("attributes"), attrs);

  return obj;
}

Local<Object> read_document(GumboNode* node) {
  GumboDocument* doc = &node->v.document;
  Local<Object> obj = Object::New();
  obj->Set(String::NewSymbol("nodeType"), Integer::New(9));
  obj->Set(String::NewSymbol("nodeName"), String::New("#document"));
  obj->Set(String::NewSymbol("hasDoctype"), Boolean::New(doc->has_doctype));

  obj->Set(String::NewSymbol("name"), String::New(doc->name));
  obj->Set(String::NewSymbol("publicIdentifier"), String::New(doc->public_identifier));
  obj->Set(String::NewSymbol("systemIdentifier"), String::New(doc->system_identifier));

  GumboVector* children = &doc->children;
  Local<Array> childNodes = Array::New(children->length);
  if(children->length > 0) {
    for (unsigned int i = 0; i < children->length; ++i) {
        childNodes->Set(i, recursive_search(static_cast<GumboNode*>(children->data[i])));
    }
  }
  obj->Set(String::NewSymbol("childNodes"), childNodes);

  return obj;
}

Local<Object> recursive_search(GumboNode* node) {
  switch(node->type) {
    case GUMBO_NODE_TEXT:
    case GUMBO_NODE_WHITESPACE:
    case GUMBO_NODE_CDATA:
        return read_text(node);
        break;

    case GUMBO_NODE_DOCUMENT:
        return read_document(node);
        break;

    case GUMBO_NODE_COMMENT:
        return read_comment(node);
        break;

    case GUMBO_NODE_ELEMENT:
        return read_element(node);
  }
}

Handle<Value> Method(const Arguments& args) {
    HandleScope scope;

    /*
     * getting options
     */
    Local<Object> config = Local<Object>::Cast(args[1]);
    Local<Value> s_o_f = config->Get(String::New("stopOnFirstError"));
    bool stop_on_first_error = false;
    if (s_o_f->IsBoolean()) {
      stop_on_first_error = s_o_f->ToBoolean()->Value();
    }

    Local<Value> t_s = config->Get(String::New("tabStop"));
    int tab_stop = 8;
    if (t_s->IsNumber()) {
      tab_stop = t_s->ToInteger()->Value();
    }

    /*
     * getting text 
     */
    Local<Value> str = args[0];
    if(!str->IsString() ) {
      return v8::ThrowException(v8::String::New("The first argument needs to be a string"));
    }

    v8::String::Utf8Value string(str);

    /*
     * creating options
     */
    GumboOptions options = kGumboDefaultOptions;
    options.tab_stop = tab_stop;
    options.stop_on_first_error = stop_on_first_error;

    GumboOutput* output = gumbo_parse_with_options(&options, *string, string.length());

    // root points to html tag
    // document points to document
    Local<Object> ret = Object::New();
    Local<Object> doc = recursive_search(output->document);
    ret->Set(String::NewSymbol("document"), doc);

    // TODO: Parse errors

    gumbo_destroy_output(&options, output);
    return scope.Close(ret);
} 

void init(Handle<Object> exports) {
    exports->Set(String::NewSymbol("gumbo"),
      FunctionTemplate::New(Method)->GetFunction());
}

NODE_MODULE(binding, init);
