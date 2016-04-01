from time import sleep
from sys import stdout
import types

c = [
    0b1111000, # 0
    0b101111,  # 1
    0b0,       # 2
    0b1111100, # 3
    0b0,       # 4
    0b0,       # 5
    0b1000100, # 6
    0b1011101, # 7
    0b100111,  # 8
    0b0,       # 9
    0b1111101, # 10
    0b1,       # 11
    0b0,       # 12
    0b1110100, # 13
    0b0,       # 14
    0b0,       # 15
    0b1101010, # 16
    0b1,       # 17
    0b0,       # 18
    0b1111100, # 19
    0b1,       # 20
    0b0,       # 21
    0b10000011, # 22
    0b1,       # 23
    0b0,       # 24
    0b1,       # 25
    0b1110100, # 26
    0b0,       # 27
    0b0,       # 28
    0b1101010, # 29
    0b10,      # 30
    0b0,       # 31
    0b10000011, # 32
    0b0,       # 33
    0b0,       # 34
    0b1,       # 35
    0b1110100, # 36
    0b11,      # 37
    0b0,       # 38
    0b1100100, # 39
    0b1,       # 40
    0b0,       # 41
    0b10000011, # 42
    0b1,       # 43
    0b0,       # 44
    0b1,       # 45
    0b1110001, # 46
    0b111,     # 47
    0b0,       # 48
    0b1010111, # 49
    0b1100100, # 50
    0b0,       # 51
    0b0,       # 52
    0b1010011, # 53
]

hello_code = types.CodeType(
    1, # il arg
    2, # il zmiennych lokalnych
    3, # wielkosc stosu
    67, # flagi
    ''.join(map(chr, c)), # kod
    (None, 1.0), # stale
    ('stdout', 'write', 'flush', 'sleep'), # nazwy / funkcje / obiekty
    ('b', 'c'), # zmienne lokalne
    "", 
    "hello", # nazwa funkcji
    1, "")

hello = types.FunctionType(hello_code, globals())
hello.func_defaults = ("Hello world!",)

hello()
hello("1234")





