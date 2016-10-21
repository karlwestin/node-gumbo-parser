#include <stdlib.h>
#include <cstring>
#include <nan.h>
#include "deps/gumbo-parser/src/gumbo.h"

using namespace v8;

Local<Object> recursive_search(GumboNode* node);
Local<Object> read_text(GumboNode* node);
Local<Object> read_comment(GumboNode* node);
Local<Object> read_element(GumboNode* node);
Local<Object> read_attribute(GumboAttribute* attr);
Local<Object> read_document(GumboNode* node);

void record_location(v8::Local<Object> node, GumboSourcePosition* pos, const char* name) {
  v8::Local<Object> position = Nan::New<v8::Object>();
  Nan::Set(position,
           Nan::New("line").ToLocalChecked(),
           Nan::New(pos->line));
  Nan::Set(position,
           Nan::New("column").ToLocalChecked(),
           Nan::New(pos->column));
  Nan::Set(position,
           Nan::New("offset").ToLocalChecked(),
           Nan::New(pos->offset));

  Nan::Set(node, Nan::New(name).ToLocalChecked(), position);
}

Local<Object> read_attribute(GumboAttribute* attr) {
  v8::Local<Object> obj = Nan::New<v8::Object>();
  Nan::Set(obj, Nan::New("nodeType").ToLocalChecked(), Nan::New<Integer>(2));
  Nan::Set(obj, Nan::New("name").ToLocalChecked(), Nan::New(attr->name).ToLocalChecked());
  Nan::Set(obj, Nan::New("value").ToLocalChecked(), Nan::New(attr->value).ToLocalChecked());

  record_location(obj, &attr->name_start, "nameStart");
  record_location(obj, &attr->name_end, "nameEnd");

  record_location(obj, &attr->value_start, "valueStart");
  record_location(obj, &attr->value_end, "valueEnd");
  return obj;
}

// thanks @drd https://github.com/drd/gumbo-node/blob/master/gumbo.cc#L162
Handle<Value> get_tag_namespace(GumboNamespaceEnum tag_namespace) {
  const char* namespace_name;

  switch (tag_namespace) {
    case GUMBO_NAMESPACE_HTML:
        namespace_name = "HTML";
        break;
    case GUMBO_NAMESPACE_SVG:
        namespace_name = "SVG";
        break;
    case GUMBO_NAMESPACE_MATHML:
        namespace_name = "MATHML";
        break;
    default:
        Nan::ThrowTypeError("Unknown tag namespace");
  }

  return Nan::New(namespace_name).ToLocalChecked();
}

Local<Object> read_text(GumboNode* node) {
  Local<Object> obj = Nan::New<v8::Object>();
  Local<Integer> type = Nan::New<v8::Integer>(3);
  const char* name;
  switch(node->type) {
    case GUMBO_NODE_TEXT:
    case GUMBO_NODE_WHITESPACE:
        name = "#text";
        break;
    case GUMBO_NODE_CDATA:
        name = ("#cdata-section");
        break;
  }

  Nan::Set(obj, Nan::New("nodeType").ToLocalChecked(), type);
  Nan::Set(obj, Nan::New("nodeName").ToLocalChecked(), Nan::New(name).ToLocalChecked());
  Nan::Set(obj, Nan::New("textContent").ToLocalChecked(), Nan::New(node->v.text.text).ToLocalChecked());
  Nan::Set(obj,
           Nan::New("originalText").ToLocalChecked(),
           Nan::New(node->v.text.original_text.data,
                  node->v.text.original_text.length).ToLocalChecked());

  record_location(obj, &node->v.text.start_pos, "startPos");
  return obj;
}

Local<Object> read_comment(GumboNode* node) {
  Local<Object> obj = Nan::New<v8::Object>();
  Nan::Set(obj, Nan::New("nodeType").ToLocalChecked(), Nan::New<v8::Integer>(8));
  Nan::Set(obj, Nan::New("nodeName").ToLocalChecked(), Nan::New("#comment").ToLocalChecked());
  Nan::Set(obj, Nan::New("textContent").ToLocalChecked(), Nan::New(node->v.text.text).ToLocalChecked());
  Nan::Set(obj, Nan::New("nodeValue").ToLocalChecked(), Nan::New(node->v.text.text).ToLocalChecked());
  return obj;
}

