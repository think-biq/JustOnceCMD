/*
The MIT License (MIT)

Copyright (c) blurryroots innovation qanat OÜ

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
/*! \file main.c
    \brief Entry point.
    
    ^^
*/

#include <JustOnce/key.h>
#include <JustOnce/otp.h>
#include <JustOnce/timing.h>
#include <qrencode.h>
#include <argparse.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <limits.h>
#include <stddef.h>
#include <inttypes.h>
#include <stdint.h>

// REplace STDIN_FILENO
#define STDIN_FILENO _fileno(stdin)
// Replace ssize_t
#if SIZE_MAX == UINT_MAX
typedef int ssize_t;        /* common 32 bit case */
#define SSIZE_MIN  INT_MIN
#define SSIZE_MAX  INT_MAX
#elif SIZE_MAX == ULONG_MAX
typedef long ssize_t;       /* linux 64 bits */
#define SSIZE_MIN  LONG_MIN
#define SSIZE_MAX  LONG_MAX
#elif SIZE_MAX == ULLONG_MAX
typedef long long ssize_t;  /* windows 64 bits */
#define SSIZE_MIN  LLONG_MIN
#define SSIZE_MAX  LLONG_MAX
#elif SIZE_MAX == USHRT_MAX
typedef short ssize_t;      /* is this even possible? */
#define SSIZE_MIN  SHRT_MIN
#define SSIZE_MAX  SHRT_MAX
#elif SIZE_MAX == UINTMAX_MAX
typedef uintmax_t ssize_t;  /* last resort, chux suggestion */
#define SSIZE_MIN  INTMAX_MIN
#define SSIZE_MAX  INTMAX_MAX
#else
#error platform has exotic SIZE_MAX
#endif
#else
#include <unistd.h>
#endif
#include <version.h>
#include "qr.h"
#include "misc.h"

#define DEFAULT_OR_CLAMP(Variable, Invalid, Default, Type, MinValue, MaxMalue) \
    Variable = Invalid == Variable ? Default : CLAMP(Type, Variable, MinValue, MaxMalue)

#define DEFAULT_OR_RETAIN(Variable, Invalid, Default) \
    Variable = Invalid == Variable ? Default : Variable

static int Global_bIsVerbose = 0;
#define PRINTF_VERBOSE(...) \
    { if (Global_bIsVerbose) { fprintf(stderr, __VA_ARGS__); } }

typedef struct argparse argparse_t;
typedef struct argparse_option argparse_option_t;

typedef struct {
    int Interval;
    int Timestamp;
    int Mode;
    int bShowOTP;
    int Digits;
    int Counter;
    int Verbose;
    char* KeyFilePath;
    char* AccountName;
    char* Issuer;
    int QRQuality;
    int QRVersion;
    int bShowHelp;
    int bShowOnlyTime;
    int bGenerateKey;
    char* KeyGenerationSeed;
    int bShowVersion;
    int bShowQR;
    int bShowURI;
    int bPrintKey;
} config_t;

