// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include "rezin/MacRoman.hpp"

#include "sfz/Bytes.hpp"
#include "sfz/Encoding.hpp"
#include "sfz/Foreach.hpp"
#include "sfz/Macros.hpp"
#include "sfz/Range.hpp"
#include "sfz/String.hpp"

using sfz::Bytes;
using sfz::BytesPiece;
using sfz::Encoding;
using sfz::String;
using sfz::StringPiece;
using sfz::ascii_encoding;
using sfz::kAsciiUnknownCodePoint;
using sfz::range;

namespace rezin {

namespace {

class MacRomanEncoding : public Encoding {
  public:
    MacRomanEncoding() { }

    virtual StringPiece name() const {
        static const String name("MacRoman", ascii_encoding());
        return name;
    }

    virtual bool can_decode(const BytesPiece& bytes) const {
        return true;
    }

    virtual void decode(const BytesPiece& in, String* out) const {
        foreach (it, in) {
            uint8_t byte = *it;
            if (byte < 0x80) {
                out->append(1, byte);
            } else {
                out->append(1, kMap[byte & 0x7f]);
            }
        }
    }

    virtual bool can_encode(const StringPiece& string) const {
        foreach (it, string) {
            if (*it < 0x80) {
                continue;
            }
            foreach (i, range(0x80)) {
                if (kMap[i] == *it) {
                    return false;
                }
            }
        }
        return true;
    }

    virtual void encode(const StringPiece& in, Bytes* out) const {
        foreach (it, in) {
            if (*it < 0x80) {
                out->append(1, *it);
                continue;
            }
            foreach (i, range(0x80)) {
                if (kMap[i] == *it) {
                    if (kMap[i] == *it) {
                        out->append(1, 0x80 | i);
                        continue;
                    }
                }
            }
            out->append(1, kAsciiUnknownCodePoint);
        }
    }

  private:
    static uint16_t kMap[0x80];

