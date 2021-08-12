#include <JustOnce/key.h>
#include <JustOnce/otp.h>
#include <JustOnce/timing.h>
#include <argparse.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static const char *const usage[] = {
    "JustOnceCMD [options] [[--] args]",
    "test_argparse [options]",
    NULL,
};

int
main(int argc, const char **argv)
{
    /*const char *Key = NULL;*/
    int Interval = -1;
    int Timestamp = -1;
    int Digits = -1;
    int Verbose = 0;

    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_GROUP("Basic options"),
        OPT_INTEGER('i', "interval", &Interval, "Interval to use for TOTP creation."),
        OPT_INTEGER('t', "timestamp", &Timestamp, "Unix timestamp to use for TOTP creation. Uses now if not specified."),
        OPT_INTEGER('d', "digits", &Digits, "Number of digits of the OTP."),
        OPT_BOOLEAN('v', "verbose", &Verbose, "Output in verbose mode."),
        OPT_END(),
    };

    struct argparse argparse;
    argparse_init(&argparse, options, usage, 0);
    argparse_describe(&argparse, 
        "\nA brief description of what the program does and how it works.", 
        "\nAdditional description of the program after the description of the arguments.");

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

    argc = argparse_parse(&argparse, argc, argv);
    if (-1 == Interval)
        Interval = 30;
    if (-1 == Timestamp)
        Timestamp = GetUnixTimeNow();    
    if (-1 == Digits)
        Digits = 6;

    int OTP = CalculateTOTP(NormalizedKey, Timestamp, Interval, Digits, NULL);
    int TimeFrame = GetTimeFrame(Timestamp, Interval);
    int Progress = GetTimeFrameProgress(Timestamp, Interval);
    char* Code = MakeStringFromOTP(OTP, Digits);
    if (Verbose)
    {
        printf("%s %d %d %d", Code, Timestamp, TimeFrame, Progress);
    }
    else
    {
        printf("%s", Code);        
    }
    free(Code);

    free(NormalizedKey);

    return 0;
}