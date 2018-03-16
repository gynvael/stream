Patch w VICE (JMP 0x106b): `> 104e 4c 6b 10`

Oryginalne bajty w tym miejscu: `d9 32 04 b0 18 a9 ...`

Gdyby nie było autorstart możnaby zrobić tak:
```POKE 4174, 76
POKE 4175, 107
POKE 4176, 16
RUN
```

