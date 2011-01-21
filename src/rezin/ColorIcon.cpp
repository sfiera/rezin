// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librezin, a free software project.  You can redistribute it and/or modify
// it under the terms of the MIT License.

#include <rezin/ColorIcon.hpp>

#include <algorithm>
#include <map>
#include <vector>
#include <png.h>
#include <rezin/BasicTypes.hpp>
#include <rezin/BitsSlice.hpp>
#include <rezin/ColorTableInternal.hpp>
#include <rgos/rgos.hpp>
#include <sfz/sfz.hpp>

using rgos::Json;
using rgos::JsonDefaultVisitor;
using rgos::StringMap;
using sfz::Bytes;
using sfz::BytesSlice;
using sfz::Exception;
using sfz::ReadSource;
using sfz::StringSlice;
using sfz::WriteTarget;
using sfz::format;
using sfz::range;
using sfz::read;
using std::make_pair;
using std::map;
using std::vector;
using std::swap;

namespace rezin {

namespace {

struct ColorIcon {
    PixMap icon_pixmap;
    BitMap mask_bitmap;
    BitMap icon_bitmap;
    uint32_t icon_data;
    std::vector<uint8_t> mask_bitmap_pixels;
    std::vector<uint8_t> icon_bitmap_pixels;
    ColorTable color_table;
    std::vector<uint8_t> icon_pixmap_pixels;

    rgos::Json to_json() const;
};

Json ColorIcon::to_json() const {
    StringMap<Json> result;
    result["color-table"] = color_table.to_json();

    const int width = mask_bitmap.bounds.width();
    const int height = mask_bitmap.bounds.height();

    vector<uint8_t>::const_iterator mask_it = mask_bitmap_pixels.begin();
    vector<uint8_t>::const_iterator icon_it = icon_pixmap_pixels.begin();

    vector<Json> pixels;
    foreach (int i, range(height)) {
        vector<Json> row;
        foreach (int j, range(width)) {
            if (*mask_it) {
                row.push_back(Json::number(*icon_it));
            } else {
                row.push_back(Json());
            }
            ++mask_it;
            ++icon_it;
        }
        pixels.push_back(Json::array(row));
    }
    result["pixels"] = Json::array(pixels);
    return Json::object(result);
}

void read_from(ReadSource in, ColorIcon* cicn) {
    read(in, &cicn->icon_pixmap);
    read(in, &cicn->mask_bitmap);
    read(in, &cicn->icon_bitmap);
    read(in, &cicn->icon_data);

    cicn->mask_bitmap.read_pixels(in, &cicn->mask_bitmap_pixels);
    cicn->icon_bitmap.read_pixels(in, &cicn->icon_bitmap_pixels);
    read(in, &cicn->color_table);
    cicn->icon_pixmap.read_pixels(in, &cicn->icon_pixmap_pixels);
}

}  // namespace

rgos::Json read_cicn(const BytesSlice& in) {
    BytesSlice remainder(in);
    ColorIcon cicn;
    read(&remainder, &cicn);
    if (!remainder.empty()) {
        throw Exception("extra bytes at end of 'cicn' resource.");
    }
    return cicn.to_json();
}

namespace {

void png_error(png_struct* png, png_const_charp error_msg) {
    throw Exception(format("png error: {0}", error_msg));
}

void png_write_data(png_struct* png, png_byte* data, png_size_t length) {
    WriteTarget* out = reinterpret_cast<WriteTarget*>(png_get_io_ptr(png));
    write(*out, data, length);
}

void png_flush_data(png_struct* png) {
    static_cast<void>(png);
}

// A JsonVisitor which transforms JSON image data into a PNG object.  This object has a state
// machine, visiting different elements of the object in different states in order to construct the
// final data.
class JsonToPngVisitor : public JsonDefaultVisitor {
  public:
    JsonToPngVisitor(WriteTarget out)
            : _state(ROOT),
              _out(out),
              _png(NULL),
              _info(NULL) {
        _png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, png_error, png_error);
        if (!_png) {
            throw Exception("couldn't create png_struct");
        }

        _info = png_create_info_struct(_png);
        if (!_info) {
            png_destroy_write_struct(&_png, NULL);
            throw Exception("couldn't create png_info");
        }

        png_set_write_fn(_png, &_out, png_write_data, png_flush_data);
    }

    virtual ~JsonToPngVisitor() {
        png_destroy_write_struct(&_png, &_info);
    }

    virtual void visit_object(const StringMap<Json>& value) {
        switch (_state) {
          case ROOT:
            descend_field(COLOR_TABLE, value, "root object", "color-table");
            descend_field(GET_HEIGHT, value, "root object", "pixels");

            png_set_IHDR(_png, _info, _width, _height, 8, PNG_COLOR_TYPE_RGBA, NULL, NULL, NULL);
            png_set_swap_alpha(_png);
            png_write_info(_png, _info);
            descend_field(PIXELS, value, "root object", "pixels");
            png_write_end(_png, _info);

            _state = DONE;
            break;

          case COLOR:
            descend_field(ID, value, "color", "id");
            descend_field(RED, value, "color", "red");
            descend_field(GREEN, value, "color", "green");
            descend_field(BLUE, value, "color", "blue");
            if (!_color_table.insert(make_pair(_id, _color)).second) {
                throw Exception(format("Duplicate color id {0}", _id));
            }
            break;

          default:
            visit_default("object");
        }
    }

