import os

env = Environment(
  ENV = os.environ,
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

source_files =  Glob('src/*.cc') + Glob('src/miniunzip/*.c') + Glob('src/zlib/*.c')

tinything = env.MBSharedLibrary("tinything", source_files, RPATH=runtime_paths)

env.MBInstallLib(tinything, 'tinything')
env.MBInstallHeaders(env.MBGlob('#/include/tinything/*'), 'tinything')

env.MBCreateInstallTarget()

program_env = env.Clone()
program_env.MBDependsOnTinything()
writer_program = program_env.MBProgram(
  target='makerbot_tinything_writer',
  source=['src/cli/writer.cc'])
program_env.Depends(writer_program, tinything)
program_env.MBInstallBin(writer_program)
