# Changes

- added mode changing to 80x50
- added terminal backend character attribute changing API
- reworked source files to use makefile based build system

# Building

Now everything is enclosed in single

```bash
make clean && make build
```

In this configuration it uses qemu instead of bochs (which I find kinda slow anyway...). Adding bochs support is relative easy anyway.