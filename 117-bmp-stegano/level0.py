# ChatGPT-4 generated with fixes by gynvael
from PIL import Image

def extract_lsb_bit_order1(image_path):
    image = Image.open(image_path)
    pixels = image.convert('RGB').getdata()
    binary_data = ''
    for pixel in pixels:
        for value in pixel[::-1]:
            binary_data += str(value & 1)  # Extract the least significant bit
    bytes_data = [chr(int(binary_data[i: i + 8], 2)) for i in range(0, len(binary_data), 8)]
    return ''.join(bytes_data).rstrip('\x00')


def extract_lsb_bit_order2(image_path):
    image = Image.open(image_path)
    pixels = image.convert('RGB').getdata()
    binary_data = ''
    for pixel in pixels:
        for value in pixel[::-1]:
            binary_data += str(value & 1)  # Extract the least significant bit
    reversed_bits = [''.join(reversed(binary_data[i: i + 8])) for i in range(0, len(binary_data), 8)]
    bytes_data = [chr(int(bit, 2)) for bit in reversed_bits]
    return ''.join(bytes_data).rstrip('\x00')

#image_path = "path/to/your/image.bmp"

#print("Hidden data:", hidden_data)


image_path = "level0.bmp"
#hidden_data = extract_lsb_bit_order2(image_path)
hidden_data = extract_lsb_bit_order1(image_path)
print("Hidden data:", hidden_data)

hidden_data = extract_lsb_bit_order2(image_path)
print("Hidden data:", hidden_data)
