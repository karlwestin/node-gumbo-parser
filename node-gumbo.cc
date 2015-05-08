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

void record_location(Local<Object> node, GumboSourcePosition* pos, const char* name) {
  Local<Object> position = NanNew<Object>();
  position->Set(NanNew<String>("line"),
                NanNew<Number>(pos->line));
  position->Set(NanNew<String>("column"),
                NanNew<Number>(pos->column));
  position->Set(NanNew<String>("offset"),
                NanNew<Number>(pos->offset));
  node->Set(NanNew<String>(name), position);
}

Local<Object> read_attribute(GumboAttribute* attr) {
  Local<Object> obj = NanNew<Object>();
  obj->Set(NanNew<String>("nodeType"), NanNew<Integer>(2));
  obj->Set(NanNew<String>("name"), NanNew(attr->name));
  obj->Set(NanNew<String>("value"), NanNew(attr->value));

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
        NanThrowTypeError("Unknown tag namespace");
  }

  return NanNew<String>(namespace_name);
}

Local<Object> read_text(GumboNode* node) {
  Local<Object> obj = NanNew<Object>();
  Local<Integer> type = NanNew<Integer>(3);
  Local<String> name;
  switch(node->type) {
    case GUMBO_NODE_TEXT:
    case GUMBO_NODE_WHITESPACE:
        name = NanNew("#text");
        break;
    case GUMBO_NODE_CDATA:
        name = NanNew("#cdata-section");
        break;
  }
  obj->Set(NanNew<String>("nodeType"), type);
  obj->Set(NanNew<String>("nodeName"), name);
  obj->Set(NanNew<String>("textContent"), NanNew(node->v.text.text));

  record_location(obj, &node->v.text.start_pos, "startPos");
  return obj;
}

Local<Object> read_comment(GumboNode* node) {
  Local<Object> obj = NanNew<Object>();
  obj->Set(NanNew<String>("nodeType"), NanNew<Integer>(8));
  obj->Set(NanNew<String>("nodeName"), NanNew("#comment"));
  obj->Set(NanNew<String>("textContent"), NanNew(node->v.text.text));
  obj->Set(NanNew<String>("nodeValue"), NanNew(node->v.text.text));
  return obj;
}

Local<Object> read_element(GumboNode* node) {
  Local<Object> obj = NanNew<Object>();
  Local<String> tag = NanNew(gumbo_normalized_tagname(node->v.element.tag));


  obj->Set(NanNew<String>("originalTag"),
           NanNew(node->v.element.original_tag.data,
                  node->v.element.original_tag.length));

  obj->Set(NanNew<String>("originalEndTag"),
           NanNew(node->v.element.original_end_tag.data,
                  node->v.element.original_end_tag.length));

  if(tag->Length() == 0) {
    // if we don't have a tagname (i.e. it's a custom tag),
    // extract the tagname from the original text
    GumboStringPiece clone = node->v.element.original_tag;
    gumbo_tag_from_original_text(&clone);
    tag = NanNew(clone.data, clone.length);
  }

  obj->Set(NanNew<String>("nodeType"), NanNew<Integer>(1));
  obj->Set(NanNew<String>("nodeName"), tag);
  obj->Set(NanNew<String>("tagName"), tag);

  obj->Set(NanNew<String>("tagNamespace"),
       get_tag_namespace(node->v.element.tag_namespace));

   obj->Set(NanNew<String>("originalTag"),
            NanNew(node->v.element.original_tag.data,
                   node->v.element.original_tag.length));

   obj->Set(NanNew<String>("originalEndTag"),
            NanNew(node->v.element.original_end_tag.data,
                   node->v.element.original_end_tag.length));


  GumboVector* children = &node->v.element.children;
  Local<Array> childNodes = NanNew<Array>(children->length);
  if(children->length > 0) {
    for (unsigned int i = 0; i < children->length; ++i) {
        childNodes->Set(i, recursive_search(static_cast<GumboNode*>(children->data[i])));
    }
  }
  obj->Set(NanNew<String>("childNodes"), childNodes);

  GumboVector* attributes = &node->v.element.attributes;
  Local<Array> attrs = NanNew<Array>(attributes->length);
  if(attributes->length > 0) {
    for (unsigned int i = 0; i < attributes->length; ++i) {
        attrs->Set(i, read_attribute(static_cast<GumboAttribute*>(attributes->data[i])));
    }
  }
  obj->Set(NanNew<String>("attributes"), attrs);

  // set location if the element actually comes from the source
  if (node->v.element.original_tag.length == 0) {
      obj->Set(NanNew<String>("startPos"), NanUndefined());
  } else {
    record_location(obj, &node->v.element.start_pos, "startPos");
    record_location(obj, &node->v.element.end_pos, "endPos");
  }

  return obj;
}