int main(int Argc, const char **Argv)
{
    const char *const Usage[] = {
        "JustOnceCMD [options]",
        NULL,
    };

    config_t Config;
    Config.Interval = -1;
    Config.Timestamp = -1;
    Config.Mode = 0;
    Config.bShowOTP = 0;
    Config.Digits = -1;
    Config.Counter = 0;
    Config.Verbose = 0;
    Config.KeyFilePath = NULL;
    Config.AccountName = NULL;
    Config.Issuer = NULL;
    Config.QRQuality = 0;
    Config.QRVersion = 0;
    Config.bShowHelp = 0;
    Config.bShowOnlyTime = 0;
    Config.bGenerateKey = 0;
    Config.KeyGenerationSeed = NULL;
    Config.bShowVersion = 0;
    Config.bShowQR = 0;
    Config.bShowURI = 0;
    Config.bPrintKey = 0;

    argparse_option_t Options[] = {
        OPT_GROUP("General"),
        OPT_BOOLEAN('h', "help", &Config.bShowHelp,
            "Show this help message and exit.", argparse_help_cb, 0, OPT_NONEG),
        OPT_BOOLEAN('v', "version", &Config.bShowVersion,
            "Show version number.", NULL, 0, OPT_NONEG),
        OPT_BOOLEAN('u', "show-times", &Config.bShowOnlyTime,
            "Only show time information.", NULL, 0, OPT_NONEG),
        OPT_BOOLEAN('P', "print-key", &Config.bPrintKey,
            "Prints key.", NULL, 0, OPT_NONEG),
        OPT_BOOLEAN('V', "verbose", &Config.Verbose,
            "Output in verbose mode."),
        OPT_STRING('k', "key-file", &Config.KeyFilePath, 
            "Specify key file path. If not specified, reads from stdin."),

        OPT_GROUP("OTP"),
        OPT_BOOLEAN('g', "generate-key", &Config.bGenerateKey,
            "Generates a base32 encoded key of length 32.", NULL, 0, OPT_NONEG),
        OPT_STRING('s', "key-seed", &Config.KeyGenerationSeed,
            "Seed phrase for key generation."),
        OPT_BOOLEAN('O', "generate-otp", &Config.bShowOTP,
            "Generates new OTP (select mode via -o).", NULL, 0, OPT_NONEG),
        OPT_INTEGER('o', "otp", &Config.Mode,
            "Selectes OTP mode. 0 is TOTP. 1 is HOTP."),
        OPT_INTEGER('d', "digits", &Config.Digits,
            "Number of digits of the OTP. (default=6)"),
        OPT_INTEGER('n', "counter", &Config.Counter,
            "Counter value to be used with HOTP. (default=0)"),
        OPT_INTEGER('l', "interval", &Config.Interval, 
            "Interval to use for TOTP creation. (default=30)"),
        OPT_INTEGER('t', "timestamp", &Config.Timestamp, 
            "Unix timestamp to use for TOTP creation. (default=NOW)"),

        OPT_GROUP("Account info / QR code generation"),
        OPT_BOOLEAN('Q', "show-qr", &Config.bShowQR, 
            "Show qr code for account.", NULL, 0, OPT_NONEG),
        OPT_BOOLEAN('U', "show-uri", &Config.bShowURI,
            "Show otpauth URI.", NULL, 0, OPT_NONEG),
        OPT_STRING('a', "account", &Config.AccountName, 
            "Specifies account name. (default=NONAME)"),
        OPT_STRING('i', "issuer", &Config.Issuer, 
            "Specifies issuer name. (default=UNKNOWN)"),
        OPT_INTEGER('q', "qr-quality", &Config.QRQuality, 
            "Qualit of qr code. (default=0)"),
        OPT_INTEGER('w', "qr-version", &Config.QRVersion, 
            "Quality of qr code. (default=0)"),
        OPT_END(),
    };

    argparse_t ArgsContext;
    argparse_init(&ArgsContext, Options, Usage, 0);
    argparse_describe(&ArgsContext, 
        "\nGenerates one-time passwords.", 
        NULL);

    PRINTF_VERBOSE("Reading arguments ...\n");
    Argc = argparse_parse(&ArgsContext, Argc, Argv);

    PRINTF_VERBOSE("Setting default values for arguments ...\n");
    Global_bIsVerbose = Config.Verbose;
    DEFAULT_OR_CLAMP(Config.Interval, -1, 30, int, 1, 120);
    DEFAULT_OR_RETAIN(Config.Timestamp, -1, GetUnixTimeNow());
    DEFAULT_OR_CLAMP(Config.Digits, -1, 6, int, 3, 10);
    DEFAULT_OR_RETAIN(Config.AccountName, NULL, strdup("NONAME"));
    DEFAULT_OR_RETAIN(Config.Issuer, NULL, strdup("UNKNOWN"));
    Config.Mode = CLAMP(int, Config.Mode, 0, 1);
    Config.QRQuality = CLAMP(int, Config.QRQuality, 0, 3);
    Config.QRVersion = CLAMP(int, Config.QRVersion, 0, 40);

    PRINTF_VERBOSE(
        "Config:\n"
        "\tInterval:\t\t%d\n"
        "\tTimestamp:\t\t%d\n"
        "\tMode:\t\t\t%d\n"
        "\tbShowOTP:\t\t%d\n"
        "\tDigits:\t\t\t%d\n"
        "\tCounter:\t\t%d\n"
        "\tVerbose:\t\t%d\n"
        "\tKeyFilePath:\t\t%s\n"
        "\tAccountName:\t\t%s\n"
        "\tIssuer:\t\t\t%s\n"
        "\tQRQuality:\t\t%d\n"
        "\tQRVersion:\t\t%d\n"
        "\tbShowHelp:\t\t%d\n"
        "\tbShowOnlyTime:\t\t%d\n"
        "\tbGenerateKey:\t\t%d\n"
        "\tKeyGenerationSeed:\t%s\n"
        "\tbShowVersion:\t\t%d\n"
        "\tbShowQR:\t\t%d\n"
        "\tbShowURI:\t\t%d\n"
        "\tbPrintKey:\t\t%d\n"
        , Config.Interval
        , Config.Timestamp
        , Config.Mode
        , Config.bShowOTP
        , Config.Digits
        , Config.Counter
        , Config.Verbose
        , Config.KeyFilePath
        , Config.AccountName
        , Config.Issuer
        , Config.QRQuality
        , Config.QRVersion
        , Config.bShowHelp
        , Config.bShowOnlyTime
        , Config.bGenerateKey
        , Config.KeyGenerationSeed
        , Config.bShowVersion
        , Config.bShowQR
        , Config.bShowURI
        , Config.bPrintKey);

    if (Config.bShowHelp)
    {
        argparse_usage(&ArgsContext);
        return 0;
    }

    if (Config.bShowVersion)
    {
        printf("%s", GetVersionString());
        return 0;
    }

    if (Config.bShowOnlyTime)
    {
        if (Config.Verbose)
        {
            int TimeFrame = GetTimeFrame(Config.Timestamp, Config.Interval);
            int Progress = GetTimeFrameProgress(Config.Timestamp, Config.Interval);
            printf("%d %d %d", Config.Timestamp, TimeFrame, Progress);
        }
        else
        {
            printf("%d", Config.Timestamp);
        }

        return 0;
    }

    PRINTF_VERBOSE("Key setup ...\n");

    char Key[33];
    if (Config.bGenerateKey)
    {
        PRINTF_VERBOSE("Generating key ...\n");

        char* TempKey;
        if (NULL != Config.KeyGenerationSeed)
        {
            TempKey = GenerateKeyFromSeed((uint8_t*)Config.KeyGenerationSeed, 
                strlen(Config.KeyGenerationSeed));
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
        PRINTF_VERBOSE("Reading key ...\n");
        if (NULL != Config.KeyFilePath)
        {
            PRINTF_VERBOSE("From file %s ...\n", Config.KeyFilePath);

            char* KeyLine = NULL;
            FILE* KeyFile;
            if (KeyFile = fopen(Config.KeyFilePath, "r"))
            {
                if (NULL == KeyFile)
                {
                    fprintf(stderr, "Could not read keyfile from '%s':\n\t", Config.KeyFilePath);
                    perror("fopen");
                    return 1;
                }

                size_t BytesRead = 0;
                ssize_t LineStatus = getline(&KeyLine, &BytesRead, KeyFile);

                fclose(KeyFile);

                if (0 == LineStatus)
                {
                    fprintf(stderr, "Keyfile empty!");
                    return 1;
                }

                if (32 == BytesRead)
                {
                    fprintf(stderr, "Need 32 character key!");
                    return 1;
                }
            }

            strncpy(Key, KeyLine, 32);
            Key[32] = '\0';

            if (KeyLine)
                free(KeyLine);
        }
        else
        {
            PRINTF_VERBOSE("From stdin ...\n");

            int Read;
            #if defined(_WIN32)
            Read = fgets(Key, 32, stdin);
            if (0 > Read)
            {
                fprintf(stderr, "OUTSH!");
                return -1;
            }
            else
            {
                fprintf(stdout, "YEA! Read %i\n", Read);
            }
            #else
            Read = read(stdin, Key, 32);
            #endif
            if (10 == Key[Read-1])
                Read = 0 < Read ? Read-1 : 0;
            Key[Read] = '\0';
        }
    }

    PRINTF_VERBOSE("Normalizing key '%s' => ", Key);
    char* NormalizedKey = NormalizeKey(Key);
    PRINTF_VERBOSE("to '%s' \n", NormalizedKey);
    int bIsValidKey = IsValidKey(NormalizedKey);
    if (0 == bIsValidKey)
    {
        fprintf(stderr, "Invalid key!");
        return 1;
    }
    
    PRINTF_VERBOSE("Creating URI ...\n");
    otp_operation_t OTPMode = Config.Mode == 0 ? OTP_OP_TOTP : OTP_OP_HOTP;
    char* URI = GenerateAuthURI(OTPMode, NormalizedKey, Config.AccountName,
        Config.Issuer, Config.Digits, Config.Interval);
    
    if (Config.bShowURI)
    {        
        printf("%s\n", URI);
    }

    if (Config.bShowQR)
    {
        QRecLevel Quality = QR_ECLEVEL_L;
        switch(Config.QRQuality)
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
        ShowQRCode(URI, Quality, Config.QRVersion, 1);
    }

    free(URI);
    
    if (Config.bShowOTP)
    {
        otp_error_t OTPError;

        int OTP;
        if (0 == Config.Mode)
            OTP = CalculateTOTP(NormalizedKey, Config.Timestamp, 
                Config.Interval, Config.Digits, &OTPError);
        else if (1 == Config.Mode)
            OTP = CalculateHOTP(NormalizedKey, Config.Counter, Config.Digits,
                &OTPError);
        else
        {
            fprintf(stderr, "Invalid OTP mode!");
            return 1;            
        }

        if (OTP_SUCCESS != OTPError)
        {
            fprintf(stderr, 
                "Encountered error when calculating OTP! (ErrorCode: %d)", 
                OTPError);
            return 1;            
        }

        {
            char* Code = MakeStringFromOTP(OTP, Config.Digits);
            if (Config.Verbose)
            {
                int TimeFrame = GetTimeFrame(Config.Timestamp, Config.Interval);
                int Progress = GetTimeFrameProgress(Config.Timestamp, Config.Interval);
                printf("%s %d %d %d", Code, Config.Timestamp, TimeFrame, Progress);
            }
            else
            {
                printf("%s", Code);
            }
            free(Code);
        }

        printf("\n");
    }

    if (Config.bPrintKey)
    {
        printf("%s", NormalizedKey);
    }

    free(NormalizedKey);
    return 0;
}
