# vim:ai:et:ff=unix:fileencoding=utf-8:sw=4:syntax=python:ts=4:

import os

env = Environment(ENV = os.environ, tools=['default', 'birdwing_install'], toolpath=[Dir('../../../')])

env.BWDependsOnJsonCpp()
env.BWDependsOnZlib()

env.Append(CPPPATH = ['include/jsoncpp'])
source_files =  Glob('src/*.cc') + Glob('src/miniunzip/*.c')

env.Clean(libjson, '#/obj')

ltinything = env.SharedLibrary('lib/tinything', source_files)
env.BWInstalLibarary(ltinything)

