#pragma once

#ifndef QRCOMMON_H
#define QRCOMMON_H

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * Encoding mode.
 */
typedef enum {
	QR_MODE_NUL = -1,   ///< Terminator (NUL character). Internal use only
	QR_MODE_NUM = 0,    ///< Numeric mode
	QR_MODE_AN,         ///< Alphabet-numeric mode
	QR_MODE_8,          ///< 8-bit data mode
	QR_MODE_KANJI,      ///< Kanji (shift-jis) mode
	QR_MODE_STRUCTURE,  ///< Internal use only
	QR_MODE_ECI,        ///< ECI mode
	QR_MODE_FNC1FIRST,  ///< FNC1, first position
	QR_MODE_FNC1SECOND, ///< FNC1, second position
} QRencodeMode;

/**
 * Level of error correction.
 */
typedef enum {
	QR_ECLEVEL_L = 0, ///< lowest
	QR_ECLEVEL_M,
	QR_ECLEVEL_Q,
	QR_ECLEVEL_H      ///< highest
} QRecLevel;

/**
 * Maximum version (size) of QR-code symbol.
 */
#define QRSPEC_VERSION_MAX 40

/**
 * Maximum version (size) of QR-code symbol.
 */
#define MQRSPEC_VERSION_MAX 4

#if defined(__cplusplus)
}
#endif

#endif /* QRCOMMON_H */
