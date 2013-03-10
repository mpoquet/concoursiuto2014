#!/usr/bin/env python

from distutils.core import setup, Extension

module_name = 'contest'

module = Extension(
    '_'+module_name,
    sources=[module_name+'_wrap.cxx', module_name+'.cpp'],
    include_dirs=['../../libnetwork/include'],
    library_dirs=['../../libnetwork/lib'],
    libraries=['libnetwork'],
    extra_compile_args=['-Wno-unused-but-set-variable'],
)

setup(
    name = module_name,
    version = '1.0',
    author      = 'Zephyr',
    description = 'Contest API',
    ext_modules = [module],
    py_modules = [module_name],
)

