/* classes: h_files */

#ifndef SCM_ERROR_H
#define SCM_ERROR_H
/* Copyright (C) 1995,1996,1997,1998,2000,2001 Free Software Foundation, Inc.
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

#include "libguile/__scm.h"


extern int scm_ints_disabled;

extern SCM scm_system_error_key;
extern SCM scm_num_overflow_key;
extern SCM scm_out_of_range_key;
extern SCM scm_args_number_key;
extern SCM scm_arg_type_key;
extern SCM scm_memory_alloc_key;
extern SCM scm_misc_error_key;



extern void scm_error (SCM key, const char *subr, const char *message,
                             SCM args, SCM rest) SCM_NORETURN;
extern SCM scm_error_scm (SCM key, SCM subr, SCM message,
                          SCM args, SCM rest) SCM_NORETURN;
extern SCM scm_strerror (SCM err);
extern void scm_syserror (const char *subr) SCM_NORETURN;
extern void scm_syserror_msg (const char *subr, const char *message,
                              SCM args, int eno) SCM_NORETURN;
extern void scm_num_overflow (const char *subr) SCM_NORETURN;
extern void scm_out_of_range (const char *subr, SCM bad_value)
     SCM_NORETURN;
extern void scm_out_of_range_pos (const char *subr, SCM bad_value, SCM pos)
     SCM_NORETURN;
extern void scm_wrong_num_args (SCM proc) SCM_NORETURN;
extern void scm_error_num_args_subr (const char* subr) SCM_NORETURN;
extern void scm_wrong_type_arg (const char *subr, int pos,
                                SCM bad_value) SCM_NORETURN;
extern void scm_wrong_type_arg_msg (const char *subr, int pos,
                                    SCM bad_value, const char *sz) SCM_NORETURN;
extern void scm_memory_error (const char *subr) SCM_NORETURN;
extern void scm_misc_error (const char *subr, const char *message,
                            SCM args) SCM_NORETURN;
extern void scm_init_error (void);



#if (SCM_DEBUG_DEPRECATED == 0)

extern SCM scm_wta (SCM arg, const char *pos, const char *s_subr);

#endif  /* SCM_DEBUG_DEPRECATED == 0 */

#endif  /* SCM_ERROR_H */

/*
  Local Variables:
  c-file-style: "gnu"
  End:
*/
