/* Copyright (C) 1995,1996,1997,1998,1999,2000,2001 Free Software Foundation, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307 USA
 *
 * As a special exception, the Free Software Foundation gives permission
 * for additional uses of the text contained in its release of GUILE.
 *
 * The exception is that, if you link the GUILE library with other files
 * to produce an executable, this does not by itself cause the
 * resulting executable to be covered by the GNU General Public License.
 * Your use of that executable is in no way restricted on account of
 * linking the GUILE library code into it.
 *
 * This exception does not however invalidate any other reasons why
 * the executable file might be covered by the GNU General Public License.
 *
 * This exception applies only to the code released by the
 * Free Software Foundation under the name GUILE.  If you copy
 * code from other Free Software Foundation releases into a copy of
 * GUILE, as the General Public License permits, the exception does
 * not apply to the code that you add in this way.  To avoid misleading
 * anyone as to the status of such modified files, you must delete
 * this exception notice from them.
 *
 * If you write modifications of your own for GUILE, it is your choice
 * whether to permit this exception to apply to your modifications.
 * If you do not wish that, delete this exception notice.  */




#include <stdio.h>
#include <fcntl.h>
#include "libguile/_scm.h"
#include "libguile/strings.h"
#include "libguile/validate.h"
#include "libguile/gc.h"

#include "libguile/fports.h"

#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#else
size_t fwrite ();
#endif
#ifdef HAVE_STRUCT_STAT_ST_BLKSIZE
#include <sys/stat.h>
#endif

#include <errno.h>

#include "libguile/iselect.h"
/* Some defines for Windows. */
#if defined(__MINGW32__) || defined(__MSVC__)
# include <sys/stat.h>
# include <winsock2.h>
# define ftruncate(fd, size) chsize (fd, size)
#endif /* __MINGW32__ */


scm_t_bits scm_tc16_fport;


/* default buffer size, used if the O/S won't supply a value.  */
static const size_t default_buffer_size = 1024;

/* create FPORT buffer with specified sizes (or -1 to use default size or
   0 for no buffer.  */
static void
scm_fport_buffer_add (SCM port, long read_size, int write_size)
#define FUNC_NAME "scm_fport_buffer_add"
{
  scm_t_fport *fp = SCM_FSTREAM (port);
  scm_t_port *pt = SCM_PTAB_ENTRY (port);

  if (read_size == -1 || write_size == -1)
    {
      size_t default_size;
#ifdef HAVE_STRUCT_STAT_ST_BLKSIZE
      struct stat st;
      
      default_size = (fstat (fp->fdes, &st) == -1) ? default_buffer_size
	: st.st_blksize;
#else
      default_size = default_buffer_size;
#endif
      if (read_size == -1)
	read_size = default_size;
      if (write_size == -1)
	write_size = default_size;
    }

  if (SCM_INPUT_PORT_P (port) && read_size > 0)
    {
      pt->read_buf = scm_must_malloc (read_size, FUNC_NAME);
      pt->read_pos = pt->read_end = pt->read_buf;
      pt->read_buf_size = read_size;
    }
  else
    {
      pt->read_pos = pt->read_buf = pt->read_end = &pt->shortbuf;
      pt->read_buf_size = 1;
    }

  if (SCM_OUTPUT_PORT_P (port) && write_size > 0)
    {
      pt->write_buf = scm_must_malloc (write_size, FUNC_NAME);
      pt->write_pos = pt->write_buf;
      pt->write_buf_size = write_size;
    }
  else
    {
      pt->write_buf = pt->write_pos = &pt->shortbuf;
      pt->write_buf_size = 1;
    }

  pt->write_end = pt->write_buf + pt->write_buf_size;
  if (read_size > 0 || write_size > 0)
    SCM_SET_CELL_WORD_0 (port, SCM_CELL_WORD_0 (port) & ~SCM_BUF0);
  else
    SCM_SET_CELL_WORD_0 (port, SCM_CELL_WORD_0 (port) | SCM_BUF0);
}
#undef FUNC_NAME

SCM_DEFINE (scm_setvbuf, "setvbuf", 2, 1, 0, 
            (SCM port, SCM mode, SCM size),
	    "Set the buffering mode for @var{port}.  @var{mode} can be:\n"
	    "@table @code\n"
	    "@item _IONBF\n"
	    "non-buffered\n"
	    "@item _IOLBF\n"
	    "line buffered\n"
	    "@item _IOFBF\n"
	    "block buffered, using a newly allocated buffer of @var{size} bytes.\n"
	    "If @var{size} is omitted, a default size will be used.\n"
	    "@end table")
