## RGB+CCT LED Strip Board Design
#### Client Name: Benjamin Voller-Brown


# Get LED Profile and Control LED State

### Step 1: Setup the Request
- **Method:** `POST`
- **URL:** `http://184.169.181.179:3004/api/led/state/{MAC address}`

### Step 2: Configure the Request Body
1. Open Postman or any API client.
2. Navigate to the **Body** tab.
3. Select `raw` and set the format to `JSON`.
4. Enter the following JSON in the body:
   ```json
   {
     "power": true,
     "brightness": 200,
     "color": "#00FF00"
   }
   ```

### Step 3: Send the Request
1. Click `Send`.
2. If `power` is set to `true`, the LED will turn on with the specified brightness and color.
3. If `power` is set to `false`, the LED will turn off.

# OTA Update Instructions

## Generating Binary Files from Arduino IDE
1. Open Arduino IDE and load your project.
2. Click on **Sketch** > **Export Compiled Binary**.
3. Locate the `.bin` file in the sketch folder (use **Sketch** > **Show Sketch Folder** to find it).
4. If the binary file is not found in the sketch folder, navigate to:
   ```sh
   C:\Users\{username}\AppData\Local\arduino\sketches\
   ```
5. Look for the most recently updated folder, then open it and find the file named km0_km4_image2.bin.
6. Use this `.bin` file for the OTA update process.

## Step 1: Generate Metadata and Checksum for the Binary File
1. Use the provided Python script `metaDataHeaderGen.py` to generate the checksum and metadata.
2. Open the script and set the `input_filename` variable to your binary file name.
3. Run the script using the command:
   ```sh
   python metaDataHeaderGen.py
   ```
4. The script will generate a processed binary file named `OTA_ready.bin` with metadata included.
5. Use `OTA_ready.bin` for the OTA update process.

## Step 2: Initiate the OTA Update

### 2.1 Setup the Request
- **Method:** `POST`
- **URL:** `http://184.169.181.179:3004/api/ota`

### 2.2 Configure the Request Body
1. Open Postman or any API client.
2. Navigate to the **Body** tab.
3. Select `form-data`.
4. Add a key with the name `firmware`.
5. Change the key type from `Text` to `File` (dropdown next to key name).
6. Click the `Choose Files` button and select `OTA_ready.bin`.

### 2.3 Send the Request
1. Click `Send`.
2. You should receive a JSON response:
   ```json
   {
     "message": "OTA update uploaded successfully"
   }
   ```

## Step 3: MQTT Trigger
1. Once the file is uploaded, an MQTT message will be published to trigger the OTA process.
2. **MQTT Topic:** `led/ota/all`

## Step 4: Download the OTA Firmware

### 4 Setup the Request
1. The uploaded firmware file will be automatically downloaded as the POST API is hit to upload binaries

## Step 5: Trigger OTA Sequence
1. After downloading the firmware file, initiate the OTA update process on your device.
2. Ensure the device follows the required steps for firmware flashing and updating.





