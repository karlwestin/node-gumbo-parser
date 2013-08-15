{
  "targets": [{
    "target_name": "binding",
    "sources": ["node-gumbo.cc"],
   'libraries': [
      '<!@(pkg-config --libs gumbo)'
    ]
  }]
}
