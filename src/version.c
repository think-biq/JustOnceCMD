#include <version.h>
#include <stdio.h>
#include <string.h>

#include <config.h>

const char* GetVersion()
{
    static char Version[MAX_VERSION_SIZE] = { '\0' };
    if (0 == strlen(Version))
    {
        snprintf(Version, MAX_VERSION_SIZE, VERSION_FORMAT, MAJOR, MINOR, PATCH);
    }

    return Version;
}