#define FUNC_NAME s_scm_setvbuf
{
  int cmode;
  long csize;
  scm_t_port *pt;

  port = SCM_COERCE_OUTPORT (port);

  SCM_VALIDATE_OPFPORT (1,port);
  SCM_VALIDATE_INUM_COPY (2,mode,cmode);
  if (cmode != _IONBF && cmode != _IOFBF && cmode != _IOLBF)
    scm_out_of_range (FUNC_NAME, mode);

  if (cmode == _IOLBF)
    {
      SCM_SET_CELL_WORD_0 (port, SCM_CELL_WORD_0 (port) | SCM_BUFLINE);
      cmode = _IOFBF;
    }
  else
    {
      SCM_SET_CELL_WORD_0 (port, SCM_CELL_WORD_0 (port) ^ SCM_BUFLINE);
    }

  if (SCM_UNBNDP (size))
    {
      if (cmode == _IOFBF)
	csize = -1;
      else
	csize = 0;
    }
  else
    {
      SCM_VALIDATE_INUM_COPY (3,size,csize);
      if (csize < 0 || (cmode == _IONBF && csize > 0))
	scm_out_of_range (FUNC_NAME, size);
    }

  pt = SCM_PTAB_ENTRY (port);

  /* silently discards buffered chars.  */
  if (pt->read_buf != &pt->shortbuf)
    scm_must_free (pt->read_buf);
  if (pt->write_buf != &pt->shortbuf)
    scm_must_free (pt->write_buf);

  scm_fport_buffer_add (port, csize, csize);
  return SCM_UNSPECIFIED;
}
#undef FUNC_NAME

/* Move ports with the specified file descriptor to new descriptors,
 * reseting the revealed count to 0.
 */

void
scm_evict_ports (int fd)
{
  long i;

  for (i = 0; i < scm_port_table_size; i++)
    {
      SCM port = scm_port_table[i]->port;

      if (SCM_FPORTP (port))
	{
	  scm_t_fport *fp = SCM_FSTREAM (port);

	  if (fp->fdes == fd)
	    {
	      fp->fdes = dup (fd);
	      if (fp->fdes == -1)
		scm_syserror ("scm_evict_ports");
	      scm_set_port_revealed_x (port, SCM_MAKINUM (0));
	    }
	}
    }
}


SCM_DEFINE (scm_file_port_p, "file-port?", 1, 0, 0,
	    (SCM obj),
	    "Determine whether @var{obj} is a port that is related to a file.")
#define FUNC_NAME s_scm_file_port_p
{
  return SCM_BOOL (SCM_FPORTP (obj));
}
#undef FUNC_NAME


/* scm_open_file
 * Return a new port open on a given file.
 *
 * The mode string must match the pattern: [rwa+]** which
 * is interpreted in the usual unix way.
 *
 * Return the new port.
 */
SCM_DEFINE (scm_open_file, "open-file", 2, 0, 0,
           (SCM filename, SCM mode),
	    "Open the file whose name is @var{filename}, and return a port\n"
	    "representing that file.  The attributes of the port are\n"
	    "determined by the @var{mode} string.  The way in which this is\n"
	    "interpreted is similar to C stdio.  The first character must be\n"
	    "one of the following:\n"
	    "@table @samp\n"
	    "@item r\n"
	    "Open an existing file for input.\n"
	    "@item w\n"
	    "Open a file for output, creating it if it doesn't already exist\n"
	    "or removing its contents if it does.\n"
	    "@item a\n"
	    "Open a file for output, creating it if it doesn't already\n"
	    "exist.  All writes to the port will go to the end of the file.\n"
	    "The \"append mode\" can be turned off while the port is in use\n"
	    "@pxref{Ports and File Descriptors, fcntl}\n"
	    "@end table\n"
	    "The following additional characters can be appended:\n"
	    "@table @samp\n"
	    "@item +\n"
	    "Open the port for both input and output.  E.g., @code{r+}: open\n"
	    "an existing file for both input and output.\n"
	    "@item 0\n"
	    "Create an \"unbuffered\" port.  In this case input and output\n"
	    "operations are passed directly to the underlying port\n"
	    "implementation without additional buffering.  This is likely to\n"
	    "slow down I/O operations.  The buffering mode can be changed\n"
	    "while a port is in use @pxref{Ports and File Descriptors,\n"
	    "setvbuf}\n"
	    "@item l\n"
	    "Add line-buffering to the port.  The port output buffer will be\n"
	    "automatically flushed whenever a newline character is written.\n"
	    "@end table\n"
	    "In theory we could create read/write ports which were buffered\n"
	    "in one direction only.  However this isn't included in the\n"
	    "current interfaces.  If a file cannot be opened with the access\n"
	    "requested, @code{open-file} throws an exception.")
