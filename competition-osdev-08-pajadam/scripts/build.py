#!/usr/bin/python
import os
import sys
import subprocess
from glob import glob


def fix_stage1_size():
    stage2_size = os.stat("stage2").st_size
    kernel_size = os.stat("kernel64").st_size

    stage2_size = (stage2_size + kernel_size + 511) / 512

    if stage2_size >= 255:
        raise Exception("stage2 & kernel are too large")

    with open("stage1", "rb+") as f:
        d = f.read()
        idx = d.index("\xb0\xcc\x90\x90")
        d = bytearray(d)
        d[idx + 1] = stage2_size
        f.seek(0)
        f.write(d)


def build():
    """ Build disk image with OS """
    cmds_to_run = []

    cc_flags = "-std=c99 -nostdlib -c -O0 -Wall -Wextra -masm=intel -ggdb"
    ld_flags = "-std=c99 -nostdlib -o kernel64 -O0 -Wall -Wextra -masm=intel -ggdb"

    objfiles = []

    for fname in glob("../src/os/*.c"):
        cmds_to_run.append("gcc %s %s" % (fname, cc_flags))
        objfiles.append("%s.o" % os.path.basename(os.path.splitext(fname)[0]))

    as_flags = "-masm=intel -ggdb -c"

    for fname in glob("../src/os/*.s"):
        cmds_to_run.append("gcc %s %s" % (fname, as_flags))
        objfiles.append("%s.o" % os.path.basename(os.path.splitext(fname)[0]))

    cmds_to_run.extend([
        "gcc %s %s" % (' '.join(objfiles), ld_flags),
        "strip kernel64",
        "nasm ../src/boot/stage1.asm -o stage1",
        "nasm ../src/boot/stage2.asm -o stage2",
        fix_stage1_size
    ])

    files_to_img = [
        "stage1",
        "stage2",
        "kernel64"
    ]

    for cmd in cmds_to_run:
        if type(cmd) is str:
            print "Running:", cmd
            print subprocess.check_output(cmd, shell=True)
        else:
            print "Calling:", cmd.func_name
            cmd()

    buf = []
    for fn in files_to_img:
        with open(fn, "rb") as f:
            d = f.read()
            buf.append(d)

            if len(d) % 512 == 0:
                continue

            padding_size = 512 - len(d) % 512
            buf.append("\0" * padding_size)

    with open("../bin/floppy.bin", "wb") as f:
        f.write(''.join(buf))
