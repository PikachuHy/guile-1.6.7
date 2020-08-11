import sys
sys.path.append('.')
from gen_dot_x import gen_dox_x
def main(argv):
    if len(argv) != 4:
        print("Usage: gen_dot_x_for_libguile.py input_c_source_file_dir output_x_file_dir other_options")
        exit(0)
    input_c_source_file_dir = argv[1]
    output_x_file_dir = argv[2]
    other_options = argv[3]
    dot_x_files = """
libguile/gh_io.x
libguile/rdelim.x
libguile/values.x
libguile/pairs.x
libguile/socket.x
libguile/smob.x
libguile/arbiters.x
libguile/print.x
libguile/backtrace.x
libguile/procprop.x
libguile/objects.x
libguile/filesys.x
libguile/load.x
libguile/gh_eval.x
libguile/continuations.x
libguile/hooks.x
libguile/memmove.x
libguile/macros.x
libguile/eq.x
libguile/symbols.x
libguile/options.x
libguile/gh_data.x
libguile/ports.x
libguile/gh_init.x
libguile/evalext.x
libguile/mallocs.x
libguile/strerror.x
libguile/numbers.x
libguile/keywords.x
libguile/rw.x
libguile/gdbint.x
libguile/vectors.x
libguile/simpos.x
libguile/net_db.x
libguile/gc.x
libguile/hashtab.x
libguile/mkstemp.x
libguile/hash.x
libguile/list.x
libguile/async.x
libguile/regex-posix.x
libguile/properties.x
libguile/strop.x
libguile/srcprop.x
libguile/modules.x
libguile/unif.x
libguile/cpp_err_symbols.x
libguile/cpp_signal.x
libguile/strings.x
libguile/sort.x
libguile/lang.x
libguile/boolean.x
libguile/version.x
libguile/strports.x
libguile/deprecation.x
libguile/guardians.x
libguile/stackchk.x
libguile/gh_list.x
libguile/posix.x
libguile/dynl.x
libguile/throw.x
libguile/strorder.x
libguile/inet_aton.x
libguile/environments.x
libguile/fluids.x
libguile/scmsigs.x
libguile/dynwind.x
libguile/cpp_errno.x
libguile/iselect.x
libguile/gc_os_dep.x
libguile/eval.x
libguile/script.x
libguile/vports.x
libguile/extensions.x
libguile/alist.x
libguile/symbols-deprecated.x
libguile/procs.x
libguile/weaks.x
libguile/gh_funcs.x
libguile/init.x
libguile/cpp_sig_symbols.x
libguile/struct.x
libguile/stime.x
libguile/random.x
libguile/root.x
libguile/variable.x
libguile/ioext.x
libguile/debug.x
libguile/fports.x
libguile/stacks.x
libguile/gh_predicates.x
libguile/alloca.x
libguile/chars.x
libguile/feature.x
libguile/objprop.x
libguile/read.x
libguile/ramap.x
libguile/c-tokenize.x
libguile/gsubr.x
libguile/error.x
libguile/goops.x
libguile-ltdl/guile-ltdl.x
"""
    dot_x_files = [it.strip() for row in dot_x_files.split('\n') for it in row.split(' ') if len(row) > 0 if len(it.strip()) > 0]
    print(dot_x_files)
    for file_x in dot_x_files:
        print(f'generate {file_x}')
        file = file_x[:-2]
        gen_dox_x(f'{input_c_source_file_dir}/{file}.c', f'{output_x_file_dir}/{file_x}', other_options)

if __name__ == '__main__':
    main(sys.argv)