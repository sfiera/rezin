// Copyright (c) 2011 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include <rezin/pict.hpp>

#include <stdint.h>
#include <rezin/clut.hpp>
#include <rezin/image.hpp>
#include <rezin/primitives.hpp>
#include <vector>

using sfz::BytesSlice;
using sfz::Exception;
using sfz::ReadSource;
using sfz::WriteTarget;
using sfz::format;
using sfz::hex;
using sfz::range;
using sfz::read;
using std::unique_ptr;
using std::vector;

namespace rezin {

struct Picture::Rep {
    Rect                    bounds;
    bool                    is_raster;
    uint8_t                 version;
    unique_ptr<RasterImage> image;
};

namespace {

template <typename T>
T round_up_even(T t) {
    if ((t % 2) == 1) {
        return t + 1;
    }
    return t;
}

struct PackBitsRectOp {
    PixMap                  pix_map;
    ColorTable              clut;
    Rect                    src_rect;
    Rect                    dst_rect;
    int16_t                 mode;
    unique_ptr<RasterImage> image;

    void draw(Picture::Rep& rep) {
        RectImage       mask(dst_rect, AlphaColor(0, 0, 0));
        TranslatedImage src(*image, dst_rect.left - src_rect.left, dst_rect.top - src_rect.top);
        rep.image->src(src, mask);
    }
};

void read_from(ReadSource in, PackBitsRectOp& op) {
    read(in, op.pix_map);
    read(in, op.clut);
    read(in, op.src_rect);
    read(in, op.dst_rect);
    read(in, op.mode);
    if (op.mode != 0) {
        throw Exception("only source compositing is supported");
    }
    op.pix_map.read_packed_image(in, op.clut, op.image);
}

struct DirectBitsRectOp {
    AddressedPixMap         pix_map;
    Rect                    src_rect;
    Rect                    dst_rect;
    int16_t                 mode;
    unique_ptr<RasterImage> image;

