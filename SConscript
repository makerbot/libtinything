# vim:ai:et:ff=unix:fileencoding=utf-8:sw=4:syntax=python:ts=4:

import os

platform_args = {} if os.environ.get('BW_NATIVE_BUILD') else {'platform': 'posix'}

env = Environment(
    ENV=os.environ, tools=['default', 'birdwing_install', 'birdwing_build'],
    toolpath=['#/../bw-scons-tools'], **platform_args
)

env.BWSetCompilerToGcc()

env.BWDependsOnJsonCpp()

env.BWAddIncludePath('include')
env.BWAddIncludePath('src/miniunzip')
env.BWAddIncludePath('src/zlib')

env.Append(CPPDEFINES={'TINYTHING_API': ''})

source_files =  Glob('src/*.cc') + Glob('src/miniunzip/*.c') + Glob('src/zlib/*c')

ltinything = env.SharedLibrary('tinything', source_files)
env.Clean(ltinything, '#/obj')
env.BWInstallLibrary(ltinything)