Local<String> get_tag_name(GumboNode* node) {
  const char* tagName = gumbo_normalized_tagname(node->v.element.tag);
  if(strlen(tagName) > 0) {
    return Nan::New(tagName).ToLocalChecked();
  }

  // if we don't have a tagname (i.e. it's a custom tag),
  // extract the tagname from the original text
  GumboStringPiece clone = node->v.element.original_tag;
  gumbo_tag_from_original_text(&clone);
  return Nan::New(clone.data, clone.length).ToLocalChecked();
}

Local<Object> read_element(GumboNode* node) {
  Local<Object> obj = Nan::New<v8::Object>();


  Nan::Set(obj,
           Nan::New("originalTag").ToLocalChecked(),
           Nan::New(node->v.element.original_tag.data,
                  node->v.element.original_tag.length).ToLocalChecked());

  Nan::Set(obj,
           Nan::New("originalEndTag").ToLocalChecked(),
           Nan::New(node->v.element.original_end_tag.data,
                  node->v.element.original_end_tag.length).ToLocalChecked());

  Local<String> tag = get_tag_name(node);

  Nan::Set(obj, Nan::New("nodeType").ToLocalChecked(), Nan::New<v8::Integer>(1));
  Nan::Set(obj, Nan::New("nodeName").ToLocalChecked(), tag);
  Nan::Set(obj, Nan::New("tagName").ToLocalChecked(), tag);

  Nan::Set(obj, Nan::New("tagNamespace").ToLocalChecked(), get_tag_namespace(node->v.element.tag_namespace));

  Nan::Set(obj,
           Nan::New("originalTag").ToLocalChecked(),
           Nan::New(node->v.element.original_tag.data,
                   node->v.element.original_tag.length).ToLocalChecked());

  Nan::Set(obj,
           Nan::New("originalEndTag").ToLocalChecked(),
           Nan::New(node->v.element.original_end_tag.data,
                   node->v.element.original_end_tag.length).ToLocalChecked());


  GumboVector* children = &node->v.element.children;
  Local<Array> childNodes = Nan::New<v8::Array>(children->length);
  if(children->length > 0) {
    for (unsigned int i = 0; i < children->length; ++i) {
        childNodes->Set(i, recursive_search(static_cast<GumboNode*>(children->data[i])));
    }
  }

  Nan::Set(obj, Nan::New("childNodes").ToLocalChecked(), childNodes);

  GumboVector* attributes = &node->v.element.attributes;
  Local<Array> attrs = Nan::New<v8::Array>(attributes->length);
  if(attributes->length > 0) {
    for (unsigned int i = 0; i < attributes->length; ++i) {
        attrs->Set(i, read_attribute(static_cast<GumboAttribute*>(attributes->data[i])));
    }
  }

  Nan::Set(obj, Nan::New("attributes").ToLocalChecked(), attrs);

  // set location if the element actually comes from the source
  if (node->v.element.original_tag.length == 0) {
      Nan::Set(obj, Nan::New("startPos").ToLocalChecked(), Nan::Undefined());
  } else {
    record_location(obj, &node->v.element.start_pos, "startPos");
    record_location(obj, &node->v.element.end_pos, "endPos");
  }

  return obj;
}

