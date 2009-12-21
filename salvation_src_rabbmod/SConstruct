#!SCons
import sys;

# Command line options
opts = Variables()
opts.AddVariables(
	BoolVariable('DEBUG', 'Debug build', False),
	BoolVariable('OFFSET', 'Offset finding', False),
	BoolVariable('QUIET', 'Quiet Output', True),
	BoolVariable('PRIV', 'For testing...', False)
)
 
# Main build environment
env = Environment(options=opts, toolpath=['./scons'], tools=['default', 'DistZip'])

# Generate help text
Help(opts.GenerateHelpText(env))
 
# Add default compiler flags
env.Append(
	CPPFLAGS=['-m32', '-fPIC', '-march=i686', '-fno-strict-aliasing', '-fvisibility=hidden'],
	LINKFLAGS=['-m32', '-shared'],
	LIBS=['m', 'pthread'],
)

env.MergeFlags('!pkg-config --cflags libelf')
 
# Release/Debug compiler flags
if env['DEBUG']:
	env.Append(
		CPPFLAGS=['-ggdb3'],
		CPPDEFINES={'ETH32_DEBUG': '1'},
		LIBS=['elf', 'bfd', 'opcodes'],
	)

elif env['OFFSET']:
        env.Append(
	        CPPFLAGS=['-ggdb3', '-DETH32_DEBUG', '-DETH32_PRELIM_OFFSETS'],
	        CPPDEFINES={'ETH32_DEBUG': '1'},
	        LIBS=['elf', 'bfd', 'opcodes'],
	)

else:
	env.Append(
		CPPFLAGS=['-O3', '-fomit-frame-pointer', '-ffast-math'],
		LIBS=['dl', 'elf', 'bfd', 'opcodes'],
		LINKFLAGS=['-s'],
	)

if env['PRIV']:
	env.Append( CPPFLAGS=['-DETH32_PRIV'] )

 
# Set build directory
if env['DEBUG']:
	buildDir = 'build/debug'
elif env ['OFFSET']:
	buildDir = 'build/offset'
else:
	buildDir = 'build/release'

if env ['QUIET']:
	env.Append(CPPFLAGS=['-w'])

def print_cmd_line(s, target, src, env):
	if 'pk3' in COMMAND_LINE_TARGETS:
		sys.stdout.write("%s\n"%s);	
	elif env ['QUIET']:
		sys.stdout.write(" Compiling [\033[1;32m%s\033[0m]\n"%(' and '.join([str(x) for x in src])))
        	#open(env['CMD_LOGFILE'], 'a').write("%s\n"%s);
	else:
        	sys.stdout.write("%s\n"%s);

env['PRINT_CMD_LINE_FUNC'] = print_cmd_line
#env['CMD_LOGFILE'] = 'build-log.txt'

SConscript(
	dirs='src',
	build_dir=buildDir,
	duplicate=False,
	exports='env'
)

if 'pk3' in COMMAND_LINE_TARGETS:
        pk3 = env.DistZip("eth32nix.pk3", [env.Dir('pk3')])
        env.Alias("pk3", pk3)

env.Clean(DEFAULT_TARGETS, 'build')