    void draw(Picture::Rep& rep) {
        RectImage       mask(dst_rect, AlphaColor(0, 0, 0));
        TranslatedImage src(*image, dst_rect.left - src_rect.left, dst_rect.top - src_rect.top);
        rep.image->src(src, mask);
    }
};

void read_from(ReadSource in, DirectBitsRectOp& op) {
    read(in, op.pix_map);
    read(in, op.src_rect);
    read(in, op.dst_rect);
    read(in, op.mode);
    op.mode &= ~0x0040;  // Ignore dithering.
    if (op.mode != 0) {
        sfz::print(sfz::io::err, sfz::format("{0}\n", op.mode));
        throw Exception("only source compositing is supported");
    }
    op.pix_map.read_direct_image(in, op.image);
}

struct Header {
    Rect bounds;
};

enum {
    HEADER_VERSION_2          = 0xffff,
    HEADER_VERSION_2_EXTENDED = 0xfffe,
};

void read_from(ReadSource in, Header& header) {
    uint16_t version = read<uint16_t>(in);
    if (version == HEADER_VERSION_2) {
        in.shift(2);  // reserved
        header.bounds.left   = read<uint32_t>(in) / 65536;
        header.bounds.top    = read<uint32_t>(in) / 65536;
        header.bounds.right  = read<uint32_t>(in) / 65536;
        header.bounds.bottom = read<uint32_t>(in) / 65536;
        in.shift(4);  // reserved
    } else if (version == HEADER_VERSION_2_EXTENDED) {
        in.shift(2);  // reserved
        if (read<uint32_t>(in) != 0x00480000) {
            throw Exception("horizontal resolution != 72 dpi");
        }
        if (read<uint32_t>(in) != 0x00480000) {
            throw Exception("vertical resolution != 72 dpi");
        }
        read(in, header.bounds);
        in.shift(2);  // reserved
    } else {
        throw Exception("only version 2 'PICT' resources are supported");
    }
}

enum {
    NOOP_V2             = 0x0000,
    CLIP_V2             = 0x0001,
    PEN_SIZE_V2         = 0x0007,
    FOREGROUND_COLOR_V2 = 0x001a,
    BACKGROUND_COLOR_V2 = 0x001b,
    DEFAULT_HILITE_V2   = 0x001e,
    OP_COLOR_V2         = 0x001f,
    SHORT_LINE_V2       = 0x0022,
    FRAME_RECT_V2       = 0x0030,
    PAINT_RECT_V2       = 0x0031,
    FRAME_SAME_RECT_V2  = 0x0038,
    PAINT_SAME_RECT_V2  = 0x0039,
    FRAME_OVAL_V2       = 0x0050,
    PAINT_OVAL_V2       = 0x0051,
    FRAME_SAME_OVAL_V2  = 0x0058,
    PAINT_SAME_OVAL_V2  = 0x0059,
    FRAME_ARC_V2        = 0x0060,
    PAINT_ARC_V2        = 0x0061,
    FRAME_SAME_ARC_V2   = 0x0068,
    PAINT_SAME_ARC_V2   = 0x0069,
    FRAME_POLY_V2       = 0x0070,
    PAINT_POLY_V2       = 0x0071,
    PACK_BITS_RECT_V2   = 0x0098,
    DIRECT_BITS_RECT_V2 = 0x009a,
    SHORT_COMMENT_V2    = 0x00a0,
    LONG_COMMENT_V2     = 0x00a1,
    HEADER_OP_V2        = 0x0c00,
    END_V2              = 0x00ff,
};

void read_version_2_pict(ReadSource in, Picture& pict) {
    if (read<uint16_t>(in) != HEADER_OP_V2) {
        throw Exception("expected header of version 2 'PICT' resource");
    }
    Header header = {pict.rep->bounds};
    read(in, header);
    if (pict.rep->bounds != header.bounds) {
        throw Exception("PICT resource must fill bounds");
    }

    while (true) {
        const uint16_t op = read<uint16_t>(in);
        switch (op) {
            case NOOP_V2:
            case DEFAULT_HILITE_V2: {
                break;
            }

            case PEN_SIZE_V2: {
                in.shift(4);
                break;
            }

            case FOREGROUND_COLOR_V2:
            case BACKGROUND_COLOR_V2:
            case OP_COLOR_V2: {
                in.shift(6);
                break;
            }

            case SHORT_LINE_V2: {
                pict.rep->is_raster = false;
                in.shift(6);
                break;
            }

            case FRAME_RECT_V2:
            case PAINT_RECT_V2:
            case FRAME_OVAL_V2:
            case PAINT_OVAL_V2: {
                pict.rep->is_raster = false;
                in.shift(8);
                break;
            }

            case FRAME_SAME_RECT_V2:
            case PAINT_SAME_RECT_V2:
            case FRAME_SAME_OVAL_V2:
            case PAINT_SAME_OVAL_V2: {
                pict.rep->is_raster = false;
                break;
            }

            case FRAME_ARC_V2:
            case PAINT_ARC_V2: {
                pict.rep->is_raster = false;
                in.shift(12);
                break;
            }

            case FRAME_SAME_ARC_V2:
            case PAINT_SAME_ARC_V2: {
                pict.rep->is_raster = false;
                in.shift(4);
                break;
            }

            case FRAME_POLY_V2:
            case PAINT_POLY_V2: {
                pict.rep->is_raster = false;
                in.shift(round_up_even(read<uint16_t>(in) - 2));
                break;
            }

            case CLIP_V2: {
                if (read<uint16_t>(in) != 0x000a) {
                    throw Exception("only rectangular clip regions are supported");
                }
                if (read<Rect>(in) != pict.rep->bounds) {
                    throw Exception("PICT clip must fill bounds");
                }
                break;
            }

            case PACK_BITS_RECT_V2: {
                PackBitsRectOp op;
                read(in, op);
                op.draw(*pict.rep);
                break;
            }

            case DIRECT_BITS_RECT_V2: {
                DirectBitsRectOp op;
                read(in, op);
                op.draw(*pict.rep);
                break;
            }

            case SHORT_COMMENT_V2: {
                in.shift(2);
                break;
            }

            case LONG_COMMENT_V2: {
                in.shift(2);
                in.shift(round_up_even(read<uint16_t>(in)));
                break;
            }

            case END_V2: {
                goto end_pict_v2;
            }

            default: {
                throw Exception(format("unsupported op ${0} in 'PICT' resource", hex(op, 4)));
            }
        }
    }
end_pict_v2:;
}

enum {
    NOOP_V1        = 0x00,
    PIC_VERSION_V1 = 0x11,
};

void read_version_1_pict(ReadSource in, Picture& pict) {
    while (!in.empty()) {
        const uint8_t op = read<uint8_t>(in);
        switch (op) {
            case NOOP_V1: {
                break;
            }

            case PIC_VERSION_V1: {
                const uint8_t version = read<uint8_t>(in);
                if (version == 0x01) {
                    pict.rep->version = 1;
                    return;
                } else if (version == 0x02) {
                    pict.rep->version = 2;
                    if (read<uint8_t>(in) != 0xff) {
                        throw Exception("expected end of version 1 'PICT' resource");
                    }
                    read_version_2_pict(in, pict);
                } else {
                    throw Exception("only version 1 and 2 'PICT' resources are supported");
                }
                break;
            }

            default: {
                throw Exception(format("unsupported op ${0} in 'PICT' resource", hex(op, 2)));
            }
        }
    }
}

}  // namespace

Picture::Picture(BytesSlice in) : rep(new Rep) {
    rep->version   = 0;
    rep->is_raster = true;
    in.shift(2);  // Ignore size of 'PICT' resource.
    read(in, rep->bounds);
    rep->image.reset(new RasterImage(rep->bounds));

    read_version_1_pict(in, *this);
}

bool Picture::is_raster() const { return rep->is_raster; }

uint8_t Picture::version() const { return rep->version; }

Picture::~Picture() {}

PngPicture png(const Picture& pict) {
    if (pict.version() != 2) {
        throw Exception("can only create png of version 2 'PICT' resource");
    }
    if (!pict.is_raster()) {
        throw Exception("cannot create png of vector 'PICT' resource");
    }
    PngPicture result = {pict};
    return result;
}

void write_to(WriteTarget out, PngPicture png_pict) {
    const Picture::Rep& rep = *png_pict.pict.rep;
    write(out, png(*rep.image));
}

}  // namespace rezin
