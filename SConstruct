#! /usr/bin/env python

version = '0.2.6';

Help("""
Type: 'scons' to build QComicBook with defaults
      'scons configure prefix=/usr/local' to set installation prefix
      'scons install' to install QComicBook
      'DESTDIR=/tmp/qcomicbook scons install' for packaging
""")

## Load the builders in config
env = Environment( tools=['default', 'generic', 'kde'], toolpath=['./', './admin'], CPPDEFINES={'VERSION':'\\"' + version + '\\"'})
env.AppendUnique( CPPFLAGS = ['-DQT_THREAD_SUPPORT', '-D_REENTRANT'] )
debug = ARGUMENTS.get('debug', 0)
if int(debug):
	env.Append(CPPFLAGS=['-g2'])

env.KDEuse("environ rpath")
env.SConscript(dirs=['src','icons'])

#env.docfolder('doc/en/', 'en', 'KleanSweep')

env.KDElang('po/', 'en')
env.dist('qcomicbook', version)
