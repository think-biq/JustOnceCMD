#include <JustOnce/key.h>
#include <JustOnce/otp.h>
#include <JustOnce/timing.h>
#include <argparse.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct argparse argparse_t;
typedef struct argparse_option argparse_option_t;

#define MAX_VERSION_SIZE 16
#define VERSION_FORMAT "v%d.%d.%d"
#define MAJOR 1
#define MINOR 0
#define PATCH 10

static const char* GetVersion()
{
    static char Version[MAX_VERSION_SIZE] = { '\0' };
    if (0 == strlen(Version))
    {
        snprintf(Version, MAX_VERSION_SIZE, VERSION_FORMAT, MAJOR, MINOR, PATCH);
    }

    return Version;
}

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
    int bShowHelp = 0;
    int bShowOnlyTime = 0;
    int bShowVersion = 0;

    argparse_option_t Options[] = {
        OPT_BOOLEAN('h', "help", &bShowHelp, \
                    "Show this help message and exit.", \
                    argparse_help_cb, 0, OPT_NONEG),
        OPT_BOOLEAN('V', "Version", &bShowVersion, \
                    "Show version number.", \
                    NULL, 0, OPT_NONEG),
        OPT_BOOLEAN('u', "only-times", &bShowOnlyTime, \
                    "Only show time information.", \
                    NULL, 0, OPT_NONEG),
        OPT_INTEGER('i', "interval", &Interval, "Interval to use for TOTP creation. (default=30)"),
        OPT_INTEGER('t', "timestamp", &Timestamp, "Unix timestamp to use for TOTP creation. (default=NOW)"),
        OPT_INTEGER('d', "digits", &Digits, "Number of digits of the OTP. (default=6)"),
        OPT_BOOLEAN('v', "verbose", &Verbose, "Output in verbose mode."),
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
    free(NormalizedKey);

    return 0;
}