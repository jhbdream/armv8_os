#! /usr/bin/env python
# encoding: utf-8

import os
import sys

# imp 在 Python 3.12 移除，用 importlib 替代，保留回退兼容旧版本
try:
    from importlib.machinery import SourceFileLoader
    def _load_source(name, path):
        return SourceFileLoader(name, path).load_module()
except ImportError:
    import imp
    def _load_source(name, path):
        return imp.load_source(name, path)

TOP_DIRS = [
    'arch/arm64',
    'init',
    'kernel',
    'driver',
    'lib',
    'mm',
]

INCLUDE_DIRS = [
    'arch/arm64/include',
    'include',
    'include/kernel',
    'include/lib/libc',
    'include/mm',
]

def options(opt):
    opt.load('compiler_c')

def configure(conf):
    # 自动检测交叉编译工具链：优先 aarch64-elf- (macOS Homebrew)，回退 aarch64-none-linux-gnu- (Linux)
    toolchain_prefix = None
    for prefix in ['aarch64-elf-', 'aarch64-none-linux-gnu-']:
        try:
            conf.find_program(prefix + 'gcc', var='CC')
            conf.find_program(prefix + 'g++', var='CXX')
            conf.find_program(prefix + 'ar',  var='AR')
            conf.find_program(prefix + 'gcc', var='AS')
            toolchain_prefix = prefix
            conf.env.TOOLCHAIN_PREFIX = prefix
            break
        except:
            continue

    if toolchain_prefix is None:
        conf.fatal('未找到可用的交叉编译工具链 (aarch64-elf- 或 aarch64-none-linux-gnu-)')

    conf.load('compiler_c')
    conf.load('gas')

    try:
        conf.load('clang_compilation_database')
    except:
        pass



def build(bld):

    bld.env.append_value('INCLUDES', INCLUDE_DIRS)
    
    cflags = [
        '-march=armv8-a',
        '-nostdinc',
        '-fno-builtin',
        '-O0',
        '-ggdb',
        '-Wno-psabi',
        '-std=gnu99',
        '-fdiagnostics-color=always',
    ]

    bld.env.append_value('CFLAGS', cflags)

    libname_set = set()
    libs = []

    for d in TOP_DIRS:
        dir_libs = build_dir_recursive(bld, d, libname_set)
        if not dir_libs:
            bld.fatal(f"[{d}] 没有找到可构建的源文件")
        libs += dir_libs

    bld.add_group()

    ld_script = bld.path.find_resource('arch/arm64/ld_script/kernel.lds.S').abspath()

    bld(
        features='c cprogram',
        target='app',
        use=libs,
        linkflags = ['-Wl,--whole-archive', '-Wl,--start-group',],
        ldflags   = ['-Wl,--end-group', '-Wl,--no-whole-archive', '-T{}'.format(ld_script), '--static', '-nostdlib', '-nostartfiles',],
    )

    bld.add_group()

    objcopy = bld.env.TOOLCHAIN_PREFIX + 'objcopy'
    objdump = bld.env.TOOLCHAIN_PREFIX + 'objdump'
    bld(rule=objcopy + ' -O binary ${SRC} ${TGT}', source='app', target='app.bin')
    bld(rule=objdump + ' -d ${SRC} > ${TGT}', source='app', target='app.dis')

    bld.add_post_fun(_gen_compile_commands)


def _gen_compile_commands(bld):
    """在 build 完成后生成 compile_commands.json"""
    import json

    src_root  = bld.path.abspath()
    build_dir = bld.path.get_bld().abspath()
    cc        = bld.env.get_flat('CC')
    cflags    = bld.env.get_flat('CFLAGS')

    inc_flags = ' '.join(f'-I{src_root}/{d}' for d in INCLUDE_DIRS)
    commands  = []

    for d in TOP_DIRS:
        _collect_sources(bld, d, commands, cc, cflags, inc_flags, src_root, build_dir)

    with open(os.path.join(src_root, 'compile_commands.json'), 'w') as f:
        json.dump(commands, f, indent=2)


def _collect_sources(bld, dir_path, commands, cc, cflags_all, include_flags, src_root, build_dir):
    cfg = _load_source('wscript', os.path.join(dir_path, 'wscript')).get_build_config(bld)
    srcs = cfg.get('build_src', [])
    subs = cfg.get('build_dir', [])
    extra_cflags  = cfg.get('cflags', [])
    extra_includes = cfg.get('includes', [])
    extra_defines  = cfg.get('defines', [])

    for f in srcs:
        src = os.path.join(dir_path, f)
        obj = os.path.join(build_dir, dir_path, f + '.o')
        flags = ' '.join(extra_cflags)
        incs  = ' '.join(f'-I{src_root}/{d}' for d in extra_includes)
        defs  = ' '.join(f'-D{d}' for d in extra_defines)

        command = f'{cc} {cflags_all} {include_flags} {flags} {incs} {defs} -c {src} -o {obj}'

        commands.append({
            'directory': src_root,
            'file': os.path.join(src_root, src),
            'arguments': command.split(),
            'output': obj,
        })

    for sub in subs:
        _collect_sources(bld, os.path.join(dir_path, sub), commands, cc, cflags_all,
                         include_flags, src_root, build_dir)

# 递归对子目录进行编译处理
def build_dir_recursive(bld, dir_path, libname_set):
    build_conf_file = os.path.join(dir_path, 'wscript')
    build_src, build_dir, cflags, includes, defines = [], [], [], [], []

    # 加载构建配置
    if not os.path.exists(build_conf_file):
        bld.fatal("[{0}] 缺少构建配置文件：{1}".format(dir_path, build_conf_file))

    try:
        mod = _load_source('wscript', build_conf_file)
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

def _qemu_cmd(kernel='build/app.bin', debug=False):
    cmd = (
        'qemu-system-aarch64 '
        '-machine virt,gic-version=3 '
        '-cpu cortex-a57 '
        '-smp 1 '
        '-m 2048 '
        '-nographic '
        '-serial mon:stdio '
    )
    if debug:
        cmd += '-s -S '
    cmd += f'-kernel {kernel}'
    return cmd


def qemu(ctx):
    kernel = 'build/app.bin'
    if not os.path.exists(kernel):
        ctx.fatal(f"Kernel file not found: {kernel}")

    ctx.exec_command(_qemu_cmd(kernel), stdin=sys.stdin, stdout=sys.stdout, stderr=sys.stderr)


def qemu_debug(ctx):
    kernel = 'build/app.bin'
    if not os.path.exists(kernel):
        ctx.fatal(f"Kernel file not found: {kernel}")

    ctx.exec_command(_qemu_cmd(kernel, debug=True), stdin=sys.stdin, stdout=sys.stdout, stderr=sys.stderr)


def compile_commands(ctx):
    """生成 compile_commands.json 供 clangd 代码跳转使用"""
    import json

    src_root  = ctx.path.abspath()
    compdb    = os.path.join(src_root, 'compile_commands.json')

    if not os.path.exists(compdb):
        ctx.fatal('compile_commands.json not found. Run ./waf build first.')

    with open(compdb) as f:
        print(f'compile_commands.json: {len(json.load(f))} entries (generated during build)')

