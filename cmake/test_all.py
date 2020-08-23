# -*- coding: utf-8 -*-

import os
import sys
import subprocess
import platform
import argparse

test_ret = {
    'fail': 0,
    'pass': 0
}


def print_test_header(s: str):
    print('=' * len(s))
    print(s)


def print_test_footer(s: str):
    print(s)
    print('=' * len(s))


def print_dir(s: str):
    s = f'test {s}'
    padding = 10
    pad_space = ' ' * padding
    print('=' * (len(s) + (padding + 1) * 2))
    print(f'={pad_space}{s}{pad_space}=')
    print('=' * (len(s) + (padding + 1) * 2))


def run_cmd(cmd: str, work_dir: str):
    if platform.system() == 'Windows':
        if cmd.startswith("./"):
            cmd = cmd[2:]
    print(f'work dir: {work_dir}')
    print(cmd)
    cmd = [_cmd for _cmd in cmd.split(' ') if len(_cmd) > 0]
    tmp_path = getenv('PATH')
    # os.environ['PATH'] = f'{work_dir}:{tmp_path}'
    ret = subprocess.call(cmd, cwd=work_dir, shell=True, env=os.environ)
    if ret != 0:
        print('\033[31;1m FAIL \033[0m')
        test_ret['fail'] += 1
    else:
        print('\033[92;1m PASS \033[0m')
        test_ret['pass'] += 1
    os.environ['PATH'] = tmp_path

def run_test(name: str, work_dir: str, cmd: str = None, fn=None):
    guile_load_path_save = os.getenv("GUILE_LOAD_PATH")
    os.environ["GUILE_LOAD_PATH"]=f'{work_dir}{sep}{guile_load_path_save}'
    print_test_header(name)
    if cmd is not None:
        run_cmd(cmd, work_dir)
    if fn is not None:
        fn(name, work_dir)
    print_test_footer(name + ' done')
    os.environ["GUILE_LOAD_PATH"]=guile_load_path_save


def print_test_report():
    total = test_ret['pass'] + test_ret['fail']
    print(f'total: {total}')
    print(f'pass: {test_ret["pass"]}')
    print(f'fail: {test_ret["fail"]}')

def getenv(key: str):
    ret = os.getenv(key)
    if ret is None:
        return ""
    return ret

def main():
    guile_srcdir = args.srcdir
    guile_dstdir = args.dstdir
    guile_main = f'guile'
    os.environ['PATH'] = f'{guile_dstdir}{sep}{getenv("PATH")}'
    if platform.system() != 'Windows':
        os.environ['GUILE_LOAD_PATH'] = '.'
        os.environ['LD_LIBRARY_PATH'] = '.'
        os.environ['GUILE_LOAD_PATH'] = f'{guile_srcdir}{sep}{getenv("GUILE_LOAD_PATH")}'
        os.environ['LD_LIBRARY_PATH'] = f'{guile_dstdir}{sep}{getenv("LD_LIBRARY_PATH")}'
    else:
        # os.environ['GUILE_LOAD_PATH'] = f'{guile_srcdir}'
        # os.environ['LD_LIBRARY_PATH'] = f'{guile_dstdir}'
        os.environ['GUILE_LOAD_PATH'] = f'{guile_srcdir}:{getenv("GUILE_LOAD_PATH")}'
        os.environ['LD_LIBRARY_PATH'] = f'{guile_dstdir}{sep}{getenv("LD_LIBRARY_PATH")}'

    print(f"""
GUILE_LOAD_PATH: {getenv("GUILE_LOAD_PATH")}
LD_LIBRARY_PATH: {getenv("LD_LIBRARY_PATH")}
    """)
    print_dir('example')
    run_test('example box', f'{guile_dstdir}/examples/box', f'./box -s {guile_srcdir}/examples/box/script.scm')
    run_test('example box-dynamic', f'{guile_dstdir}/examples/box-dynamic', f'{guile_main} -s {guile_srcdir}/examples/box-dynamic/script.scm')

    def run_test_example_box_dynamic_module(name, work_dir):
        os.environ['GUILE_LOAD_PATH'] = f'{guile_srcdir}{sep}{work_dir}'
        for subtest in ['box-mixed', 'box-module']:
            print(f'> run {subtest}')
            cmd = f'{guile_main} -s {guile_srcdir}/examples/box-dynamic-module/{subtest}-script.scm'
            run_cmd(cmd, work_dir)

    run_test('example box-dynamic-module', f'{guile_dstdir}/examples/box-dynamic-module',
             fn=run_test_example_box_dynamic_module)
    run_test('example box-module', f'{guile_dstdir}/examples/box-module', f'./box -s {guile_srcdir}/examples/box-module/script.scm')

    run_test('examples modules', f'{guile_srcdir}/examples/modules', f'{guile_main} -s {guile_srcdir}/examples/modules/main')
    def run_test_example_safe(name, work_dir):
        for loop in ['evil', 'untrusted']:
            print(f'> {loop}')
            run_cmd(f'{guile_main} -s safe {loop}.scm', work_dir)

    run_test('examples safe', f'{guile_srcdir}/examples/safe', fn=run_test_example_safe)

    def run_test_example_scripts(name, work_dir):
        print('> run fact 5')
        run_cmd(f'{guile_main} -s {guile_srcdir}/examples/scripts/fact 5', work_dir)
        for loop in ['hello', 'simple-hello.scm']:
            print(f'> run {loop}')
            run_cmd(f'{guile_main} -s {guile_srcdir}/examples/scripts/{loop}', work_dir)

    run_test('exmaple scripts', f'{guile_srcdir}/examples/scripts', fn=run_test_example_scripts)

    print_dir('test-suite')

    guile_test = f'{guile_srcdir}/test-suite/guile-test'
    run_test('test-suite tests', f'{guile_srcdir}/test-suite', f'{guile_main} -e main -s {guile_srcdir}/test-suite/guile-test --test-suite {guile_srcdir}/test-suite/tests')
    print_test_report()
    print('all done!!!')


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--srcdir')
    parser.add_argument('--dstdir')
    parser.add_argument('--msvc', action='store_true', default=False)
    args = parser.parse_args()
    if args.msvc:
        sep = ';'
    else:
        sep = ';'
    main()
