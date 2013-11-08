import os

env = Environment(
  ENV = os.environ,
  tools=['default', 'mb_install'])

env.MBAddIncludePaths('include')
env.MBAddIncludePaths('include/tinything')
env.MBAddIncludePaths('src')
env.MBAddIncludePaths('src/miniunzip')
env.MBAddIncludePaths('src/zlib')

env.MBAddStandardCompilerFlags()
# env.Append(CPPDEFINES = ["__DEBUG"])

env.MBDependsOnJsonCpp()

if env.MBIsWindows():
  runtime_paths = []
else:
  runtime_paths = [
      '/usr/lib'
  ]

source_files =  Glob('src/*.cc') + Glob('src/miniunzip/*.c') + Glob('src/zlib/*.c')

print [str(file) for file in source_files]

tinything = env.MBSharedLibrary("tinything", source_files, RPATH=runtime_paths)

env.MBInstallLib(tinything, 'tinything')
