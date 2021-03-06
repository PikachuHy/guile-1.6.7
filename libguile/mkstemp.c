/* Copyright (C) 1991, 1992, 1996, 1998, 2001 Free Software Foundation, Inc.
   This file is derived from mkstemps.c from the GNU Libiberty Library
   which in turn is derived from the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   As a special exception, the Free Software Foundation gives permission
   for additional uses of the text contained in its release of GUILE.

   The exception is that, if you link the GUILE library with other files
   to produce an executable, this does not by itself cause the
   resulting executable to be covered by the GNU General Public License.
   Your use of that executable is in no way restricted on account of
   linking the GUILE library code into it.

   This exception does not however invalidate any other reasons why
   the executable file might be covered by the GNU General Public License.

   This exception applies only to the code released by the
   Free Software Foundation under the name GUILE.  If you copy
   code from other Free Software Foundation releases into a copy of
   GUILE, as the General Public License permits, the exception does
   not apply to the code that you add in this way.  To avoid misleading
   anyone as to the status of such modified files, you must delete
   this exception notice from them.

   If you write modifications of your own for GUILE, it is your choice
   whether to permit this exception to apply to your modifications.
   If you do not wish that, delete this exception notice.  */

#include "libguile/scmconfig.h"

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

/* We need to provide a type for gcc_uint64_t.  */
#ifdef __GNUC__
typedef unsigned long long gcc_uint64_t;
#else
typedef unsigned long gcc_uint64_t;
#endif

#ifndef TMP_MAX
#define TMP_MAX 16384
#endif

/* Generate a unique temporary file name from TEMPLATE.

   TEMPLATE has the form:

   <path>/ccXXXXXX

   The last six characters of TEMPLATE must be "XXXXXX"; they are
   replaced with a string that makes the filename unique.

   Returns a file descriptor open on the file for reading and writing.  */
int
mkstemp (template)
     char *template;
{
  static const char letters[]
    = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  static gcc_uint64_t value;
#ifdef HAVE_GETTIMEOFDAY
  struct timeval tv;
#endif
  char *XXXXXX;
  size_t len;
  int count;

  len = strlen (template);

  if ((int) len < 6
      || strncmp (&template[len - 6], "XXXXXX", 6))
    {
      return -1;
    }

  XXXXXX = &template[len - 6];

#ifdef HAVE_GETTIMEOFDAY
  /* Get some more or less random data.  */
  gettimeofday (&tv, NULL);
  value += ((gcc_uint64_t) tv.tv_usec << 16) ^ tv.tv_sec ^ getpid ();
#else
  value += getpid ();
#endif

  for (count = 0; count < TMP_MAX; ++count)
    {
      gcc_uint64_t v = value;
      int fd;

      /* Fill in the random bits.  */
      XXXXXX[0] = letters[v % 62];
      v /= 62;
      XXXXXX[1] = letters[v % 62];
      v /= 62;
      XXXXXX[2] = letters[v % 62];
      v /= 62;
      XXXXXX[3] = letters[v % 62];
      v /= 62;
      XXXXXX[4] = letters[v % 62];
      v /= 62;
      XXXXXX[5] = letters[v % 62];

      fd = open (template, O_RDWR|O_CREAT|O_EXCL, 0600);
      if (fd >= 0)
	/* The file does not exist.  */
	return fd;

      /* This is a random value.  It is only necessary that the next
	 TMP_MAX values generated by adding 7777 to VALUE are different
	 with (module 2^32).  */
      value += 7777;
    }

  /* We return the null string if we can't find a unique file name.  */
  template[0] = '\0';
  return -1;
}