    virtual void visit_array(const vector<Json>& value) {
        switch (_state) {
          case COLOR_TABLE:
            foreach (const Json& item, value) {
                descend(COLOR, item);
            }
            break;

          case GET_HEIGHT:
            _height = value.size();
            if (_height == 0) {
                throw Exception("Image must have height > 0");
            }
            descend(GET_WIDTH, value.front());
            break;

          case GET_WIDTH:
            _width = value.size();
            if (_width == 0) {
                throw Exception("Image must have width > 0");
            }
            break;

          case PIXELS:
            foreach (const Json& item, value) {
                descend(PIXEL_ROW, item);
            }
            break;

          case PIXEL_ROW:
            if (value.size() != _width) {
                throw Exception("Image must be rectangular");
            }
            _row_data.clear();
            foreach (const Json& item, value) {
                descend(PIXEL, item);
            }
            png_write_row(_png, _row_data.data());
            break;

          default:
            visit_default("array");
        }
    }

    virtual void visit_number(double value) {
        // TODO(sfiera): check that |value| is integral.
        int64_t int_value = value;

        switch (_state) {
          case ID:
            _id = int_value;
            break;

          case RED:
            _color.red = int_value >> 8;
            break;

          case GREEN:
            _color.green = int_value >> 8;
            break;

          case BLUE:
            _color.blue = int_value >> 8;
            break;

          case PIXEL:
            {
                if (_color_table.find(int_value) == _color_table.end()) {
                    throw Exception(format("No color with id {0}", int_value));
                }
                const Color& color = _color_table.find(int_value)->second;
                append_pixel(color.red, color.green, color.blue, 255);
            }
            break;

          default:
            visit_default("number");
        }
    }

    virtual void visit_null() {
        switch (_state) {
          case PIXEL:
            append_pixel(0, 0, 0, 0);
            break;

          default:
            visit_default("null");
        }
    }

    virtual void visit_default(const char* type) {
        throw Exception(format("Unexpected {0}", type));
    }

  private:
    enum State {
        // The initial state, before anything has been visited.
        ROOT,

        // States while reading the color table.  The COLOR_TABLE state is used when reading the
        // color table array, COLOR is used when reading the color objects, and the remaining
        // states are used for reading individual fields of the color objects.
        COLOR_TABLE, COLOR, ID, RED, GREEN, BLUE,

        // States when getting the width and height of the image.  GET_HEIGHT is used when reading
        // the pixel array, and GET_WIDTH is used when reading the first row of pixels.
        GET_HEIGHT, GET_WIDTH,

        // States when reading pixel data.  PIXELS is used when reading the pixel array, PIXEL_ROW
        // is used when reading each row of pixels, and PIXEL is used when reading each pixel
        // within each row.
        PIXELS, PIXEL_ROW, PIXEL,

        // The final state, set when processing has finished.
        DONE,
    };

    // Appends a single pixel to _row_data, with the given color components.
    void append_pixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
        // I'm not sure why we specify "RGBA"-formatted pixels, but write them here as "ARGB".
        // I've tried this on both x86_64 and ppc, so it's not an endianness issue; it just seems
        // to be that PNG_COLOR_TYPE_RGBA has a misleading name.
        uint8_t rgba[4] = {alpha, red, green, blue};
        _row_data.append(rgba, 4);
    }

    // Changes state to `state`, then visits `value`.  When done visiting, resets the state back to
    // its previous value.
    void descend(State state, const Json& value) {
        swap(state, _state);
        value.accept(this);
        swap(state, _state);
    }

    // Changes state to `state`, then visits `object[field]`.  If `field` is not an element of
    // `object`, throws an exception, using `name` to describe `object` in the error description.
    void descend_field(
            State state, const StringMap<Json>& object, const StringSlice& name,
            const StringSlice& field) {
        if (object.find(field) == object.end()) {
            throw Exception(format("{0} must have {1} field", name, quote(field)));
        }
        descend(state, object.find(field)->second);
    }

    State _state;

    WriteTarget _out;
    png_struct* _png;
    png_info* _info;

    struct Color {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    };
    map<int64_t, Color> _color_table;
    int64_t _id;
    Color _color;

    size_t _width;
    size_t _height;

    Bytes _row_data;

    DISALLOW_COPY_AND_ASSIGN(JsonToPngVisitor);
};

}  // namespace

void write_png(WriteTarget out, const rgos::Json& image) {
    JsonToPngVisitor visitor(out);
    image.accept(&visitor);
}

}  // namespace rezin