    DISALLOW_COPY_AND_ASSIGN(MacRomanEncoding);
};

uint16_t MacRomanEncoding::kMap[0x80] = {
    0x00C4,  // LATIN CAPITAL LETTER A WITH DIAERESIS
    0x00C5,  // LATIN CAPITAL LETTER A WITH RING ABOVE
    0x00C7,  // LATIN CAPITAL LETTER C WITH CEDILLA
    0x00C9,  // LATIN CAPITAL LETTER E WITH ACUTE
    0x00D1,  // LATIN CAPITAL LETTER N WITH TILDE
    0x00D6,  // LATIN CAPITAL LETTER O WITH DIAERESIS
    0x00DC,  // LATIN CAPITAL LETTER U WITH DIAERESIS
    0x00E1,  // LATIN SMALL LETTER A WITH ACUTE
    0x00E0,  // LATIN SMALL LETTER A WITH GRAVE
    0x00E2,  // LATIN SMALL LETTER A WITH CIRCUMFLEX
    0x00E4,  // LATIN SMALL LETTER A WITH DIAERESIS
    0x00E3,  // LATIN SMALL LETTER A WITH TILDE
    0x00E5,  // LATIN SMALL LETTER A WITH RING ABOVE
    0x00E7,  // LATIN SMALL LETTER C WITH CEDILLA
    0x00E9,  // LATIN SMALL LETTER E WITH ACUTE
    0x00E8,  // LATIN SMALL LETTER E WITH GRAVE
    0x00EA,  // LATIN SMALL LETTER E WITH CIRCUMFLEX
    0x00EB,  // LATIN SMALL LETTER E WITH DIAERESIS
    0x00ED,  // LATIN SMALL LETTER I WITH ACUTE
    0x00EC,  // LATIN SMALL LETTER I WITH GRAVE
    0x00EE,  // LATIN SMALL LETTER I WITH CIRCUMFLEX
    0x00EF,  // LATIN SMALL LETTER I WITH DIAERESIS
    0x00F1,  // LATIN SMALL LETTER N WITH TILDE
    0x00F3,  // LATIN SMALL LETTER O WITH ACUTE
    0x00F2,  // LATIN SMALL LETTER O WITH GRAVE
    0x00F4,  // LATIN SMALL LETTER O WITH CIRCUMFLEX
    0x00F6,  // LATIN SMALL LETTER O WITH DIAERESIS
    0x00F5,  // LATIN SMALL LETTER O WITH TILDE
    0x00FA,  // LATIN SMALL LETTER U WITH ACUTE
    0x00F9,  // LATIN SMALL LETTER U WITH GRAVE
    0x00FB,  // LATIN SMALL LETTER U WITH CIRCUMFLEX
    0x00FC,  // LATIN SMALL LETTER U WITH DIAERESIS
    0x2020,  // DAGGER
    0x00B0,  // DEGREE SIGN
    0x00A2,  // CENT SIGN
    0x00A3,  // POUND SIGN
    0x00A7,  // SECTION SIGN
    0x2022,  // BULLET
    0x00B6,  // PILCROW SIGN
    0x00DF,  // LATIN SMALL LETTER SHARP S
    0x00AE,  // REGISTERED SIGN
    0x00A9,  // COPYRIGHT SIGN
    0x2122,  // TRADE MARK SIGN
    0x00B4,  // ACUTE ACCENT
    0x00A8,  // DIAERESIS
    0x2260,  // NOT EQUAL TO
    0x00C6,  // LATIN CAPITAL LETTER AE
    0x00D8,  // LATIN CAPITAL LETTER O WITH STROKE
    0x221E,  // INFINITY
    0x00B1,  // PLUS-MINUS SIGN
    0x2264,  // LESS-THAN OR EQUAL TO
    0x2265,  // GREATER-THAN OR EQUAL TO
    0x00A5,  // YEN SIGN
    0x00B5,  // MICRO SIGN
    0x2202,  // PARTIAL DIFFERENTIAL
    0x2211,  // N-ARY SUMMATION
    0x220F,  // N-ARY PRODUCT
    0x03C0,  // GREEK SMALL LETTER PI
    0x222B,  // INTEGRAL
    0x00AA,  // FEMININE ORDINAL INDICATOR
    0x00BA,  // MASCULINE ORDINAL INDICATOR
    0x03A9,  // GREEK CAPITAL LETTER OMEGA
    0x00E6,  // LATIN SMALL LETTER AE
    0x00F8,  // LATIN SMALL LETTER O WITH STROKE
    0x00BF,  // INVERTED QUESTION MARK
    0x00A1,  // INVERTED EXCLAMATION MARK
    0x00AC,  // NOT SIGN
    0x221A,  // SQUARE ROOT
    0x0192,  // LATIN SMALL LETTER F WITH HOOK
    0x2248,  // ALMOST EQUAL TO
    0x2206,  // INCREMENT
    0x00AB,  // LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
    0x00BB,  // RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
    0x2026,  // HORIZONTAL ELLIPSIS
    0x00A0,  // NO-BREAK SPACE
    0x00C0,  // LATIN CAPITAL LETTER A WITH GRAVE
    0x00C3,  // LATIN CAPITAL LETTER A WITH TILDE
    0x00D5,  // LATIN CAPITAL LETTER O WITH TILDE
    0x0152,  // LATIN CAPITAL LIGATURE OE
    0x0153,  // LATIN SMALL LIGATURE OE
    0x2013,  // EN DASH
    0x2014,  // EM DASH
    0x201C,  // LEFT DOUBLE QUOTATION MARK
    0x201D,  // RIGHT DOUBLE QUOTATION MARK
    0x2018,  // LEFT SINGLE QUOTATION MARK
    0x2019,  // RIGHT SINGLE QUOTATION MARK
    0x00F7,  // DIVISION SIGN
    0x25CA,  // LOZENGE
    0x00FF,  // LATIN SMALL LETTER Y WITH DIAERESIS
    0x0178,  // LATIN CAPITAL LETTER Y WITH DIAERESIS
    0x2044,  // FRACTION SLASH
    0x20AC,  // EURO SIGN
    0x2039,  // SINGLE LEFT-POINTING ANGLE QUOTATION MARK
    0x203A,  // SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
    0xFB01,  // LATIN SMALL LIGATURE FI
    0xFB02,  // LATIN SMALL LIGATURE FL
    0x2021,  // DOUBLE DAGGER
    0x00B7,  // MIDDLE DOT
    0x201A,  // SINGLE LOW-9 QUOTATION MARK
    0x201E,  // DOUBLE LOW-9 QUOTATION MARK
    0x2030,  // PER MILLE SIGN
    0x00C2,  // LATIN CAPITAL LETTER A WITH CIRCUMFLEX
    0x00CA,  // LATIN CAPITAL LETTER E WITH CIRCUMFLEX
    0x00C1,  // LATIN CAPITAL LETTER A WITH ACUTE
    0x00CB,  // LATIN CAPITAL LETTER E WITH DIAERESIS
    0x00C8,  // LATIN CAPITAL LETTER E WITH GRAVE
    0x00CD,  // LATIN CAPITAL LETTER I WITH ACUTE
    0x00CE,  // LATIN CAPITAL LETTER I WITH CIRCUMFLEX
    0x00CF,  // LATIN CAPITAL LETTER I WITH DIAERESIS
    0x00CC,  // LATIN CAPITAL LETTER I WITH GRAVE
    0x00D3,  // LATIN CAPITAL LETTER O WITH ACUTE
    0x00D4,  // LATIN CAPITAL LETTER O WITH CIRCUMFLEX
    0xF8FF,  // Apple logo
    0x00D2,  // LATIN CAPITAL LETTER O WITH GRAVE
    0x00DA,  // LATIN CAPITAL LETTER U WITH ACUTE
    0x00DB,  // LATIN CAPITAL LETTER U WITH CIRCUMFLEX
    0x00D9,  // LATIN CAPITAL LETTER U WITH GRAVE
    0x0131,  // LATIN SMALL LETTER DOTLESS I
    0x02C6,  // MODIFIER LETTER CIRCUMFLEX ACCENT
    0x02DC,  // SMALL TILDE
    0x00AF,  // MACRON
    0x02D8,  // BREVE
    0x02D9,  // DOT ABOVE
    0x02DA,  // RING ABOVE
    0x00B8,  // CEDILLA
    0x02DD,  // DOUBLE ACUTE ACCENT
    0x02DB,  // OGONEK
    0x02C7,  // CARON
};

}  // namespace

const Encoding& mac_roman_encoding() {
    static const MacRomanEncoding instance;
    return instance;
}

void cr2nl(String* string) {
    String result;
    foreach (it, StringPiece(*string)) {
        uint32_t code = *it;
        if (code == '\r') {
            result.append(1, '\n');
        } else {
            result.append(1, code);
        }
    }
    result.swap(string);
}

}  // namespace rezin
