## Process this file with automake to produce Makefile.in.
##
##  	Copyright (C) 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
##
##   This file is part of GUILE.
##
##   GUILE is free software; you can redistribute it and/or modify
##   it under the terms of the GNU General Public License as
##   published by the Free Software Foundation; either version 2, or
##   (at your option) any later version.
##
##   GUILE is distributed in the hope that it will be useful, but
##   WITHOUT ANY WARRANTY; without even the implied warranty of
##   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##   GNU General Public License for more details.
##
##   You should have received a copy of the GNU General Public
##   License along with GUILE; see the file COPYING.  If not, write
##   to the Free Software Foundation, Inc., 59 Temple Place, Suite
##   330, Boston, MA 02111-1307 USA

AUTOMAKE_OPTIONS = 1.5

SUBDIRS = oop qt libguile-ltdl libguile ice-9 guile-config guile-readline \
	  scripts srfi doc examples test-suite am

bin_SCRIPTS = guile-tools

include_HEADERS = libguile.h

EXTRA_DIST = HACKING GUILE-VERSION ANON-CVS SNAPSHOTS \
	     $(ACLOCAL) BUGS README-PACKAGING

TESTS = check-guile

# clean up check-guile.in symlink...
CLEANFILES = guile-procedures.txt

DISTCLEANFILES = check-guile.log

# The rule will cd to $(top_srcdir).
ACLOCAL = ./guile-aclocal.sh

# Makefile.am ends here
