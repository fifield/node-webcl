import Options
from os import symlink, environ, chdir, mkdir, system
from os.path import exists 

srcdir = "."
blddir = "build"
VERSION = "0.0.1"

def set_options(opt):
  opt.tool_options("compiler_cxx")
  opt.add_option( '--opencl-lib'
                  , action='store'
                  , type='string'
                  , default=False
                  , help='Location of OpenCL library'
                  )
  opt.add_option( '--opencl-inc'
                  , action='store'
                  , type='string'
                  , default=False
                  , help='Location of OpenCL include files'
                  )

def configure(conf):
  conf.check_tool("compiler_cxx")
  conf.check_tool("node_addon")

  o = Options.options
  if o.opencl_lib:
    conf.env['OPENCL_LIB_PATH'] = o.opencl_lib
  elif 'OPENCL_LIB_PATH' in environ:
    conf.env['OPENCL_LIB_PATH'] = environ['OPENCL_LIB_PATH']
  else:
    conf.env['OPENCL_LIB_PATH'] = '';

  conf.check(conf.env, lib='OpenCL', libpath=conf.env['OPENCL_LIB_PATH'], 
             uselib_store='OPENCL', mandatory=True)
  
  if o.opencl_inc:
    conf.env['OPENCL_INC_PATH'] = o.opencl_inc
  elif 'OPENCL_INC_PATH' in environ:
    conf.env['OPENCL_INC_PATH'] = environ['OPENCL_INC_PATH']
  else:
    conf.env['OPENCL_INC_PATH'] = '';

def wrapper_cmd(bld):
  cmd = "make -C ../src/wrapper/src all"
  return cmd

def build_wrapper(bld):
  wrapper = bld.new_task_gen(
    name = 'wrapper',
    source = bld.path.ant_glob('src/wrapper/src/*')+' '+bld.path.ant_glob('src/wrapper/include/*'),
    before = "cxx",
    rule = wrapper_cmd(bld)
    )
  wrapper.env.env = dict(environ)

  if ('CXXFLAGS' not in wrapper.env.env): wrapper.env.env['CXXFLAGS'] = ""

  if ('OPENCL_INC_PATH' in bld.env):
    if ('INCLUDES' in wrapper.env.env):
      wrapper.env.env['INCLUDES'] = wrapper.env.env["INCLUDES"] + " -I" + bld.env['OPENCL_INC_PATH']
    else:
      wrapper.env.env['INCLUDES'] = "-I" + bld.env['OPENCL_INC_PATH']

  if ('OPENCL_LIB_PATH' in bld.env):
    if ('LDFLAGS' in wrapper.env.env):
      wrapper.env.env["LDFLAGS"] = wrapper.env.env["LDFLAGS"] + " -L" + bld.env['OPENCL_LIB_PATH']
    else:
      wrapper.env.env["LDFLAGS"] = "-L" + bld.env['OPENCL_LIB_PATH']

  wrapper.env.env['CXXFLAGS'] = wrapper.env.env['CXXFLAGS'] + " -fPIC"

def build(bld):
  obj = bld.new_task_gen("cxx", "shlib", "node_addon")
  obj.target = "_webcl"
  obj.source = "src/webcl.cpp "
  obj.source += "src/platform.cpp "
  obj.source += "src/device.cpp "
  obj.source += "src/context.cpp "
  obj.source += "src/memoryobject.cpp "
  obj.source += "src/programobject.cpp "
  obj.source += "src/kernelobject.cpp "
  obj.source += "src/commandqueue.cpp "
  obj.source += "src/event.cpp "
  obj.source += "src/sampler.cpp "

  obj.lib = "clwrapper"
  obj.libpath = "./"

  if ('OPENCL_INC_PATH' in bld.env): obj.cxxflags = "-I" + bld.env['OPENCL_INC_PATH']
  obj.cxxflags = [obj.cxxflags, '-std=gnu++0x']

  build_wrapper(bld)

def shutdown():
  if Options.commands['clean']:
    cmd = "make -C ./src/wrapper clean; rm -rf ./build; rm -rf node_modules"
    system(cmd);
  else:
    if not exists('node_modules'): mkdir('node_modules')
    if exists('build/Release/_webcl.node') and not exists('node_modules/_webcl.node'):
      chdir('node_modules')
      symlink('../build/Release/_webcl.node', '_webcl.node')
      chdir('..')
    if exists('webcl.js') and not exists('node_modules/webcl.js'):
      chdir('node_modules')
      symlink('../webcl.js', 'webcl.js')
      chdir('..')
