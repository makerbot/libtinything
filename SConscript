import os

env = Environment(
  ENV = os.environ,
  tools=['default', 'mb_install'])

paths = {
    'json-cpp'  : '../json-cpp/',
    'zlib' : '../zlib-1.2.8/'
    }

env['CCFLAGS'] = [
    '-Wall',
    '-D__DEBUG'
    ]

cpp_path = [
    'src',
    paths['zlib']
]

env.MBDependsOnJsonCpp()

library_paths = [
    paths['zlib']
    ]

if env.MBIsWindows():
  runtime_paths = []
else:
  runtime_paths = [
      '/usr/lib'
  ]

libraries = [
    'z',
    'jsoncpp'
    ]


source_files =  Glob('src/*.cc') + Glob('src/miniunzip/*.c')

env['CPPPATH'] = cpp_path

tinything = env.MBSharedLibrary("tinything", source_files, LIBS=libraries, LIBPATH=library_paths, RPATH=runtime_paths)

env.MBInstallLib(tinything, 'tinything')
