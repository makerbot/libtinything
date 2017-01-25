# vim:ai:et:ff=unix:fileencoding=utf-8:sw=4:syntax=python:ts=4:

import os

env = Environment(
    ENV=os.environ, tools=['default', 'birdwing_install', 'birdwing_build'],
    toolpath=['#/../bw-scons-tools'],
)

env.BWSetCompilerToGCC()
env.BWDependsOnJsonCpp()

env.BWDependsOnBWCodegenSharedCPP()
env.BWDependsOnMBCoreUtils()

env.BWPyInstallPkg('../python/tinything')

env.BWAddIncludePath('include')
env.BWAddIncludePath('src/miniunzip')
env.BWAddIncludePath('src/zlib')

env.Append(CPPDEFINES={'TINYTHING_API': ''})

source_files = [
    Glob('src/zlib/*.c'),
    Glob('src/miniunzip/*.c'),
    Glob('src/*.cc'),
]

ltinything = env.SharedLibrary('tinything', source_files)
env.Alias('tinything', ltinything)
install_lib = env.BWInstallLibrary(ltinything)
env.Alias('install', install_lib)
