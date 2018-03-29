d = bytearray(open("asdf", "rb").read())

d[0x124] = 0x45
d[0x10:0x12] = "as"

open("asdf_1", "wb").write(d)

# pefile i pyelftools/python-elf

