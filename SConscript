import os

env = Environment(ENV = os.environ)

paths = {
    'json-cpp'  : '../json-cpp/',
    'zlib' : '../zlib-1.2.7/'
    }

install_dir = ARGUMENTS.get('install_dir', '/usr/lib') 

env['CCFLAGS'] = [
    '-Wall',
    '-D__DEBUG'
    ]

cpp_path = [
    'src',
    paths['zlib'],
    paths['json-cpp'] + '/include'
    
]

library_paths = [
    paths['zlib'],
    paths['json-cpp'] + '/obj'
    ]

runtime_paths = [
    '/usr/lib'
]

libraries = [
    'z',
    'jsoncpp'
    ]


source_files =  Glob('src/*.cc') + Glob('src/miniunzip/*.c')

env['CPPPATH'] = cpp_path

tinything = env.SharedLibrary("lib/tinything", source_files, LIBS=libraries, LIBPATH=library_paths, RPATH=runtime_paths)
Alias("install", env.Install(install_dir, tinything))
