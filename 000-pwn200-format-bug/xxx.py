d = open("quotes", "rb").read().split('\0')

params = {
  "add":10,
  "sub":4,
  "mul":6,
  "div":5,
  "pow":8,
  "mod":8,
  "sin":9,
  "cos":7,
  "tan":9,
  "cot":12,
}

l = []

i = 0
for k in d:
  if len(k) == 0:
    continue
  i += 1
  if i >= 21:
    break
  l.append(k)

for func, quote in zip(l[::2], l[1::2]):
  lens = (len(quote) + len("[div] Message of the day: ") +
          len(", operands: [") +
          params[func] * (11 + 2) - 2 + 1)
  print func, lens, params[func], len(quote), quote


for func, quote in zip(l[::2], l[1::2]):
  print func
  print params[func]
  for _ in range(params[func]):
    print -0x80000000

print "quit"


