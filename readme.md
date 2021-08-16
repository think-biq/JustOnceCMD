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
    -P, --print-key           Prints key.
    -V, --verbose             Output in verbose mode.
    -k, --key-file=<str>      Specify key file path. If not specified, reads from stdin.

OTP
    -g, --generate-key        Generates a base32 encoded key of length 32.
    -s, --key-seed=<str>      Seed phrase for key generation.
    -O, --generate-otp        Generates new OTP (select mode via -o).
    -o, --otp=<int>           Selectes OTP mode. 0 is TOTP. 1 is HOTP.
    -d, --digits=<int>        Number of digits of the OTP. (default=6)
    -n, --counter=<int>       Counter value to be used with HOTP. (default=0)
    -l, --interval=<int>      Interval to use for TOTP creation. (default=30)
    -t, --timestamp=<int>     Unix timestamp to use for TOTP creation. (default=NOW)

Account info / QR code generation
    -Q, --show-qr             Show qr code for account.
    -U, --show-uri            Show otpauth URI.
    -a, --account=<str>       Specifies account name. (default=NONAME)
    -i, --issuer=<str>        Specifies issuer name. (default=UNKNOWN)
    -q, --qr-quality=<int>    Qualit of qr code. (default=0)
    -w, --qr-version=<int>    Quality of qr code. (default=0)
```

## Examples

### OTPs

If you select TOTP (*-o 0*) you can configure a specific unix timestamp through *-t*. The default period is 30s and the default digit length is 6. For HOTP (*-o 1*), you can specify the counter value through *-n*. The default counter value is 0.

#### TOTP

```bash
JustOnceCMD -O -o 0 -t 626633580 < Key.hash
```

Example result:

```bash
576235
```

#### HOTP

```bash
JustOnceCMD -O -o 1 -n 23 < Key.hash
```

Example result:

```bash
282527
```

### Account configurations

#### Print otpauth URI for configuration

```bash
JustOnceCMD -U -a "neo@matr.ix" -i "architect" -l 23 -d 7 < Key.hash
```

*-U* tells JustOnceCMD to print an [otpauth URI](https://github.com/google/google-authenticator/wiki/Key-Uri-Format), with the account name (*-a*)
'neo@matr.ix' and issuer/app name (*-i*) 'architect'. It configures the OTP to have a validity period (*-l*) of 23s and a length of 7 digits. The key file *Key.hash* is read through stdin.
The mode of OTP generation is by default set to TOTP and can be configure through (*-o 0* for TOTP, *-o 1* for HOTP). 

Example result:
```bash
otpauth://totp/neo%40matr.ix?secret=GQ3DONJWGM3EEMRQG42DMOBWGUZDANRU&issuer=architect&algorithm=SHA1&digits=7&period=23
```

#### Generate QR code

```bash
JustOnceCMD -Q -a "neo@matr.ix" -i "architect" -l 23 -d 7 < Key.hash

```

Tells JustOnceCMD via *-Q* to print a otpauth URI as QR code. You can specify
the [quality](https://www.qrcode.com/en/about/error_correction.html) of the QR code through *-q* (0 - Level L, .., 3 - Level H), and configure the [version](https://www.qrcode.com/en/about/version.html) through *-w* (0 - 40).

Example result:

<pre style="font-family: monospace; line-height: 1em; letter-spacing: 0px;">
 ▄▄▄▄▄ █▀▀▄ ▄ ▀▀▄ ▄▀▄▀█ ▀█ ▀█  ▄▀█ ▄▄▄▄▄
 █   █ █▀ ▀▀ ▄▀▀ ▄▄▄ ▄▄▄▄ █▄▀▀   █ █   █
 █▄▄▄█ █▀▄▄█ ▀▀ ▀  █▀█▄    ▀███ ██ █▄▄▄█
▄▄▄▄▄▄▄█▄▀ █▄▀▄▀▄▀ █▄▀▄▀ █ █▄█▄█ █▄▄▄▄▄▄▄
▄   ▄▀▄ ▄█▄▄▀▄▄▄ ██▀█ ▀█ ▄ ▄ ▄▄▀▀▄▀ ▀▄█▄▀
▀ ▀██ ▄▀▀▀▀█▄█▀ ██▄▀▄▄▄█▀ ▀ ▄█▀█    █▀█ ▄
▀█  ▀▄▄▄▀██▄▄▀▀▀▀█▄█▀▀▀▀▀▄▀█▄▄▄▀▄▄█▄██ ▄▀
█▀▄█▀█▄▄ ▄  ▄▄██▀█▀ ▀███ ▄▀▄▄█▀█▄▀█▄ ▄█▀█
██▄  █▄▀▄█  ▀▀▀█▀▀  █ ▀  ▄▄▄▀▄▄ ▀▀▀█▀▄▄█▀
 ▄▀▀ █▄▀▀▀█   ▄ ▀ ▀ ██ ██▀▄▄▀ ▀▀▀▄▀ █ █
▄ ▄█▀█▄█▀█▄▄▀▀▄▀▄▀▀▄█▀▀ ▀▄▀▀▄█▄ ▄▄▀▄█▄▄█
█▀▄  ▄▄ █ ▄█▀▀▀ ▄▀▀██ ▀▀▀█ ▀██   █ █▀▄▀█▀
▄▀▄▄  ▄   ▀▀▀█▀▄▀▄▀▀█▀▀▀▀ ▄██ ▀▀▀█▀█▀ ▄█▄
▄█▄ █▄▄▀▀▄▄▄▀█▀█▀█▄█▄▀██▄▄█▄█ ▀▀  ███ █▄█
 █▀▄▀█▄▀██ ▄▄█▀▀▀▀▄██▄▀█▀▄▀ ▄   ▄▄ ▄▀ ▄
 ██▄ ▀▄█▄▀▄ ▄▄▀█▀ █▄▄▄  ▀▄▀▀███ ▄▀█████▀█
▄██▄▄▄▄█▀ █ ▀ ▀██▀█ ▀█▀ █  ▄  ▀█ ▄▄▄  ▄█▀
 ▄▄▄▄▄ █▄██▄▀ ▄█  ▀▀▀█▄██▀▀ ▀▀▄▄ █▄█ ▀█
 █   █ █ ▀█▄▀█▄ ▄▀▀▄▄▄▀█▀█▀▄█▄ █▄▄▄ ▄▀▄ ▀
 █▄▄▄█ █ ▄██▀▀▄ ▄█▄█▄█▀ ▄▄█ ▀█▄▀▀█▄█ █▀ █
▄▄▄▄▄▄▄█▄█▄██▄█▄██▄▄███▄▄▄▄▄▄▄███▄▄██▄▄██
</pre>

### Keys

#### Generate a random OTP key

```bash
JustOnceCMD -g -s "Keep a fire burning in your eye" -P

```
Instructs JustOnceCMD to generate a key, based on the seed "Keep a fire burning in your eye" and print it to stdout. Seeds should be at least 10 bytes long. If you omit the seed, JustOnceCMD will generate a key based on a randomly generated seed.

Example result:

```bash
GRBDMNJWGU3TAMRQGYYTEMBWGY3DSNZS
```

### Timing info

```bash
JustOnceCMD -u -V
```

Produces the current unix time, period and intra-period progress (time in seconds passed since period began). If you omit the verbose flag (*-V*), only the current unix time is print.

Example result:

```bash
1629128023 54304267 13
```