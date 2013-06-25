import os

env = Environment(ENV = os.environ)

paths = {
    'boost'     : '../../../boost_1_51_0/',
    'common'    : '../common',
    'json-cpp'  : '../../../json-cpp/',
    'libstepper': '../libstepper',
    'machine'   : '../machine',
    'meta'      : '../meta',
    'parser'    : '../parser',
    'pru_loader': '../../../pru_sw/app_loader',
    'yajl'      : '../../../yajl/'
    }

install_dir = ARGUMENTS.get('install_dir', '/usr/lib') 

env['CCFLAGS'] = [
    '-Wall',
    '-D__DEBUG'
    ]

cpp_path = [
    'src',
    paths['common'],
    paths['json-cpp'] + '/include'
]

library_paths = [
    paths['json-cpp'] + '/obj'
    ]

runtime_paths = [
    '/usr/lib'
]

libraries = [
    'jsoncpp'
    ]


source_files =  Glob('src/*.cc') + Glob('src/miniunzip/*.c')

env['CPPPATH'] = cpp_path

tinything = env.SharedLibrary("lib/tinything", source_files, LIBS=libraries, LIBPATH=library_paths, RPATH=runtime_paths)
Alias("install", env.Install(install_dir, tinything))
