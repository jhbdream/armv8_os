#! /usr/bin/env python
# encoding: utf-8

import os
import imp

top = '.'

from waflib import Configure, Logs, Utils

def options(opt):
    opt.load('gcc gas')

def configure(conf):
    conf.load('gcc gas')

def build(bld):

    include_dirs = [
        'arch/arm64/include',
        'include',
        'include/kernel',
        'include/lib/libc',
    ]
    
    bld.env.append_value('INCLUDES', include_dirs)
    
    cflags = [
        '-nostdinc',
        '-fno-builtin',
        '-O0',
        '-ggdb',
        '-Wno-psabi',
        '-std=gnu99',
    ]

    bld.env.append_value('CFLAGS', cflags)

    # 参与编译的子目录
    top_dirs = [
        'arch/arm64',
        'init',
        'kernel',
        'driver',
        'lib',
    ]

    libname_set = set()
    libs = []

    for d in top_dirs:
        libs += build_dir_recursive(bld, d, libname_set)
        if not libs:
            bld.fatal(f"[{d}] 没有找到可构建的源文件")

    ld_script = bld.path.find_resource('arch/arm64/ld_script/kernel.lds.S').abspath()

    bld.program(
        features='c cprogram',
        target='app',
        use=libs,
        linkflags = ['-Wl,--whole-archive', '-Wl,--start-group',],
        ldflags   = ['-Wl,--end-group', '-Wl,--no-whole-archive', '-T{}'.format(ld_script), '--static', '-nostdlib', '-nostartfiles',],
    )

    bld(rule='aarch64-none-linux-gnu-objcopy -O binary ${SRC} ${TGT}', source='app', target='app.bin')
    bld(rule='aarch64-none-linux-gnu-objdump -d ${SRC} > ${TGT}', source='app', target='app.dis')

# 递归对子目录进行编译处理
def build_dir_recursive(bld, dir_path, libname_set):
    build_conf_file = os.path.join(dir_path, 'wscript')
    build_src, build_dir, cflags, includes, defines = [], [], [], [], []

    # 加载构建配置
    if not os.path.exists(build_conf_file):
        bld.fatal("[{0}] 缺少构建配置文件：{1}".format(dir_path, build_conf_file))

    try:
        mod = imp.load_source('wscript', build_conf_file)
    except Exception as e:
        bld.fatal("[{0}] 加载构建配置失败: {1}".format(dir_path, str(e)))

    cfg = mod.get_build_config(bld)
    build_src = cfg.get('build_src', [])
    build_dir = cfg.get('build_dir', [])
    cflags = cfg.get('cflags', [])
    includes = cfg.get('includes', [])
    defines = cfg.get('defines', [])

    src_list = [os.path.join(dir_path, f) for f in build_src]

    lib_name = dir_path.replace(os.sep, '_') + '_lib'
    if lib_name in libname_set:
        bld.fatal("Duplicate lib target name: {0} in {1}".format(lib_name, dir_path))

    libname_set.add(lib_name)

    if src_list:
        bld.stlib(
            target=lib_name,
            source=src_list,
            includes=includes,
            cflags=cflags,
            defines=defines,
        )

    sub_libs = []
    for sub in build_dir:
        libs = build_dir_recursive(bld, os.path.join(dir_path, sub), libname_set)
        if libs:
            sub_libs.extend(libs)

    return ([lib_name] if src_list else []) + sub_libs

def qemu(ctx):
    import os, sys

    kernel = 'build/app.bin'  # 可以改成 build/eeos.bin 或通过参数传入
    if not os.path.exists(kernel):
        ctx.fatal(f"Kernel file not found: {kernel}")

    cmd = (
        'qemu-system-aarch64 '
        '-machine virt,gic-version=3 '
        '-cpu cortex-a57 '
        '-smp 1 '
        '-m 2048 '
        '-nographic '
        '-serial mon:stdio '
        '-kernel build/app.bin'
    )
    ctx.exec_command(cmd, stdin=sys.stdin, stdout=sys.stdout, stderr=sys.stderr)

