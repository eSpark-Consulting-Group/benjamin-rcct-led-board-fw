import os

# Input and output file names
input_filename = "km0_km4_image2.bin"
output_filename = "OTA_ready.bin"

int_max_value = 4294967295
empty = 0

# Calculate file size
length = os.stat(input_filename).st_size

# Compute checksum (sum of all bytes mod 2^32)
checksum = 0
with open(input_filename, "rb") as f:
    byte = f.read(1)
    while byte:
        checksum = (checksum + int.from_bytes(byte, 'big')) % int_max_value
        byte = f.read(1)

# Create 12-byte header
header = bytearray()
header += checksum.to_bytes(4, 'little')  # Checksum (4 bytes)
header += empty.to_bytes(4, 'little')     # Empty (4 bytes)
header += length.to_bytes(4, 'little')    # File length (4 bytes)

# Write new file with header + original binary
with open(output_filename, "wb") as out_file:
    out_file.write(header)  # Write the header
    with open(input_filename, "rb") as in_file:
        out_file.write(in_file.read())  # Append original binary
print(out_file)
print(f"Created {output_filename} with prepended 12-byte header.")
print(f"Checksum: {hex(checksum)}")
print(f"File Length: {length} bytes")
