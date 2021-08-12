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
#include <unistd.h>
#include "version.h"
#include "qr.h"
#include "misc.h"

typedef struct argparse argparse_t;
typedef struct argparse_option argparse_option_t;

typedef struct {
    int Interval;
    int Timestamp;
    int Mode;
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
    Config.Mode = -1;
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
        OPT_BOOLEAN('h', "help", &Config.bShowHelp, \
                    "Show this help message and exit.", \
                    argparse_help_cb, 0, OPT_NONEG),
        OPT_BOOLEAN('v', "version", &Config.bShowVersion, \
                    "Show version number.", \
                    NULL, 0, OPT_NONEG),
        OPT_BOOLEAN('u', "show-times", &Config.bShowOnlyTime, \
                    "Only show time information.", \
                    NULL, 0, OPT_NONEG),
        OPT_BOOLEAN('g', "generate-key", &Config.bGenerateKey, \
                    "Generates a base32 encoded key of length 32.", \
                    NULL, 0, OPT_NONEG),
        OPT_BOOLEAN('P', "print-key", &Config.bPrintKey, \
                    "Prints key.", \
                    NULL, 0, OPT_NONEG),
        OPT_BOOLEAN('V', "verbose", &Config.Verbose,
            "Output in verbose mode."),
        OPT_STRING('k', "key-file", &Config.KeyFilePath, 
            "Specify key file path. If not specified, reads from stdin."),
        OPT_GROUP("OTP"),
        OPT_STRING('s', "key-seed", &Config.KeyGenerationSeed,
            "Seed phrase for key generation."),
        OPT_INTEGER('o', "show-otp", &Config.Mode,
            "Selectes OTP mode. 0 is TOTP. 1 is HOTP."),
        OPT_INTEGER('d', "digits", &Config.Digits,
            "Number of digits of the OTP. (default=6)"),
        OPT_INTEGER('c', "counter", &Config.Counter,
            "Counter value to be used with HOTP. (default=0)"),
        OPT_INTEGER('i', "interval", &Config.Interval, 
            "Interval to use for TOTP creation. (default=30)"),
        OPT_INTEGER('t', "timestamp", &Config.Timestamp, 
            "Unix timestamp to use for TOTP creation. (default=NOW)"),
        OPT_GROUP("Account info / QR code generation"),
        OPT_BOOLEAN('q', "show-qr", &Config.bShowQR, "Show qr code for account.", \
                    NULL, 0, OPT_NONEG),
        OPT_STRING('A', "account", &Config.AccountName, 
            "Specifies account name. (default=NONAME)"),
        OPT_STRING('I', "issuer", &Config.Issuer, 
            "Specifies issuer name. (default=UNKNOWN)"),
        OPT_INTEGER('Q', "qr-quality", &Config.QRQuality, 
            "Qualit of qr code. (default=0)"),
        OPT_INTEGER('C', "qr-version", &Config.QRVersion, 
            "Qualit of qr code. (default=0)"),
        OPT_BOOLEAN('U', "show-url", &Config.bShowURI, "Show otpauth url.", \
                    NULL, 0, OPT_NONEG),
        OPT_END(),
    };

    argparse_t ArgsContext;
    argparse_init(&ArgsContext, Options, Usage, 0);
    argparse_describe(&ArgsContext, 
        "\nGenerates one-time passwords.", 
        NULL);

    Argc = argparse_parse(&ArgsContext, Argc, Argv);
    if (-1 == Config.Interval)
        Config.Interval = 30;
    if (-1 == Config.Timestamp)
        Config.Timestamp = GetUnixTimeNow();    
    if (-1 == Config.Digits)
        Config.Digits = 6;
    if (NULL == Config.AccountName)
        Config.AccountName = strdup("NONAME");
    if (NULL == Config.Issuer)
        Config.Issuer = strdup("UNKNOWN");    
    Config.Mode = MAX(int, -1, MIN(int, 1, Config.Mode));
    Config.QRQuality = MAX(int, MIN(int, 3, Config.QRQuality), 0);
    Config.QRVersion = MAX(int, MIN(int, 40, Config.QRVersion), 0);

    if (Config.bShowHelp)
    {
        argparse_usage(&ArgsContext);
        return 0;
    }

    if (Config.bShowVersion)
    {
        printf("%s", GetVersion());
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

    char Key[33];
    if (Config.bGenerateKey)
    {
        char* TempKey;
        if (NULL != Config.KeyGenerationSeed)
        {
            TempKey = GenerateKeyFromSeed(Config.KeyGenerationSeed);
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
        if (NULL != Config.KeyFilePath)
        {
            FILE* KeyFile;
            char* line = NULL;
            size_t len = 0;
            ssize_t read;

            KeyFile = fopen(Config.KeyFilePath, "r");
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
    
    char* URI = GenerateAuthURI(OTP_OP_TOTP, NormalizedKey, Config.AccountName, Config.Issuer, Config.Digits, Config.Interval);
    
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

    if (-1 < Config.Mode)
    {
        int OTP;
        if (0 == Config.Mode)
            OTP = CalculateTOTP(NormalizedKey, Config.Timestamp, Config.Interval, Config.Digits, NULL);
        else if (1 == Config.Mode)
            OTP = CalculateHOTP(NormalizedKey, Config.Counter, Config.Digits, NULL);        
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
    }

    if (Config.bPrintKey)
    {
        printf("%s", NormalizedKey);
    }

    free(NormalizedKey);
    return 0;
}