#define FUNC_NAME s_scm_open_file
{
  SCM port;
  int fdes;
  int flags = 0;
  char *file;
  char *md;
  char *ptr;

  SCM_VALIDATE_STRING (1, filename);
  SCM_VALIDATE_STRING (2, mode);
  SCM_STRING_COERCE_0TERMINATION_X (filename);
  SCM_STRING_COERCE_0TERMINATION_X (mode);

  file = SCM_STRING_CHARS (filename);
  md = SCM_STRING_CHARS (mode);

  switch (*md)
    {
    case 'r':
      flags |= O_RDONLY;
      break;
    case 'w':
      flags |= O_WRONLY | O_CREAT | O_TRUNC;
      break;
    case 'a':
      flags |= O_WRONLY | O_CREAT | O_APPEND;
      break;
    default:
      scm_out_of_range (FUNC_NAME, mode);
    }
  ptr = md + 1;
  while (*ptr != '\0')
    {
      switch (*ptr)
	{
	case '+':
	  flags = (flags & ~(O_RDONLY | O_WRONLY)) | O_RDWR;
	  break;
	case 'b':
#if defined (O_BINARY)
	  flags |= O_BINARY;
#endif
	  break;
	case '0':  /* unbuffered: handled later.  */
	case 'l':  /* line buffered: handled during output.  */
	  break;
	default:
	  scm_out_of_range (FUNC_NAME, mode);
	}
      ptr++;
    }
  SCM_SYSCALL (fdes = open (file, flags, 0666));
  if (fdes == -1)
    {
      int en = errno;

      SCM_SYSERROR_MSG ("~A: ~S",
			scm_cons (scm_makfrom0str (strerror (en)),
				  scm_cons (filename, SCM_EOL)), en);
    }
  port = scm_fdes_to_port (fdes, md, filename);
  return port;
}
#undef FUNC_NAME


#if defined(__MINGW32__) || defined(__MSVC__)
/*
 * Try getting the appropiate file flags for a given file descriptor
 * under Windows. This incorporates some fancy operations because Windows
 * differentiates between file, pipe and socket descriptors.
 */
#ifndef O_ACCMODE
# define O_ACCMODE 0x0003
#endif

static int getflags (int fdes)
{
  int flags = 0;
  struct stat buf;
  int error, optlen = sizeof (int);

  /* Is this a socket ? */
  if (getsockopt (fdes, SOL_SOCKET, SO_ERROR, (void *) &error, &optlen) >= 0)
    flags = O_RDWR;
  /* Maybe a regular file ? */
  else if (fstat (fdes, &buf) < 0)
    flags = -1;
  else
    {
      /* Or an anonymous pipe handle ? */
      if (buf.st_mode & 0x1000 /* _O_SHORT_LIVED */)
	flags = O_RDWR;
      /* stdin ? */
      else if (fdes == 0 && isatty (fdes))
	flags = O_RDONLY;
      /* stdout / stderr ? */
      else if ((fdes == 1 || fdes == 2) && isatty (fdes))
	flags = O_WRONLY;
      else
	flags = buf.st_mode;
    }
  return flags;
}
#endif /* __MINGW32__ */

/* Building Guile ports from a file descriptor.  */