Local<Object> read_document(GumboNode* node) {
  GumboDocument* doc = &node->v.document;
  Local<Object> obj = Nan::New<v8::Object>();
  Nan::Set(obj, Nan::New("nodeType").ToLocalChecked(), Nan::New<v8::Integer>(9));
  Nan::Set(obj, Nan::New("nodeName").ToLocalChecked(), Nan::New("#document").ToLocalChecked());
  Nan::Set(obj, Nan::New("hasDoctype").ToLocalChecked(), Nan::New<v8::Boolean>(doc->has_doctype));
           obj,
  Nan::Set(obj, Nan::New("name").ToLocalChecked(), Nan::New(doc->name).ToLocalChecked());
  Nan::Set(obj, Nan::New("publicIdentifier").ToLocalChecked(), Nan::New(doc->public_identifier).ToLocalChecked());
  Nan::Set(obj, Nan::New("systemIdentifier").ToLocalChecked(), Nan::New(doc->system_identifier).ToLocalChecked());

  GumboVector* children = &doc->children;
  Local<Array> childNodes = Nan::New<v8::Array>(children->length);
  if(children->length > 0) {
    for (unsigned int i = 0; i < children->length; ++i) {
        childNodes->Set(i, recursive_search(static_cast<GumboNode*>(children->data[i])));
    }
  }
  Nan::Set(obj, Nan::New("childNodes").ToLocalChecked(), childNodes);
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

NAN_METHOD(Method) {
    /*
     * getting options
     */
    Local<Object> config = Local<Object>::Cast(info[1]);
    Local<Value> s_o_f = Nan::Get(config, Nan::New("stopOnFirstError").ToLocalChecked()).ToLocalChecked();
    bool stop_on_first_error = false;
    if (s_o_f->IsBoolean()) {
      stop_on_first_error = s_o_f->ToBoolean()->Value();
    }

    Local<Value> t_s = Nan::Get(config, Nan::New("tabStop").ToLocalChecked()).ToLocalChecked();
    int tab_stop = 8;
    if (t_s->IsNumber()) {
      tab_stop = t_s->ToInteger()->Value();
    }

    /*
     * getting text
     */
    Local<Value> str = info[0];
    if(!str->IsString() ) {
      return Nan::ThrowError("The first argument needs to be a string");
    }

    v8::String::Utf8Value string(str);

    bool fragment = false;
    /*
     * creating options
     */
    GumboOptions options = kGumboDefaultOptions;
    options.tab_stop = tab_stop;
    options.stop_on_first_error = stop_on_first_error;

    /*
     * find out context for fragment parsing
     */
    Local<Value> fragment_context = Nan::Get(config, Nan::New("fragmentContext").ToLocalChecked()).ToLocalChecked();
    Local<Value> fragment_namespace = Nan::Get(config, Nan::New("fragmentNamespace").ToLocalChecked()).ToLocalChecked();

    if(fragment_context->IsString()) {
      Nan::Utf8String f_c(fragment_context->ToString());
      GumboTag context = gumbo_tag_enum(*f_c);
      GumboNamespaceEnum ns = GUMBO_NAMESPACE_HTML;
      if(fragment_namespace->IsString()) {
        Nan::Utf8String f_n(fragment_namespace->ToString());
        char *nsString = *f_n;
        if(strcmp(nsString, "svg") == 0) {
          ns = GUMBO_NAMESPACE_SVG;
        } else if (strcmp(nsString, "mathml") == 0) {
          ns = GUMBO_NAMESPACE_MATHML;
        } else {
          ns = GUMBO_NAMESPACE_HTML;
        }
      }

      options.fragment_context = context;
      options.fragment_namespace = ns;
      fragment = true;
    }

    GumboOutput* output = gumbo_parse_with_options(&options, *string, string.length());

    // root points to html tag
    // document points to document
    Local<Object> ret = Nan::New<Object>();
    if(!fragment) {
      Local<Object> doc = recursive_search(output->document);
      Nan::Set(ret, Nan::New("document").ToLocalChecked(), doc);
    } else {
      Local<Object> root = recursive_search(output->root);
      Nan::Set(ret, Nan::New("root").ToLocalChecked(), root);
    }

    // TODO: Parse errors

    gumbo_destroy_output(&options, output);
    info.GetReturnValue().Set(ret);
}

void init(Handle<Object> exports) {
    Nan::Set(exports,
             Nan::New("gumbo").ToLocalChecked(),
             Nan::New<FunctionTemplate>(Method)->GetFunction());
}

NODE_MODULE(binding, init);
