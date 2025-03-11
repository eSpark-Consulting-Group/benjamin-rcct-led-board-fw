import pandas as pd
import requests

# Load the Excel file
file_path = "Sunrise Profile.xlsx"  # Update path if needed
df = pd.read_excel(file_path)

# Convert all values to integers
df = df.astype(int)

# Prepare data as a list of integer tuples
data_values = df[["Time", "R_Value", "WW_Value", "CW_Value"]].values.tolist()

# Convert to bytes (optional: for binary transmission)
binary_data = bytearray()
for entry in data_values:
    for value in entry:
        binary_data.extend(value.to_bytes(4, byteorder="little", signed=False))  # 4 bytes per int

# Save to a binary file
bin_file_path = "output_data.bin"
with open(bin_file_path, "wb") as bin_file:
    bin_file.write(binary_data)

print(f"Binary data saved to {bin_file_path}")

# Send as raw binary data (or keep as integers in a list for text-based sending)
endpoint_url = "http://your-http-endpoint.com/upload"  # Replace with actual endpoint
headers = {"Content-Type": "application/octet-stream"}  # Sending as binary

# try:
#     with open(bin_file_path, "rb") as bin_file:
#         response = requests.post(endpoint_url, data=bin_file, headers=headers)
#     print(f"Response Code: {response.status_code}")
#     print(f"Response Body: {response.text}")
# except Exception as e:
#     print(f"Error: {e}")
