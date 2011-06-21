import Options
from os import unlink, symlink, popen, environ
from os.path import exists 

srcdir = "."
blddir = "build"
VERSION = "0.0.1"

def set_options(opt):
  opt.tool_options("compiler_cxx")

def configure(conf):
  conf.check_tool("compiler_cxx")
  conf.check_tool("node_addon")

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
  obj.cxxflags = ["-O0", "-I"+environ['OPENCL_PATH']+"/include",
                  "-D_FILE_OFFSET_BITS=64", "-D_LARGEFILE_SOURCE"]
  obj.ldflags = ["-lclwrapper", "-L./"]

def shutdown():
  if Options.commands['clean']:
    if exists('_webcl.node'): unlink('_webcl.node')
  else:
    if exists('build/default/_webcl.node') and not exists('_webcl.node'):
      symlink('build/default/_webcl.node', '_webcl.node')
