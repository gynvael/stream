# Changes

- added mode changing to 80x50
- added terminal backend character attribute changing API
- added string formatting API (sorry, no floats)
- reworked source files to use makefile based build system

# Building

Now building and cleaning everything is as simple as

```bash
make clean && make build
```

In this configuration it uses qemu instead of bochs (which I find kinda slow anyway...). Adding bochs support is relative easy anyway.