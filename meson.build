project('jr-embed', ['cpp', 'c'],
  default_options: [
    'cpp_std=c++20',
    'warning_level=3',
    'buildtype=release',
    'cpp_eh=none',
    'cpp_rtti=false',
  ])

if not meson.is_subproject()
  add_global_arguments('-Wno-c99-extensions', language: 'cpp')
  add_global_arguments('-Wno-dollar-in-identifier-extension', language: 'cpp')
  add_global_arguments('-Wno-gnu-auto-type', language: 'cpp')
  add_global_arguments('-Wno-gnu-case-range', language: 'cpp')
  add_global_arguments('-Wno-gnu-conditional-omitted-operand', language: 'cpp')
  add_global_arguments('-Wno-gnu-statement-expression', language: 'cpp')
endif

cxx = meson.get_compiler('cpp')
clang_cpp_lib = cxx.find_library('libclang-cpp', required: true)
llvm_lib = cxx.find_library('libLLVM', required: true)

jr_embed_plugin = shared_library('jr-embed', [
    'src/plugin/main.cpp',
  ],
  name_prefix: '',
  name_suffix: 'ext',
  include_directories: 'src/plugin',
  dependencies: [clang_cpp_lib, llvm_lib]
  )

if not meson.is_subproject()
  python_exe = find_program('python', 'python3', required: true)
  make_sure_embed_plugin_exists = custom_target(
    'make-sure-embed-plugin-exists',
    output: 'make-sure-embed-plugin-exists',
    input: jr_embed_plugin,
    command: [
      python_exe,
      '-c',
      'open("@OUTPUT@", "w+")'
    ])

  plugin_test_exe = executable('plugin-test', [
    'src/test/main.cpp',
    make_sure_embed_plugin_exists,
    ],
    cpp_args: [
      '-fplugin=jr-embed.ext',
      '-Wno-unused-command-line-argument',
      '-Wno-unknown-pragmas',
    ])
endif
