// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of rezin, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#include <rezin/commands/convert.hpp>

#include <fcntl.h>
#include <unistd.h>
#include <rezin/rezin.hpp>

namespace args = sfz::args;

namespace rezin {

ConvertCommand::ConvertCommand() = default;

bool ConvertCommand::argument(pn::string_view arg) {
    if (!_type.has_value()) {
        _type.emplace(arg.copy());
    } else if (!_id.has_value()) {
        args::integer_option(arg, &_id);
    } else {
        return false;
    }
    return true;
}

void ConvertCommand::run(const ResourceFork& rsrc, const Options& options) const {
    const ResourceEntry& entry = rsrc.at(*_type).at(*_id);
    pn::data_view        data  = entry.data();
    pn::data             converted;

    if (*_type == "snd ") {
        Sound snd(data);
        converted = aiff(snd);
    } else if (*_type == "TEXT") {
        pn::string string(options.decode(data));
        converted = string.as_data().copy();
    } else if (*_type == "STR#") {
        StringList string_list(data, options);
        pn::value  list           = value(string_list);
        pn::string decoded_string = pn::dump(list);
        converted                 = decoded_string.as_data().copy();
    } else if (*_type == "cicn") {
        ColorIcon cicn(data);
        converted = png(cicn);
    } else if (*_type == "clut") {
        ColorTable clut(data);
        pn::value  list           = value(clut);
        pn::string decoded_string = pn::dump(list);
        converted                 = decoded_string.as_data().copy();
    } else if (*_type == "PICT") {
        Picture pict(data);
        converted = png(pict);
    } else {
        pn::format(
                stderr, "warning: printing unknown resource type {0} as raw data.\n",
                pn::dump(*_type, pn::dump_short));
        converted = data.copy();
    }
    pn::file_view{stdout}.write(converted).check();
}

}  // namespace rezin
