{
  "targets": [{
    "target_name": "binding",
    "sources": ["node-gumbo.cc"],
    'dependencies': [
      'deps/gumbo-parser/gumbo-parser.gyp:gumbo'
    ],
	'include_dirs': [
	"<!(node -e \"require('nan')\")"
	]
  }]
}
