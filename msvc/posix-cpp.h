//
// Created by pikachu on 8/13/2020.
//

#ifndef GUILE_POSIX_CPP_H
#define GUILE_POSIX_CPP_H
#ifdef __cplusplus
extern "C" {
#endif
#include "libguile/__scm.h"

extern SCM scm_tcsetpgrp (SCM port, SCM pgid);
extern SCM scm_tcgetpgrp (SCM port);
extern SCM scm_ctermid (void);
extern SCM scm_setsid (void);
extern SCM scm_setpgid (SCM pid, SCM pgid);
extern SCM scm_pipe (void);
extern SCM scm_getgroups (void);
extern SCM scm_getpgrp (void);
extern SCM scm_getpwuid (SCM user);
extern SCM scm_setpwent (SCM arg);
extern SCM scm_getgrgid (SCM name);
extern SCM scm_setgrent (SCM arg);
extern SCM scm_kill (SCM pid, SCM sig);
extern SCM scm_waitpid (SCM pid, SCM options);
extern SCM scm_status_exit_val (SCM status);
extern SCM scm_status_term_sig (SCM status);
extern SCM scm_status_stop_sig (SCM status);
extern SCM scm_getppid (void);
extern SCM scm_getuid (void);
extern SCM scm_getgid (void);
extern SCM scm_geteuid (void);
extern SCM scm_getegid (void);
extern SCM scm_setuid (SCM id);
extern SCM scm_setgid (SCM id);
extern SCM scm_seteuid (SCM id);
extern SCM scm_setegid (SCM id);
extern SCM scm_ttyname (SCM port);
extern SCM scm_execl (SCM filename, SCM args);
extern SCM scm_execlp (SCM filename, SCM args);
extern SCM scm_execle (SCM filename, SCM env, SCM args);
extern SCM scm_fork (void);
extern SCM scm_uname (void);
extern SCM scm_environ (SCM env);
extern SCM scm_tmpnam (void);
extern SCM scm_mkstemp (SCM tmpl);
extern SCM scm_open_pipe (SCM pipestr, SCM modes);
extern SCM scm_close_pipe (SCM port);
extern SCM scm_utime (SCM pathname, SCM actime, SCM modtime);
extern SCM scm_access (SCM path, SCM how);
extern SCM scm_getpid (void);
extern SCM scm_putenv (SCM str);
extern SCM scm_setlocale (SCM category, SCM locale);
extern SCM scm_mknod (SCM path, SCM type, SCM perms, SCM dev);
extern SCM scm_nice (SCM incr);
extern SCM scm_sync (void);
extern SCM scm_crypt (SCM key, SCM salt);
extern SCM scm_chroot (SCM path);
extern SCM scm_getlogin (void);
extern SCM scm_cuserid (void);
extern SCM scm_getpriority (SCM which, SCM who);
extern SCM scm_setpriority (SCM which, SCM who, SCM prio);
extern SCM scm_getpass (SCM prompt);
extern SCM scm_flock (SCM file, SCM operation);
extern SCM scm_sethostname (SCM name);
extern SCM scm_gethostname (void);
extern void scm_init_posix (void);
#ifdef __cplusplus
}
#endif
#endif //GUILE_POSIX_CPP_H
