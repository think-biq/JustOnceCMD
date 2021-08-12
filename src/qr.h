
#include <qrencode.h>

/**
* Prints given QR code to stdout.
* 
* @param Text Text to base the QR code on.
* @param Quality Quality of QR code.
* @param Version Version of QR code.
* @param bCaseSensitive If tes should be treated case-sensitive.
*/
void ShowQRCode(const char* Text, QRecLevel Quality, int Version, int bCaseSensitive);