AddOption('--embedded', action='store_true', dest='embedded')
AddOption('--nfs', dest='rootfs',  type='string', nargs=1, action='store')

if GetOption('embedded'):
    SConscript('SConscript_birdwing', duplicate=1)
else:
    env = Environment(tools = ['mb_sconstruct'])
    env.MBSConscript('SConcript')