/* Build a Scheme port from an open file descriptor `fdes'.
   MODE indicates whether FILE is open for reading or writing; it uses
      the same notation as open-file's second argument.
   NAME is a string to be used as the port's filename.
*/
SCM
scm_fdes_to_port (int fdes, char *mode, SCM name)
#define FUNC_NAME "scm_fdes_to_port"
{
  long mode_bits = scm_mode_bits (mode);
  SCM port;
  scm_t_port *pt;
  int flags;

  /* test that fdes is valid.  */
#if defined(__MINGW32__) || defined(__MSVC__)
  flags = getflags (fdes);
#else
  flags = fcntl (fdes, F_GETFL, 0);
#endif
  if (flags == -1)
    SCM_SYSERROR;
  flags &= O_ACCMODE;
  if (flags != O_RDWR
      && ((flags != O_WRONLY && (mode_bits & SCM_WRTNG))
	  || (flags != O_RDONLY && (mode_bits & SCM_RDNG))))
    {
      SCM_MISC_ERROR ("requested file mode not available on fdes", SCM_EOL);
    }

  SCM_NEWCELL (port);
  SCM_DEFER_INTS;
  pt = scm_add_to_port_table (port);
  SCM_SETPTAB_ENTRY (port, pt);
  SCM_SET_CELL_TYPE (port, (scm_tc16_fport | mode_bits));

  {
    scm_t_fport *fp
      = (scm_t_fport *) scm_must_malloc (sizeof (scm_t_fport),
					      FUNC_NAME);

    fp->fdes = fdes;
    pt->rw_random = SCM_FDES_RANDOM_P (fdes);
    SCM_SETSTREAM (port, fp);
    if (mode_bits & SCM_BUF0)
      scm_fport_buffer_add (port, 0, 0);
    else
      scm_fport_buffer_add (port, -1, -1);
  }
  SCM_SET_FILENAME (port, name);
  SCM_ALLOW_INTS;
  return port;
}
#undef FUNC_NAME

/* Return a lower bound on the number of bytes available for input.  */
static int
fport_input_waiting (SCM port)
{
  int fdes = SCM_FSTREAM (port)->fdes;

#ifdef HAVE_SELECT
  struct timeval timeout;
  SELECT_TYPE read_set;
  SELECT_TYPE write_set;
  SELECT_TYPE except_set;

  FD_ZERO (&read_set);
  FD_ZERO (&write_set);
  FD_ZERO (&except_set);

  FD_SET (fdes, &read_set);
  
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;

  if (select (SELECT_SET_SIZE,
	      &read_set, &write_set, &except_set, &timeout)
      < 0)
    scm_syserror ("fport_input_waiting");
  return FD_ISSET (fdes, &read_set) ? 1 : 0;
#elif defined (FIONREAD)
  int remir;
  // undefined reference to `ioctl'
  ioctl(fdes, FIONREAD, &remir);
  return remir;
#else    
  scm_misc_error ("fport_input_waiting",
		  "Not fully implemented on this platform",
		  SCM_EOL);
#endif
}


static int 
fport_print (SCM exp, SCM port, scm_print_state *pstate SCM_UNUSED)
{
  scm_puts ("#<", port);
  scm_print_port_mode (exp, port);    
  if (SCM_OPFPORTP (exp))
    {
      int fdes;
      SCM name = SCM_FILENAME (exp);
      if (SCM_STRINGP (name) || SCM_SYMBOLP (name))
	scm_display (name, port);
      else
	scm_puts (SCM_PTOBNAME (SCM_PTOBNUM (exp)), port);
      scm_putc (' ', port);
      fdes = (SCM_FSTREAM (exp))->fdes;
      
#ifdef HAVE_TTYNAME
      if (isatty (fdes))
	scm_puts (ttyname (fdes), port);
      else
#endif /* HAVE_TTYNAME */
	scm_intprint (fdes, 10, port);
    }
  else
    {
      scm_puts (SCM_PTOBNAME (SCM_PTOBNUM (exp)), port);
      scm_putc (' ', port);
      scm_intprint (SCM_UNPACK (SCM_CDR (exp)), 16, port);
    }
  scm_putc ('>', port);
  return 1;
}

#ifdef GUILE_ISELECT
/* thread-local block for input on fport's fdes.  */
static void
fport_wait_for_input (SCM port)
{
  int fdes = SCM_FSTREAM (port)->fdes;

  if (!fport_input_waiting (port))
    {
      int n;
      SELECT_TYPE readfds;
      int flags = fcntl (fdes, F_GETFL);

      if (flags == -1)
	scm_syserror ("scm_fdes_wait_for_input");
      if (!(flags & O_NONBLOCK))
	do
	  {
	    FD_ZERO (&readfds);
	    FD_SET (fdes, &readfds);
	    n = scm_internal_select (fdes + 1, &readfds, NULL, NULL, NULL);
	  }
	while (n == -1 && errno == EINTR);
    }
}
#endif

static void fport_flush (SCM port);

/* fill a port's read-buffer with a single read.  returns the first
   char or EOF if end of file.  */
