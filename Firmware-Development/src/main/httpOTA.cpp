#include "httpOTA.hpp"

WiFiClient otaWifiClient;
HttpClient http(otaWifiClient);

OTAClass::OTAClass()
{
    otaAddrOld = 0xFFFFFFFF;
    otaAddrNew = 0xFFFFFFFF;
    mdnsBuffer = NULL;
    mdnsServiceId = NULL;
    txtRecord = NULL;
}

int OTAClass::beginCloud(const char *otaServer, const char *otaServerPath, uint16_t otaServerPort, bool rebootOnSuccess)
{

    int ret = -1;

    int clientSocket = -1;

    do
    {
        if (choseOtaAddress() < 0)
            break;

        int err = http.get(otaServer, otaServerPort, otaServerPath);
        if (err == 0)
        {
            Serial.println("startedRequest ok");

            err = http.responseStatusCode();
            if (err == 200)
            {
                Serial.print("[getStatusCode] Status Code Success  ");
                Serial.println(err);
            }
            else
            {
                Serial.println("[getStatusCode]Status Code Failed");
                break;
            }
        }
        else
        {
            break;
        }

        int imageLength, imageChecksum;
        if (receiveImageHeader(imageLength, imageChecksum) < 0)
        {
            printf("[begin cloud] receiveImageHeader failed\n\r");
            break;
        }
        printf("[begin cloud] receiveImageHeader success\n\r");

        if (eraseImageFlash(imageLength) < 0)
        {
            printf("[begin cloud] eraseImageFlash failed\n\r");
            break;
        }
        printf("[begin cloud] eraseImageFlash success\n\r");

        if (receiveImage(imageLength) < 0)
        {
            printf("[begin cloud] receiveImage failed\n\r");
            break;
        }
        printf("[begin cloud] receiveImage success\n\r");

        int calculatedChecksum = 0;
        if (calculateImageChecksum(calculatedChecksum, imageLength) < 0)
        {
            printf("[begin cloud] calculateImageChecksum failed\n\r");
            break;
        }
        printf("[begin cloud] calculateImageChecksum success\n\r");

        printf("Calculatedimagecheck sum : %d\n\r", calculatedChecksum);
        printf("imageimagecheck sum : %d\n\r", imageChecksum);

        if (calculatedChecksum != imageChecksum)
        {
            printf("Bad checksum: %08X expected: %08X\r\n", (unsigned int)calculatedChecksum, (unsigned int)imageChecksum);
            break;
        }

        if (createImageHeader() < 0)
        {
            printf("[begin cloud] createImageHeader failed\n\r");
            break;
        }
        printf("[begin cloud] createImageHeader success\n\r");

        ret = 0;
        printf("OTA success\r\n");

    } while (0);

    if (ret < 0)
    {
        printf("OTA fail\r\n");
        flash_write_word(&flash, otaAddrNew, OTA_INVALID);
        flash_write_word(&flash, otaAddrOld, OTA_VALID);
    }
    else if (rebootOnSuccess)
        sys_reset();

    return ret;
}

int OTAClass::receiveImage(int imageLength)
{
    unsigned char *buf = (unsigned char *)malloc(BUFSIZE);
    if (buf == NULL)
    {
        printf("Fail to allocate memory\r\n");
        return -1;
    }

    printf("Start Download.\r\n");
    int processedLength = 0;
    int readLength = 0;

    while (processedLength < imageLength)
    {
        memset(buf, 0, BUFSIZE);

        // Read from HTTP instead of socket
        readLength = http.read(buf, BUFSIZE);

        if (readLength < 0)
        {
            printf("HTTP Read Error\r\n");
            break;
        }

        if (readLength == 0)
        {
            printf("No more data from HTTP server\r\n");
            break;
        }

        // Flash the received data
        if (flash_stream_write(&flash, otaAddrNew + processedLength, readLength, buf) < 0)
        {
            printf("Write sector fail\r\n");
            break;
        }

        processedLength += readLength;
    }

    free(buf);

    if (processedLength != imageLength)
    {
        printf("Download failed\r\n");
        return -1;
    }

    printf("Download successful\r\n");
    return 0;
}

