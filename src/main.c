#include <JustOnce/key.h>
#include <JustOnce/otp.h>
#include <JustOnce/timing.h>
#include <qrencode.h>
#include <argparse.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define GENERIC_MAX(x, y) ((x) > (y) ? (x) : (y))
#define GENERIC_MIN(x, y) ((x) < (y) ? (x) : (y))

#define ENSURE_int(i)   _Generic((i), int:   (i))
#define ENSURE_float(f) _Generic((f), float: (f))

#define MAX(type, x, y) \
  (type)GENERIC_MAX(ENSURE_##type(x), ENSURE_##type(y))
#define MIN(type, x, y) \
  (type)GENERIC_MIN(ENSURE_##type(x), ENSURE_##type(y))

#define MAX_VERSION_SIZE 16
#define VERSION_FORMAT "v%d.%d.%d"
#define MAJOR 1
#define MINOR 0
#define PATCH 10

typedef struct argparse argparse_t;
typedef struct argparse_option argparse_option_t;

static const char* GetVersion()
{
    static char Version[MAX_VERSION_SIZE] = { '\0' };
    if (0 == strlen(Version))
    {
        snprintf(Version, MAX_VERSION_SIZE, VERSION_FORMAT, MAJOR, MINOR, PATCH);
    }

    return Version;
}

#if 1

static void writeUTF8_margin(FILE* fp, int margin, int realwidth, const char* white,
                             const char *reset, const char* full)
{
    int x, y;

    for (y = 0; y < margin/2; y++) {
        fputs(white, fp);
        for (x = 0; x < realwidth; x++)
            fputs(full, fp);
        fputs(reset, fp);
        fputc('\n', fp);
    }
}

static int writeUTF8(const QRcode *qrcode, int use_ansi, int invert, int margin)
{
    FILE *fp;
    int x, y;
    int realwidth;
    const char *white, *reset;
    const char *empty, *lowhalf, *uphalf, *full;

    empty = " ";
    lowhalf = "\342\226\204";
    uphalf = "\342\226\200";
    full = "\342\226\210";

    if (invert) {
        const char *tmp;

        tmp = empty;
        empty = full;
        full = tmp;

        tmp = lowhalf;
        lowhalf = uphalf;
        uphalf = tmp;
    }

    if (use_ansi){
        if (use_ansi == 2) {
            white = "\033[38;5;231m\033[48;5;16m";
        } else {
            white = "\033[40;37;1m";
        }
        reset = "\033[0m";
    } else {
        white = "";
        reset = "";
    }

    //fp = openFile(outfile);
    fp = stdout;

    realwidth = (qrcode->width + margin * 2);

    /* top margin */
    writeUTF8_margin(fp, 0, realwidth, white, reset, full);

    /* data */
    for(y = 0; y < qrcode->width; y += 2) {
        unsigned char *row1, *row2;
        row1 = qrcode->data + y*qrcode->width;
        row2 = row1 + qrcode->width;

        fputs(white, fp);

        for (x = 0; x < margin; x++) {
            fputs(full, fp);
        }

        for (x = 0; x < qrcode->width; x++) {
            if(row1[x] & 1) {
                if(y < qrcode->width - 1 && row2[x] & 1) {
                    fputs(empty, fp);
                } else {
                    fputs(lowhalf, fp);
                }
            } else if(y < qrcode->width - 1 && row2[x] & 1) {
                fputs(uphalf, fp);
            } else {
                fputs(full, fp);
            }
        }

        for (x = 0; x < margin; x++)
            fputs(full, fp);

        fputs(reset, fp);
        fputc('\n', fp);
    }

    /* bottom margin */
    writeUTF8_margin(fp, 0, realwidth, white, reset, full);

    //fclose(fp);

    return 0;
}

static void ShowQRCode(const char* AppURI, QRecLevel Quality, int Version, int bCaseSensitive)
{
    QRcode* Code = QRcode_encodeString(
        AppURI,
        Version,
        Quality,
        QR_MODE_8,
        bCaseSensitive
    );

    writeUTF8(Code, 1, 0, 0);

    free(Code);
}
#endif

static const char *const Usage[] = {
    "JustOnceCMD [options] [< key_file]",
    NULL,
};

int main(int Argc, const char **Argv)
{
    int Interval = -1;
    int Timestamp = -1;
    int Digits = -1;
    int Verbose = 0;
    char* AccountName = NULL;
    char* Issuer = NULL;
    int QRQuality = 0;
    int QRVersion = 0;
    int bShowHelp = 0;
    int bShowOnlyTime = 0;
    int bShowVersion = 0;
    int bShowQR = 0;

    argparse_option_t Options[] = {
        OPT_BOOLEAN('h', "help", &bShowHelp, \
                    "Show this help message and exit.", \
                    argparse_help_cb, 0, OPT_NONEG),
        OPT_BOOLEAN('v', "Version", &bShowVersion, \
                    "Show version number.", \
                    NULL, 0, OPT_NONEG),
        OPT_BOOLEAN('u', "only-times", &bShowOnlyTime, \
                    "Only show time information.", \
                    NULL, 0, OPT_NONEG),
        OPT_BOOLEAN('q', "show-qr", &bShowQR, \
                    "Show qr code for account.", \
                    NULL, 0, OPT_NONEG),
        OPT_INTEGER('i', "interval", &Interval, "Interval to use for TOTP creation. (default=30)"),
        OPT_INTEGER('t', "timestamp", &Timestamp, "Unix timestamp to use for TOTP creation. (default=NOW)"),
        OPT_INTEGER('d', "digits", &Digits, "Number of digits of the OTP. (default=6)"),
        OPT_BOOLEAN('V', "verbose", &Verbose, "Output in verbose mode."),
        OPT_STRING('A', "account", &Verbose, "Specifies account name. (Default=NONAME)"),
        OPT_STRING('I', "issuer", &Verbose, "Specifies issuer name. (Default=UNKNOWN)"),
        OPT_INTEGER('Q', "qr-quality", &QRQuality, "Qualit of qr code."),
        OPT_INTEGER('C', "qr-version", &QRVersion, "Qualit of qr code."),
        OPT_END(),
    };

    argparse_t ArgsContext;
    argparse_init(&ArgsContext, Options, Usage, 0);
    argparse_describe(&ArgsContext, 
        "\nGenerates one-time passwords.", 
        NULL);

    Argc = argparse_parse(&ArgsContext, Argc, Argv);
    if (-1 == Interval)
        Interval = 30;
    if (-1 == Timestamp)
        Timestamp = GetUnixTimeNow();    
    if (-1 == Digits)
        Digits = 6;
    if (NULL == AccountName)
        AccountName = strdup("NONAME");
    if (NULL == Issuer)
        Issuer = strdup("UNKNOWN");

    if (bShowHelp)
    {
        argparse_usage(&ArgsContext);
        return 0;
    }

    if (bShowVersion)
    {
        printf("%s", GetVersion());
        return 0;
    }

    if (bShowOnlyTime)
    {
        if (Verbose)
        {
            int TimeFrame = GetTimeFrame(Timestamp, Interval);
            int Progress = GetTimeFrameProgress(Timestamp, Interval);
            printf("%d %d %d", Timestamp, TimeFrame, Progress);
        }
        else
        {
            printf("%d", Timestamp);
        }

        return 0;
    }

    char Key[33];
    int Read = read(STDIN_FILENO, Key, 32);
    if (10 == Key[Read-1])
        Read = 0 < Read ? Read-1 : 0;
    Key[Read] = '\0';

    char* NormalizedKey = NormalizeKey(Key);
    int bIsValidKey = IsValidKey(NormalizedKey);
    if (0 == bIsValidKey)
    {
        return 1;
    }

    if (bShowQR)
    {
        const char* FMT = "otpauth://%s/%s?secret=%s&issuer=%s&algorithm=SHA1&digits=%i&period=%i";
        char URI[256];
        snprintf(URI, 256, FMT, "totp", AccountName, NormalizedKey, Issuer, Digits, Interval);
        //printf("\n");
        QRecLevel Quality = QR_ECLEVEL_L;
        switch(QRQuality)
        {
            default:
            case 0:
                Quality = QR_ECLEVEL_L;
                break;
            case 1:
                Quality = QR_ECLEVEL_M;
                break;
            case 2:
                Quality = QR_ECLEVEL_Q;
                break;
            case 3:
                Quality = QR_ECLEVEL_H;
                break;
        }
        int Version = MAX(int, MIN(int, 40, QRVersion), 0);
        ShowQRCode(URI, Quality, Version, 1);
    }
    else
    {
        int OTP = CalculateTOTP(NormalizedKey, Timestamp, Interval, Digits, NULL);
        {
            char* Code = MakeStringFromOTP(OTP, Digits);
            if (Verbose)
            {
                int TimeFrame = GetTimeFrame(Timestamp, Interval);
                int Progress = GetTimeFrameProgress(Timestamp, Interval);
                printf("%s %d %d %d", Code, Timestamp, TimeFrame, Progress);
            }
            else
            {
                printf("%s", Code);
            }
            free(Code);
        }
    }
    free(NormalizedKey);

    return 0;
}