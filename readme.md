# JustOnceCMD

Command-line tool for one-time password generation.

## Feature Overview

* Generate key to be used for OTP generation.
* Get timing information
* Generate HOTPs or TOTPs
* Generate QR code for specific configuration

## Command-line arguments

```
Usage: JustOnceCMD [options]

Generates one-time passwords.


General
    -h, --help                Show this help message and exit.
    -v, --version             Show version number.
    -u, --show-times          Only show time information.
    -g, --generate-key        Generates a base32 encoded key of length 32.
    -P, --print-key           Prints key.
    -V, --verbose             Output in verbose mode.
    -k, --key-file=<str>      Specify key file path. If not specified, reads from stdin.

OTP
    -s, --key-seed=<str>      Seed phrase for key generation.
    -o, --show-otp=<int>      Selectes OTP mode. 0 is TOTP. 1 is HOTP.
    -d, --digits=<int>        Number of digits of the OTP. (default=6)
    -c, --counter=<int>       Counter value to be used with HOTP. (default=0)
    -i, --interval=<int>      Interval to use for TOTP creation. (default=30)
    -t, --timestamp=<int>     Unix timestamp to use for TOTP creation. (default=NOW)

Account info / QR code generation
    -q, --show-qr             Show qr code for account.
    -A, --account=<str>       Specifies account name. (default=NONAME)
    -I, --issuer=<str>        Specifies issuer name. (default=UNKNOWN)
    -Q, --qr-quality=<int>    Qualit of qr code. (default=0)
    -C, --qr-version=<int>    Qualit of qr code. (default=0)
    -U, --show-url            Show otpauth url.
```

## Examples

### Configurations

#### Print otpauth URI for configuration

```bash
JustOnceCMD -U -A "neo@matr.ix" -I "architect" -i 23 -d 7 < Key.hash

```

Tells JustOnceCMD via *-U* to print a otpauth URI, with the account name (*-A*)
neo@matr.ix and issuer (*-I*) architect and reads the file Key.hash to stdin.
The mode of OTP generation is by default set to TOTP.