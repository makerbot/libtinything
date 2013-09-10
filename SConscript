# vim:ai:et:ff=unix:fileencoding=utf-8:sw=4:syntax=python:ts=4:

import os

env = Environment(ENV = os.environ, tools=['default', 'birdwing_install'])

env.BWSetCompilerToGcc()

env.BWDependsOnJsonCpp()
env.BWDependsOnZlib()

source_files =  Glob('src/*.cc') + Glob('src/miniunzip/*.c')

ltinything = env.SharedLibrary('tinything', source_files)
env.Clean(ltinything, '#/obj')
env.BWInstallLibrary(ltinything)