int OTAClass::receiveImageHeader(int &imageLength, int &imageChecksum)
{
    uint8_t fileInfo[12];
    int bytesRead = 0;

    // Read HTTP headers and discard them
    while (http.available())
    {
        String line = http.readStringUntil('\n');
        if (line == "\r")
        { // End of headers
            break;
        }
    }

    // Read exactly 12 bytes of header
    while (bytesRead < 12)
    {
        if (http.available())
        {
            int byteRead = http.read();
            if (byteRead == -1)
            {
                printf("Failed to read HTTP data\r\n");
                return -1;
            }
            fileInfo[bytesRead++] = (uint8_t)byteRead;
        }
    }

    printf("Raw Header Bytes: ");
    for (int i = 0; i < 12; i++)
    {
        printf("%02X ", fileInfo[i]);
    }
    printf("\r\n");

    // Extract values using the same logic as the original TCP function
    // uint8_t to uint32_t
    imageLength = 0;
    imageChecksum = 0;

    for (int i = 0; i < 4; i++)
    {
        imageLength = (imageLength << 8) + fileInfo[11 - i];
        imageChecksum = (imageChecksum << 8) + fileInfo[3 - i];
    }

    printf("Image Length: %d\r\n", imageLength);
    printf("Image Checksum: 0x%08X\r\n", imageChecksum);

    // Extract image length from bytes 8-11 (Little Endian)
    for (int i = 0; i < 4; i++)
    {
        imageLength |= (fileInfo[8 + i] << (i * 8)); // Read as little-endian
    }

    // Extract image checksum from bytes 4-7 (Little Endian)
    for (int i = 0; i < 4; i++)
    {
        imageChecksum |= (fileInfo[4 + i] << (i * 8)); // Read as little-endian
    }
    return 0;
}

int OTAClass::choseOtaAddress()
{
    uint32_t signature1 = 1, signature2 = 1;
    flash_read_word(&flash, OTA_ADDRESS_1, &signature1);
    flash_read_word(&flash, OTA_ADDRESS_2, &signature2);
    if (signature1 == 1 || signature2 == 1)
    {
        printf("No OTA Address Found; Signature read failed.\r\n");
        return -1;
    }
    else if (signature2 == OTA_VALID)
    { // Even if both images are valid image 2 gets booted, so we overwrite image 1
        otaAddrNew = OTA_ADDRESS_1;
        otaAddrOld = OTA_ADDRESS_2;
        return 0;
    }
    else if (signature1 == OTA_VALID)
    { // Image 1 is booted so we overwrite image 2
        otaAddrNew = OTA_ADDRESS_2;
        otaAddrOld = OTA_ADDRESS_1;
        return 0;
    }
    else
    { // Both images invalid means we don't know which image is in use; this normally shouldn't happen
        printf("No OTA Address chosen to prevent Hard Fault Error.\r\n");
        printf("Currently used Image couldn't be identified.\r\n");
        return -1;
    }
}

int OTAClass::calculateImageChecksum(int &calculatedChecksum, int imageLength)
{
    if (otaAddrNew != OTA_ADDRESS_1 && otaAddrNew != OTA_ADDRESS_2)
        return -1;
    if (imageLength <= 0)
        return -1;
    unsigned char *buf = (unsigned char *)malloc(BUFSIZE);
    if (buf == NULL)
    {
        printf("Fail to allocate memory\r\n");
        return -1;
    }
    int processedLength = 0;
    calculatedChecksum = 0;
    while (processedLength < imageLength)
    {
        int readLength = (processedLength + BUFSIZE < imageLength) ? BUFSIZE : (imageLength - processedLength);
        flash_stream_read(&flash, otaAddrNew + processedLength, readLength, buf);
        for (int i = 0; i < readLength; i++)
            calculatedChecksum += (buf[i] & 0xFF);
        processedLength += readLength;
    }
    free(buf);
    return 0;
}

int OTAClass::eraseImageFlash(int imageLength)
{
    if (otaAddrNew != OTA_ADDRESS_1 && otaAddrNew != OTA_ADDRESS_2)
        return -1;
    if (imageLength <= 0)
        return -1;
    int imageSizeInSectors = ((imageLength - 1) / IMAGE_SECTOR_SIZE) + 1;
    for (int i = 0; i < imageSizeInSectors; i++)
    {
        // void function; if it fails -> Hard Fault Error :/
        flash_erase_sector(&flash, otaAddrNew + i * IMAGE_SECTOR_SIZE);
    }
    return 0;
}

int OTAClass::createImageHeader()
{
    uint32_t signature1 = 1, signature2 = 1;
    // Mark image 1 as new image
    flash_write_word(&flash, otaAddrNew + 0, OTA_VALID);
    flash_write_word(&flash, otaAddrNew + 4, OTA_VALID2);

    flash_read_word(&flash, otaAddrNew + 0, &signature1);
    flash_read_word(&flash, otaAddrNew + 4, &signature2);
    if (signature1 != OTA_VALID || signature2 != OTA_VALID2)
    {
        printf("Put signature fail\r\n");
        return -1;
    }

    // Mark image 2 as old image
    flash_write_word(&flash, otaAddrOld + 0, OTA_INVALID);

    return 0;
}

OTAClass OTA;