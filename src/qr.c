

#include <qrprint.h>

void ShowQRCode(const char* AppURI, QRecLevel Quality, int Version, int bCaseSensitive)
{
    QRprint_utf8(stdout, AppURI, Quality, Version, bCaseSensitive);
}