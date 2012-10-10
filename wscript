#!/usr/bin/env python
import waflib, os

APPNAME = 'hashtable'
VERSION = '0.1.0'

top = '.'
out = 'build'

# General paths
paths = {
	  'src'     : 'src'    # folder containing source files
	, 'test'    : 'test'   # folder containing tests' source files
	, 'ext'     : 'ext'    # folder containing external (helper) libraries
	, 'sample'  : 'sample' # folder containing sample applications
	, 'include' : 'src'    # folder containing include files
}

# General configuration
config = {
	# TODO
	#'cflags': {
	#	  'gcc': ['-O2', '-Wall']
	#	, 'msvc': ['/O2', '/Wall']
	#}
	#, 'include': []
}


#
subjects = {

	# The application
	APPNAME : {
		'type': 'src'
		, 'sources': ['hashtable.c']
		, 'headers': ['hashtable.h']
	}

	# External libraries
	#, 'csv': {
	#	'type': 'ext'
	#	, 'sources' : ['csv.c']
	#	, 'headers': ['csv.h']
	#}

	# Samples
	, 'stars': {
		'type': 'sample'
		, 'sources' : ['stars.c']
		, 'files': ['stars.csv']
		, 'deps': ['hashtable']
	}
}


def options(ctx):
	helper = Helper(ctx)

	# Waf chore
	ctx.load("compiler_c")

	# Options
	ctx.add_option(
		'-c', '--c-compiler'
		, dest = 'c_compiler'
		, default = ''
		, action = 'store'
		, help = "Chooses the exact compiler to use in the build process."
	)


def configure(ctx):
	helper = Helper(ctx)

	# Load compiler
	if ctx.options.c_compiler:
		ctx.load(ctx.options.c_compiler)
	else:
		ctx.load("compiler_c")

	# Load other tools
	ctx.load("doxygen")


def build(ctx):
	helper = Helper(ctx)

	# Building subjects
	for subjname in helper.subjects:
		subject = helper.subjects[subjname]

		if subject.type == "sample":
			ctx.program(
				  source   = subject.sources()
				, target   = subject.name
				, includes = subject.incpaths()
				, cflags   = subject.cflags()
				, use      = subject.deps()
			)
		else:
			ctx.stlib(
				  source   = subject.sources()
				, target   = subject.name
				, includes = subject.incpaths()
				, cflags   = subject.cflags()
				, use      = subject.deps()
			)
			ctx.shlib(
				  source   = subject.sources()
				, target   = subject.name
				, includes = subject.incpaths()
				, cflags   = subject.cflags()
				, use      = subject.deps()
			)


def test(ctx):
	"""Runs all tests. TODO!"""
	import os

	helper = Helper(ctx)


import waflib
class TestContext(waflib.Build.BuildContext):
	cmd = 'test'
	fun = test


class Helper(object):
	"""A small helper class, various function utilities are put here."""

	class Subject(object):
		"""Easier handling of configuration data."""

		def __init__(self, parent, name, data):
			self.type = data["type"]
			self.name = name
			self.parent = parent
			self.data = data


		def basepath(self):
			path = os.path.join(paths[self.type], self.name)

			if self.type == "src":
				path = paths['src']

			if "basepath" in self.data:
				path = self.basepath

			return path


		def incpath(self):
			path = self.basepath()

			if "incpath" in self.data: 
				path = os.path.join(path, self.data["incpath"])

			return path


		def srcpath(self):
			path = self.basepath()

			if "srcpath" in self.data: 
				path = os.path.join(path, self.data["srcpath"])

			return path


		def sources(self):
			ret = []
			basepath = self.srcpath()

			for source in self.data['sources']:
				ret.append(os.path.join(basepath, source))

			return ret


		def headers(self):
			ret = []
			basepath = self.incpath()

			if "headers" in self.data:
				for source in self.data['headers']:
					ret.append(os.path.join(basepath, source))

			return ret


		def incpaths(self):
			ret = self.parent.incpaths()

			if "include" in self.data:
				for incpath in self.data["include"]:
					if incpath not in ret:
						ret.append(incpath)

			if "deps" in self.data:
				for dep in self.data["deps"]:
					deppath = self.parent.subjects[dep].incpath()
					if deppath not in ret:
						ret.append(deppath)

			return ret


		def cflags(self):
			ret = self.parent.cflags()

			if "cflags" in self.data:
				for cflag in self.data["cflags"]:
					if cflag not in ret:
						ret.append(cflag)

			return ret


		def deps(self):
			ret = []

			if "deps" in self.data:
				ret += self.data["deps"]

			return ret


	def __init__(self, ctx):
		self.ctx = ctx
		self.subjects = {}

		for subjname in subjects:
			self.subjects[subjname] = Helper.Subject(self, subjname, subjects[subjname])

		self.app = self.subjects[APPNAME]

	def incpaths(self):
		ret = []

		if "include" in config:
			for incpath in config["include"]:
				ret.append(incpath)

		return ret


	def cflags(self):
		ret = []

		if "cflags" in config:
			for cflag in config["cflags"]:
				ret.append(cflag)

		return ret

