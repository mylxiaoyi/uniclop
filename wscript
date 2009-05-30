VERSION='0.1'
APPNAME='uniclop'
srcdir = '.'
blddir = 'output'

def set_options(opt):
    print '  setting the options'
    opt.tool_options('compiler_cxx')

def configure(conf):
    print '  executing the configuration'
    conf.check_tool('compiler_cxx')

    conf.check_tool('boost')
    conf.check_boost(lib=["filesystem", "program_options"])


def build(bld):
    print '  building the project'
    bld.add_subdirs(["src/algorithms", "src/applications"])