Local<Object> read_document(GumboNode* node) {
  GumboDocument* doc = &node->v.document;
  Local<Object> obj = NanNew<Object>();
  obj->Set(NanNew<String>("nodeType"), NanNew<Integer>(9));
  obj->Set(NanNew<String>("nodeName"), NanNew("#document"));
  obj->Set(NanNew<String>("hasDoctype"), NanNew<Boolean>(doc->has_doctype));

  obj->Set(NanNew<String>("name"), NanNew(doc->name));
  obj->Set(NanNew<String>("publicIdentifier"), NanNew(doc->public_identifier));
  obj->Set(NanNew<String>("systemIdentifier"), NanNew(doc->system_identifier));

  GumboVector* children = &doc->children;
  Local<Array> childNodes = NanNew<Array>(children->length);
  if(children->length > 0) {
    for (unsigned int i = 0; i < children->length; ++i) {
        childNodes->Set(i, recursive_search(static_cast<GumboNode*>(children->data[i])));
    }
  }
  obj->Set(NanNew<String>("childNodes"), childNodes);
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
    NanScope();
    /*
     * getting options
     */
    Local<Object> config = Local<Object>::Cast(args[1]);
    Local<Value> s_o_f = config->Get(NanNew("stopOnFirstError"));
    bool stop_on_first_error = false;
    if (s_o_f->IsBoolean()) {
      stop_on_first_error = s_o_f->ToBoolean()->Value();
    }

    Local<Value> t_s = config->Get(NanNew("tabStop"));
    int tab_stop = 8;
    if (t_s->IsNumber()) {
      tab_stop = t_s->ToInteger()->Value();
    }

    /*
     * getting text
     */
    Local<Value> str = args[0];
    if(!str->IsString() ) {
      return NanThrowError("The first argument needs to be a string");
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
    Local<Value> fragment_context = config->Get(NanNew("fragmentContext"));
    Local<Value> fragment_namespace = config->Get(NanNew("fragmentNamespace"));

    if(fragment_context->IsString()) {
      GumboTag context = gumbo_tag_enum(*NanUtf8String(fragment_context->ToString()));
      GumboNamespaceEnum ns = GUMBO_NAMESPACE_HTML;
      if(fragment_namespace->IsString()) {
        char *nsString = *NanUtf8String(fragment_namespace->ToString());
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
    Local<Object> ret = NanNew<Object>();
    if(!fragment) {
      Local<Object> doc = recursive_search(output->document);
      ret->Set(NanNew<String>("document"), doc);
    } else {
      Local<Object> root = recursive_search(output->root);
      ret->Set(NanNew<String>("root"), root);
    }

    // TODO: Parse errors

    gumbo_destroy_output(&options, output);
    NanReturnValue(ret);
}

void init(Handle<Object> exports) {
    exports->Set(NanNew<String>("gumbo"),
      NanNew<FunctionTemplate>(Method)->GetFunction());
}

NODE_MODULE(binding, init);
