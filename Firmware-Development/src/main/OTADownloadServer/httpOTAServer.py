import http.server
import socketserver
import os

PORT = 80  # HTTP port
FILENAME = "km0_km4_image2.bin"
INT_MAX_VALUE = 4294967295

class OTARequestHandler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        if self.path == f"/{FILENAME}":  # Serve only the OTA binary with a header
            try:
                # Calculate checksum
                checksum = 0
                length = os.path.getsize(FILENAME)
                empty = 0
                
                with open(FILENAME, "rb") as f:
                    byte = f.read(1)
                    while byte:
                        checksum += int.from_bytes(byte, 'big')
                        byte = f.read(1)
                checksum = checksum % INT_MAX_VALUE
                
                # Prepare header (12 bytes total)
                header = bytearray()
                header += checksum.to_bytes(4, 'little')  # 4-byte checksum
                header += empty.to_bytes(4, 'little')  # 4-byte empty field
                header += length.to_bytes(4, 'little')  # 4-byte file length

                # Send response headers
                self.send_response(200)
                self.send_header("Content-type", "application/octet-stream")
                self.send_header("Content-Length", str(12 + length))  # Header + file
                self.end_headers()

                # Send the 12-byte header
                self.wfile.write(header)

                # Send the binary file
                with open(FILENAME, "rb") as f:
                    self.wfile.write(f.read())
                
                print(f"Served OTA binary with header: {FILENAME}")
            except Exception as e:
                self.send_error(500, f"Server Error: {e}")
        else:
            self.send_error(404, "File Not Found")

# Start HTTP server
with socketserver.TCPServer(("", PORT), OTARequestHandler) as httpd:
    print(f"Serving OTA binary at http://192.168.2.148:{PORT}/{FILENAME}")
    httpd.serve_forever()