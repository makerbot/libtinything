import os

env = Environment(
    ENV=os.environ,
    tools=['default', 'mb_install'],
    toolpath=['#/../mw-scons-tools', '#/Install/mw-scons-tools'])

env.MBAddIncludePaths('include')
env.MBAddIncludePaths('src/miniunzip')
env.MBAddIncludePaths('src/zlib')

env.MBAddStandardCompilerFlags()

env.MBDependsOnJsonCpp()

if env.MBIsWindows():
    runtime_paths = []
else:
    runtime_paths = [
        '/usr/lib'
    ]

source_files = [
    Glob('src/*.cc'),
    Glob('src/miniunzip/*.c'),
    Glob('src/zlib/*.c'),
]

tinything = env.MBSharedLibrary("tinything", source_files, RPATH=runtime_paths)

tinything_install = [
    env.MBInstallLib(tinything, 'tinything'),
    env.MBInstallHeaders(env.MBGlob('#/include/tinything/*'), 'tinything'),
]

# Stupid hack to get tinything doing the same thing as the other
# cmake repos until it gets converted to cmake
if env.MBIsMac():
    libtinything = env.Command(
        os.path.join(env['MB_LIB_DIR'], 'libtinything.dylib'),
        'libtinything.dylib',
        'cp $SOURCE $TARGET && '
        'install_name_tool -id @rpath/libtinything.dylib $TARGET'
    )
    env.Append(MB_INSTALL_TARGETS=libtinything)
    tinything_install.append(libtinything)

env.MBCreateInstallTarget()

program_env = env.Clone()
program_env.MBDependsOnTinything()
writer_program = program_env.MBProgram(
    target='makerbot_tinything_writer',
    source=['src/cli/writer.cc'])
program_env.Depends(writer_program, tinything_install)
program_env.MBInstallBin(writer_program)