static int
fport_fill_input (SCM port)
{
  long count;
  scm_t_port *pt = SCM_PTAB_ENTRY (port);
  scm_t_fport *fp = SCM_FSTREAM (port);

#ifdef GUILE_ISELECT
  fport_wait_for_input (port);
#endif
  SCM_SYSCALL (count = read (fp->fdes, pt->read_buf, pt->read_buf_size));
  if (count == -1)
    scm_syserror ("fport_fill_input");
  if (count == 0)
    return EOF;
  else
    {
      pt->read_pos = pt->read_buf;
      pt->read_end = pt->read_buf + count;
      return *pt->read_buf;
    }
}

static off_t
fport_seek (SCM port, off_t offset, int whence)
{
  scm_t_port *pt = SCM_PTAB_ENTRY (port);
  scm_t_fport *fp = SCM_FSTREAM (port);
  off_t rv;
  off_t result;

  if (pt->rw_active == SCM_PORT_WRITE)
    {
      if (offset != 0 || whence != SEEK_CUR)
	{
	  fport_flush (port);
	  result = rv = lseek (fp->fdes, offset, whence);
	}
      else
	{
	  /* read current position without disturbing the buffer.  */
	  rv = lseek (fp->fdes, offset, whence);
	  result = rv + (pt->write_pos - pt->write_buf);
	}
    }
  else if (pt->rw_active == SCM_PORT_READ)
    {
      if (offset != 0 || whence != SEEK_CUR)
	{
	  /* could expand to avoid a second seek.  */
	  scm_end_input (port);
	  result = rv = lseek (fp->fdes, offset, whence);
	}
      else
	{
	  /* read current position without disturbing the buffer
	     (particularly the unread-char buffer).  */
	  rv = lseek (fp->fdes, offset, whence);
	  result = rv - (pt->read_end - pt->read_pos);

	  if (pt->read_buf == pt->putback_buf)
	    result -= pt->saved_read_end - pt->saved_read_pos;
	}
    }
  else /* SCM_PORT_NEITHER */
    {
      result = rv = lseek (fp->fdes, offset, whence);      
    }

  if (rv == -1)
    scm_syserror ("fport_seek");

  return result;
}

static void
fport_truncate (SCM port, off_t length)
{
  scm_t_fport *fp = SCM_FSTREAM (port);

  if (ftruncate (fp->fdes, length) == -1)
    scm_syserror ("ftruncate");
}

/* helper for fport_write: try to write data, using multiple system
   calls if required.  */
#define FUNC_NAME "write_all"
static void write_all (SCM port, const void *data, size_t remaining)
{
  int fdes = SCM_FSTREAM (port)->fdes;

  while (remaining > 0)
    {
      size_t done;

      SCM_SYSCALL (done = write (fdes, data, remaining));

      if (done == -1)
	SCM_SYSERROR;
      remaining -= done;
      data = ((const char *) data) + done;
    }
}
#undef FUNC_NAME

static void
fport_write (SCM port, const void *data, size_t size)
{
  /* this procedure tries to minimize the number of writes/flushes.  */
  scm_t_port *pt = SCM_PTAB_ENTRY (port);

  if (pt->write_buf == &pt->shortbuf
      || (pt->write_pos == pt->write_buf && size >= pt->write_buf_size))
    {
      /* "unbuffered" port, or
	 port with empty buffer and data won't fit in buffer. */
      write_all (port, data, size);
      return;
    }

  {
    off_t space = pt->write_end - pt->write_pos;

    if (size <= space)
      {
	/* data fits in buffer.  */
	memcpy (pt->write_pos, data, size);
	pt->write_pos += size;
	if (pt->write_pos == pt->write_end)
	  {
	    fport_flush (port);
	    /* we can skip the line-buffering check if nothing's buffered. */
	    return;
	  }
      }
    else
      {
	memcpy (pt->write_pos, data, space);
	pt->write_pos = pt->write_end;
	fport_flush (port);
	{
	  const void *ptr = ((const char *) data) + space;
	  size_t remaining = size - space;

	  if (size >= pt->write_buf_size)
	    {
	      write_all (port, ptr, remaining);
	      return;
	    }
	  else
	    {
	      memcpy (pt->write_pos, ptr, remaining);
	      pt->write_pos += remaining;
	    }
	}
      }

    /* handle line buffering.  */     
    if ((SCM_CELL_WORD_0 (port) & SCM_BUFLINE) && memchr (data, '\n', size))
      fport_flush (port);
  }
}

