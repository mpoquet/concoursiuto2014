""" Installer

This module defines a minimal installer for scons build scripts.  It is aimed
at *nix like systems, but I guess it could easily be adapted to other ones as
well.
"""

import fnmatch, os, os.path
import SCons.Defaults

PREFIX = "prefix"
EPREFIX = "eprefix"
BINDIR = "bindir"
LIBDIR = "libdir"
INCLUDEDIR = "includedir"
SHAREDIR = "sharedir"

def AddOptions(opts):
        """ Adds the installer options to the opts.  """
        opts.Add(PREFIX, "Directory of architecture independant files.", "/usr")
        opts.Add(EPREFIX, "Directory of architecture dependant files.", "${%s}" % PREFIX)
        opts.Add(BINDIR, "Directory of executables.", "${%s}/bin" % EPREFIX)
        opts.Add(LIBDIR, "Directory of libraries.", "${%s}/lib" % EPREFIX)
        opts.Add(INCLUDEDIR, "Directory of header files.", "${%s}/include" % PREFIX)
        opts.Add(SHAREDIR, "Directory of data files.", "${%s}/share" % EPREFIX)


class Installer:
    """ A basic installer. """
    def __init__(self, env):
        """ Initialize the installer.

        @param configuration A dictionary containing the configuration.
        @param env The installation environment.
        """
        self._prefix = os.path.join(env.Dir('#').abspath, env.get(PREFIX, "/usr"))
        env['prefix'] = self._prefix

        self._eprefix = env.Dir(env.get(EPREFIX, self._prefix)).srcnode().abspath
        self._bindir = env.Dir(env.get(BINDIR, os.path.join(self._eprefix, "bin"))).srcnode().abspath
        self._libdir = env.Dir(env.get(LIBDIR, os.path.join(self._eprefix, "lib"))).srcnode().abspath
        self._includedir = env.Dir(env.get(INCLUDEDIR, os.path.join(self._prefix, "include"))).srcnode().abspath
        self._sharedir = env.Dir(env.get(SHAREDIR, os.path.join(self._prefix, "share"))).srcnode().abspath
        self._env = env

    def eprefix(self):
        return self._eprefix

    def bindir(self):
        return self._bindir

    def libdir(self):
        return self._libdir

    def includedir(self):
        return self._includedir

    def sharedir(self):
        return self._sharedir

    def Add(self, destdir, name, basedir="", perm=0644):
        destination = os.path.join(destdir, basedir)
        obj = self._env.Install(destination, name)
        self._env.Alias('install', destination)
        
        if basedir != '':
            self._env.Clean('.', destination)

        for i in obj:
            self._env.AddPostAction(i, SCons.Defaults.Chmod(i, perm))

    def AddProgram(self, program):
        """ Install a program.

        @param program The program to install.
        """
        self.Add(self._bindir, program, perm=0755)

    def AddLibrary(self, library, basedir=""):
        """ Install a library.

        @param library the library to install.
        """
        self.Add(self._libdir, library, basedir)

    def AddHeader(self, header, basedir=""):
        self.Add(self._includedir, header, basedir)

    def AddDataFile(self, datafile, basedir=""):
        self.Add(self._sharedir, datafile, basedir)

    def AddHeaders(self, parent, pattern, basedir="", recursive=False):
        """ Installs a set of headers.

        @param parent The parent directory of the headers.
        @param pattern A pattern to identify the files that are headers.
        @param basedir The subdirectory in which to install the headers.
        @param recursive Search recursively for headers.
        """
        for entry in os.listdir(parent):
            entrypath = os.path.join(parent, entry)
            if os.path.isfile(entrypath) and fnmatch.fnmatch(entry, pattern):
                self.AddHeader(entrypath, basedir)
            elif os.path.isdir(entrypath) and recursive:
                self.AddHeaders(entrypath, pattern, os.path.join(basedir, entry), recursive)


