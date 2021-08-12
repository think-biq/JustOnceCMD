#include <JustOnce/key.h>
#include <JustOnce/otp.h>
#include <JustOnce/timing.h>
#include <qrencode.h>
#include <argparse.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "version.h"
#include "qr.h"
#include "misc.h"

typedef struct argparse argparse_t;
typedef struct argparse_option argparse_option_t;

int main(int Argc, const char **Argv)
{
    const char *const Usage[] = {
        "JustOnceCMD [options]",
        NULL,
    };

    int Interval = -1;
    int Timestamp = -1;
    int Mode = -1;
    int Digits = -1;
    int Counter = 0;
    int Verbose = 0;
    char* KeyFilePath = NULL;
    char* AccountName = NULL;
    char* Issuer = NULL;
    int QRQuality = 0;
    int QRVersion = 0;
    int bShowHelp = 0;
    int bShowOnlyTime = 0;
    int bGenerateKey = 0;
    char* KeyGenerationSeed = NULL;
    int bShowVersion = 0;
    int bShowQR = 0;
    int bShowURI = 0;
    int bPrintKey = 0;

    argparse_option_t Options[] = {
        OPT_GROUP("General"),
        OPT_BOOLEAN('h', "help", &bShowHelp, \
                    "Show this help message and exit.", \
                    argparse_help_cb, 0, OPT_NONEG),
        OPT_BOOLEAN('v', "Version", &bShowVersion, \
                    "Show version number.", \
                    NULL, 0, OPT_NONEG),
        OPT_BOOLEAN('u', "show-times", &bShowOnlyTime, \
                    "Only show time information.", \
                    NULL, 0, OPT_NONEG),
        OPT_BOOLEAN('g', "generate-key", &bGenerateKey, \
                    "Generates a base32 encoded key of length 32.", \
                    NULL, 0, OPT_NONEG),
        OPT_BOOLEAN('P', "print-key", &bPrintKey, \
                    "Prints key.", \
                    NULL, 0, OPT_NONEG),
        OPT_BOOLEAN('V', "verbose", &Verbose,
            "Output in verbose mode."),
        OPT_STRING('k', "key-file", &KeyFilePath, 
            "Specify key file path. If not specified, reads from stdin."),
        OPT_GROUP("OTP"),
        OPT_STRING('s', "key-seed", &KeyGenerationSeed,
            "Seed phrase for key generation."),
        OPT_INTEGER('o', "show-otp", &Mode,
            "Selectes OTP mode. 0 is TOTP. 1 is HOTP."),
        OPT_INTEGER('d', "digits", &Digits,
            "Number of digits of the OTP. (default=6)"),
        OPT_INTEGER('c', "counter", &Counter,
            "Counter value to be used with HOTP. (default=0)"),
        OPT_INTEGER('i', "interval", &Interval, 
            "Interval to use for TOTP creation. (default=30)"),
        OPT_INTEGER('t', "timestamp", &Timestamp, 
            "Unix timestamp to use for TOTP creation. (default=NOW)"),
        OPT_GROUP("Account info / QR code generation"),
        OPT_BOOLEAN('q', "show-qr", &bShowQR, "Show qr code for account.", \
                    NULL, 0, OPT_NONEG),
        OPT_STRING('A', "account", &Verbose, 
            "Specifies account name. (default=NONAME)"),
        OPT_STRING('I', "issuer", &Verbose, 
            "Specifies issuer name. (default=UNKNOWN)"),
        OPT_INTEGER('Q', "qr-quality", &QRQuality, 
            "Qualit of qr code. (default=0)"),
        OPT_INTEGER('C', "qr-version", &QRVersion, 
            "Qualit of qr code. (default=0)"),
        OPT_BOOLEAN('U', "show-url", &bShowURI, "Show otpauth url.", \
                    NULL, 0, OPT_NONEG),
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
    Mode = MAX(int, -1, MIN(int, 1, Mode));

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
    if (bGenerateKey)
    {
        char* TempKey;
        if (NULL != KeyGenerationSeed)
        {
            TempKey = GenerateKeyFromSeed(KeyGenerationSeed);
        }
        else
        {
            TempKey = GenerateKey();
        }

        if (NULL == TempKey)
        {
            fprintf(stderr, "Could not generate key!");
            return 1;
        }

        strncpy(Key, TempKey, 32);
        Key[32] = '\0';
    }
    else
    {
        if (NULL != KeyFilePath)
        {
            FILE * KeyFile;
            char * line = NULL;
            size_t len = 0;
            ssize_t read;

            KeyFile = fopen(KeyFilePath, "r");
            if (NULL == KeyFile)
            {
                argparse_usage(&ArgsContext);
                return 1;
            }

            read = getline(&line, &len, KeyFile);
            fclose(KeyFile);

            if (0 == read)
            {
                fprintf(stderr, "Keyfile empty!");
                return 1;
            }

            strncpy(Key, line, 32);
            Key[32] = '\0';

            if (line)
                free(line);
        }
        else
        {
            int Read = read(STDIN_FILENO, Key, 32);
            if (10 == Key[Read-1])
                Read = 0 < Read ? Read-1 : 0;
            Key[Read] = '\0';
        }
    }

    char* NormalizedKey = NormalizeKey(Key);
    int bIsValidKey = IsValidKey(NormalizedKey);
    if (0 == bIsValidKey)
    {
        fprintf(stderr, "Invalid key!");
        return 1;
    }

    if (bPrintKey)
    {
        printf("%s", Key);
        return 0;
    }

    const char* FMT =
        "otpauth://%s/%s?secret=%s&issuer=%s&algorithm=SHA1&digits=%i&period=%i";
    
    char URI[256];
    snprintf(URI, 256, FMT, 
        "totp", AccountName, NormalizedKey, Issuer, Digits, Interval);

    if (bShowURI)
    {        
        printf("%s\n", URI);
    }

    if (bShowQR)
    {
        QRQuality = MAX(int, MIN(int, 3, QRQuality), 0);
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

    if (-1 < Mode)
    {
        int OTP;
        if (0 == Mode)
            OTP = CalculateTOTP(NormalizedKey, Timestamp, Interval, Digits, NULL);
        else if (1 == Mode)
            OTP = CalculateHOTP(NormalizedKey, Counter, Digits, NULL);        
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