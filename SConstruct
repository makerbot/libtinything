import os
env = Environment(
    ENV=os.environ, tools=['default', 'birdwing_install', 'birdwing_build'],
    toolpath=['#/../bw-scons-tools'],
)
SConscript('SConscript',
           variant_dir='.build-'+env.BWGetVariantDirSuffix(),
           duplicate=1)

env.Default('install')