/* becomes 1 when process is exiting: normal exception handling won't
   work by this time.  */
extern int scm_i_terminating; 

static void
fport_flush (SCM port)
{
  scm_t_port *pt = SCM_PTAB_ENTRY (port);
  scm_t_fport *fp = SCM_FSTREAM (port);
  unsigned char *ptr = pt->write_buf;
  long init_size = pt->write_pos - pt->write_buf;
  long remaining = init_size;

  while (remaining > 0)
    {
      long count;

      SCM_SYSCALL (count = write (fp->fdes, ptr, remaining));
      if (count < 0)
	{
	  /* error.  assume nothing was written this call, but
	     fix up the buffer for any previous successful writes.  */
	  long done = init_size - remaining;
	      
	  if (done > 0)
	    {
	      int i;

	      for (i = 0; i < remaining; i++)
		{
		  *(pt->write_buf + i) = *(pt->write_buf + done + i);
		}
	      pt->write_pos = pt->write_buf + remaining;
	    }
	  if (scm_i_terminating)
	    {
	      const char *msg = "Error: could not flush file-descriptor ";
	      char buf[11];

	      write (2, msg, strlen (msg));
	      sprintf (buf, "%d\n", fp->fdes);
	      write (2, buf, strlen (buf));

	      count = remaining;
	    }
	  else if (scm_gc_running_p)
	    {
	      /* silently ignore the error.  scm_error would abort if we
		 called it now.  */
	      count = remaining;
	    }
	  else
	    scm_syserror ("fport_flush");
	}
      ptr += count;
      remaining -= count;
    }
  pt->write_pos = pt->write_buf;
  pt->rw_active = SCM_PORT_NEITHER;
}

/* clear the read buffer and adjust the file position for unread bytes. */
static void
fport_end_input (SCM port, int offset)
{
  scm_t_fport *fp = SCM_FSTREAM (port);
  scm_t_port *pt = SCM_PTAB_ENTRY (port);
  
  offset += pt->read_end - pt->read_pos;

  if (offset > 0)
    {
      pt->read_pos = pt->read_end;
      /* will throw error if unread-char used at beginning of file
	 then attempting to write.  seems correct.  */
      if (lseek (fp->fdes, -offset, SEEK_CUR) == -1)
	scm_syserror ("fport_end_input");
    }
  pt->rw_active = SCM_PORT_NEITHER;
}

static int
fport_close (SCM port)
{
  scm_t_fport *fp = SCM_FSTREAM (port);
  scm_t_port *pt = SCM_PTAB_ENTRY (port);
  int rv;

  fport_flush (port);
  SCM_SYSCALL (rv = close (fp->fdes));
  if (rv == -1 && errno != EBADF)
    {
      if (scm_gc_running_p)
	/* silently ignore the error.  scm_error would abort if we
	   called it now.  */
	;
      else
	scm_syserror ("fport_close");
    }
  if (pt->read_buf == pt->putback_buf)
    pt->read_buf = pt->saved_read_buf;
  if (pt->read_buf != &pt->shortbuf)
    scm_must_free (pt->read_buf);
  if (pt->write_buf != &pt->shortbuf)
    scm_must_free (pt->write_buf);
  scm_must_free ((char *) fp);
  return rv;
}

static size_t
fport_free (SCM port)
{
  fport_close (port);
  return 0;
}

static scm_t_bits
scm_make_fptob ()
{
  scm_t_bits tc = scm_make_port_type ("file", fport_fill_input, fport_write);

  scm_set_port_free            (tc, fport_free);
  scm_set_port_print           (tc, fport_print);
  scm_set_port_flush           (tc, fport_flush);
  scm_set_port_end_input       (tc, fport_end_input);
  scm_set_port_close           (tc, fport_close);
  scm_set_port_seek            (tc, fport_seek);
  scm_set_port_truncate        (tc, fport_truncate);
  scm_set_port_input_waiting   (tc, fport_input_waiting);

  return tc;
}

void
scm_init_fports ()
{
  scm_tc16_fport = scm_make_fptob ();

  scm_c_define ("_IOFBF", SCM_MAKINUM (_IOFBF));
  scm_c_define ("_IOLBF", SCM_MAKINUM (_IOLBF));
  scm_c_define ("_IONBF", SCM_MAKINUM (_IONBF));

#include "libguile/fports.x"
}

/*
  Local Variables:
  c-file-style: "gnu"
  End:
*/
