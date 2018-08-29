/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V2.0.0
* Date               : 29/8/2018
* Description        : firmware.
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/

#ifndef FIRMWARE_H
#define FIRMWARE_H

#include "user_port.h"
#include <QString>
#include <QObject>
#include <QThread>

class Firmware
{
public:
    Firmware();

    //static Firmware ProcessFirmware(const char *path);
    const Firmware* ProcessFirmware(const char *path);
    const Firmware* ProcessFirmware(QString path);
    //char *imagebyte;
    const uint8_t* image(void) const
    {
        return (const uint8_t*)imagebyte;
    }
    const uint32_t image_size(void) const
    {
        return (const uint32_t)_image_size;
    }
    uint32_t __crc32(const uint8_t bytes[], const uint32_t lenght, uint32_t state);
    int crc(const int padlen);
    int crc32(const int padlen);
    uint32_t _crc32(const uint8_t *src, unsigned len, unsigned state);
    int board_id;
    int board_revision;
private:
    static const uint32_t code_size_max;
    size_t _image_size;
    char *imagebyte;
    static const uint32_t crctab[];
    static const uint8_t crcpad[] ;//=  { 0xff, 0xff, 0xff, 0xff };
    static uint32_t crc32tab[];
};


#endif // UPLOADER_H
