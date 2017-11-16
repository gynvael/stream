import math
import struct
import pickle



def goertzel(samples, sample_rate, *freqs):
    """
    Implementation of the Goertzel algorithm, useful for calculating individual
    terms of a discrete Fourier transform.

    `samples` is a windowed one-dimensional signal originally sampled at `sample_rate`.

    The function returns 2 arrays, one containing the actual frequencies calculated,
    the second the coefficients `(real part, imag part, power)` for each of those frequencies.
    For simple spectral analysis, the power is usually enough.

    Example of usage :

        # calculating frequencies in ranges [400, 500] and [1000, 1100]
        # of a windowed signal sampled at 44100 Hz

        freqs, results = goertzel(some_samples, 44100, (400, 500), (1000, 1100))
    """
    window_size = len(samples)
    f_step = sample_rate / float(window_size)
    f_step_normalized = 1.0 / window_size

    # Calculate all the DFT bins we have to compute to include frequencies
    # in `freqs`.
    bins = set()
    for f_range in freqs:
        f_start, f_end = f_range
        k_start = int(math.floor(f_start / f_step))
        k_end = int(math.ceil(f_end / f_step))

        if k_end > window_size - 1: raise ValueError('frequency out of range %s' % k_end)
        bins = bins.union(range(k_start, k_end))

    # For all the bins, calculate the DFT term
    n_range = range(0, window_size)
    freqs = []
    results = []
    for k in bins:

        # Bin frequency and coefficients for the computation
        f = k * f_step_normalized
        w_real = 2.0 * math.cos(2.0 * math.pi * f)
        w_imag = math.sin(2.0 * math.pi * f)

        # Doing the calculation on the whole sample
        d1, d2 = 0.0, 0.0
        for n in n_range:
            y  = samples[n] + w_real * d1 - d2
            d2, d1 = d1, y

        # Storing results `(real part, imag part, power)`
        results.append((
            0.5 * w_real * d1 - d2, w_imag * d1,
            d2**2 + d1**2 - w_real * d1 * d2)
        )
        freqs.append(f * sample_rate)
    return freqs, results


def g(samples):
  f, r = goertzel(samples, 44100, (4000-300, 4000+300), (5400-300, 5400+300))
  return r[2][2], r[0][2]


with open("dump_atari.wav", "rb") as f:
  d = bytearray(f.read())

print "Init"
dd = []
for i in range(0, len(d), 2):
  k = struct.unpack("H", d[i:i+2])[0] / 32768.
  dd.append(k)

sz = len(dd)
d = dd

K = 66

r_max = -1000000000
r_min = 1000000000

print "Goertzel"
ll = []
for i in range(0, sz - K):
  r0, r1 = g(d[i:i+K])
  r_max = max(r0, r1, r_max)
  r_min = min(r0, r1, r_min)
  ll.append((r0, r1))

print "Norm"
kk = []
for l0, l1 in ll:
  l0 = int(((l0 - r_min) / (r_max - r_min)) * 255)
  l1 = int(((l1 - r_min) / (r_max - r_min)) * 255)

  l0 = (l0 > 40) * 255
  l1 = (l1 > 40) * 255
  #o = (chr(l0) * 32) + (chr(l1) * 32)
  kk.append(chr(l0) + chr(l1))

kk = ''.join(kk)

with open("g.dump3_big", "wb") as f:
  f.write(kk)

#with open("g.dump_big", "rb") as f:
  #kk = f.read()


MARKER = "01010101010101010101"

def decode_bit(d, offset):
  vff = 0
  v00 = 0

  for i in range(-15, 16):
    idx = offset + i * 2 + 1
    if d[idx] == "\0":
      v00 += 1
    else:
      vff += 1

  return "01"[vff > v00]

def decode_bits(d, offset, sz):
  o = ""
  for i in range(sz):
    o += decode_bit(d, offset + i * 74 * 2)
  return o


def decode_block(b):
  idx = 30
  o = ""
  for i in range(128):
    bits = b[idx+1:idx+8+1]
    bits = chr(int(bits[::-1], 2))
    o += bits
    idx += 10

  return o

def chk_start(d, offset):
  bits = decode_bits(d, offset, 20);
  if bits == MARKER:
    print offset, "MARKER"
    bits = decode_bits(d, offset, 1320)
    return decode_block(bits)

  return False


f = open("dump3.bin", "wb")

print "Read"
i = 18000
while i < len(kk) - 5000:
  #print i/2,
  res = chk_start(kk, i)
  if type(res) is str:
    f.write(res)
    i += 98000 * 2
  else:
    i += 2  # TODO


#with open("dump.data", "wb") as f:
  #f.write(''.join(kk))









