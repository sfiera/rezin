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

using sfz::hex;
using sfz::range;
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

void read_from(pn::file_view in, PackBitsRectOp* op) {
    read_from(in, &op->pix_map);
    read_from(in, &op->clut);
    read_from(in, &op->src_rect);
    read_from(in, &op->dst_rect);
    in.read(&op->mode).check();
    if (op->mode != 0) {
        throw std::runtime_error("only source compositing is supported");
    }
    op->image = op->pix_map.read_packed_image(in, op->clut);
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

void read_from(pn::file_view in, DirectBitsRectOp* op) {
    read_from(in, &op->pix_map);
    read_from(in, &op->src_rect);
    read_from(in, &op->dst_rect);
    in.read(&op->mode).check();
    op->mode &= ~0x0040;  // Ignore dithering.
    if (op->mode != 0) {
        throw std::runtime_error("only source compositing is supported");
    }
    op->image = op->pix_map.read_direct_image(in);
}

struct Header {
    Rect bounds;
};

enum {
    HEADER_VERSION_2          = 0xffff,
    HEADER_VERSION_2_EXTENDED = 0xfffe,
};

void read_from(pn::file_view in, Header* header) {
    uint16_t version;
    in.read(&version).check();
    if (version == HEADER_VERSION_2) {
        uint32_t left, top, right, bottom;
        in.read(pn::pad(2), &left, &top, &right, &bottom, pn::pad(4)).check();
        header->bounds.left   = left / 65536;
        header->bounds.top    = top / 65536;
        header->bounds.right  = right / 65536;
        header->bounds.bottom = bottom / 65536;
    } else if (version == HEADER_VERSION_2_EXTENDED) {
        uint32_t hdpi, vdpi;
        in.read(pn::pad(2), &hdpi, &vdpi).check();
        read_from(in, &header->bounds);
        in.read(pn::pad(2)).check();
        if (hdpi != 0x00480000) {
            throw std::runtime_error("horizontal resolution != 72 dpi");
        }
        if (vdpi != 0x00480000) {
            throw std::runtime_error("vertical resolution != 72 dpi");
        }
    } else {
        throw std::runtime_error("only version 2 'PICT' resources are supported");
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

void read_version_2_pict(pn::file_view in, Picture& pict) {
    uint16_t header_version;
    in.read(&header_version).check();
    if (header_version != HEADER_OP_V2) {
        throw std::runtime_error("expected header of version 2 'PICT' resource");
    }
    Header header = {pict.rep->bounds};
    read_from(in, &header);
    if (pict.rep->bounds != header.bounds) {
        throw std::runtime_error("PICT resource must fill bounds");
    }

    while (true) {
        uint16_t op;
        in.read(&op).check();
        switch (op) {
            case NOOP_V2:
            case DEFAULT_HILITE_V2: {
                break;
            }

            case PEN_SIZE_V2: {
                in.read(pn::pad(4)).check();
                break;
            }

            case FOREGROUND_COLOR_V2:
            case BACKGROUND_COLOR_V2:
            case OP_COLOR_V2: {
                in.read(pn::pad(6)).check();
                break;
            }

            case SHORT_LINE_V2: {
                pict.rep->is_raster = false;
                in.read(pn::pad(6)).check();
                break;
            }

            case FRAME_RECT_V2:
            case PAINT_RECT_V2:
            case FRAME_OVAL_V2:
            case PAINT_OVAL_V2: {
                pict.rep->is_raster = false;
                in.read(pn::pad(8)).check();
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
                in.read(pn::pad(12)).check();
                break;
            }

            case FRAME_SAME_ARC_V2:
            case PAINT_SAME_ARC_V2: {
                pict.rep->is_raster = false;
                in.read(pn::pad(4)).check();
                break;
            }

            case FRAME_POLY_V2:
            case PAINT_POLY_V2: {
                pict.rep->is_raster = false;
                uint16_t poly_size;
                in.read(&poly_size).check();
                in.read(pn::pad(round_up_even(poly_size - 2))).check();
                break;
            }

            case CLIP_V2: {
                uint16_t clip_type;
                in.read(&clip_type).check();
                if (clip_type != 0x000a) {
                    throw std::runtime_error("only rectangular clip regions are supported");
                }
                Rect r;
                read_from(in, &r);
                if (r != pict.rep->bounds) {
                    throw std::runtime_error("PICT clip must fill bounds");
                }
                break;
            }

            case PACK_BITS_RECT_V2: {
                PackBitsRectOp op;
                read_from(in, &op);
                op.draw(*pict.rep);
                break;
            }

            case DIRECT_BITS_RECT_V2: {
                DirectBitsRectOp op;
                read_from(in, &op);
                op.draw(*pict.rep);
                break;
            }

            case SHORT_COMMENT_V2: {
                in.read(pn::pad(2)).check();
                break;
            }

            case LONG_COMMENT_V2: {
                in.read(pn::pad(2)).check();
                uint16_t comment_size;
                in.read(&comment_size).check();
                in.read(pn::pad(round_up_even(comment_size))).check();
                break;
            }

            case END_V2: {
                goto end_pict_v2;
            }

            default: {
                throw std::runtime_error(
                        pn::format("unsupported op ${0} in 'PICT' resource", hex(op, 4)).c_str());
            }
        }
    }
end_pict_v2:;
}

enum {
    NOOP_V1        = 0x00,
    PIC_VERSION_V1 = 0x11,
};

void read_version_1_pict(pn::file_view in, Picture& pict) {
    while (true) {
        uint8_t op;
        if (in.read(&op).eof()) {
            break;
        }
        in.check();
        switch (op) {
            case NOOP_V1: {
                break;
            }

            case PIC_VERSION_V1: {
                uint8_t version;
                in.read(&version).check();
                if (version == 0x01) {
                    pict.rep->version = 1;
                    return;
                } else if (version == 0x02) {
                    pict.rep->version = 2;
                    in.read(&op).check();
                    if (op != 0xff) {
                        throw std::runtime_error("expected end of version 1 'PICT' resource");
                    }
                    read_version_2_pict(in, pict);
                } else {
                    throw std::runtime_error(
                            "only version 1 and 2 'PICT' resources are supported");
                }
                break;
            }

            default: {
                throw std::runtime_error(
                        pn::format("unsupported op ${0} in 'PICT' resource", hex(op, 2)).c_str());
            }
        }
    }
}

}  // namespace

Picture::Picture(pn::data_view in) : rep(new Rep) {
    pn::file f     = in.open();
    rep->version   = 0;
    rep->is_raster = true;
    f.read(pn::pad(2)).check();
    read_from(f, &rep->bounds);
    rep->image.reset(new RasterImage(rep->bounds));

    read_version_1_pict(f, *this);
}

bool Picture::is_raster() const { return rep->is_raster; }

uint8_t Picture::version() const { return rep->version; }

Picture::~Picture() {}

pn::data png(const Picture& pict) {
    if (pict.version() != 2) {
        throw std::runtime_error("can only create png of version 2 'PICT' resource");
    }
    if (!pict.is_raster()) {
        throw std::runtime_error("cannot create png of vector 'PICT' resource");
    }
    const Picture::Rep& rep = *pict.rep;
    return png(*rep.image);
}

}  // namespace rezin
