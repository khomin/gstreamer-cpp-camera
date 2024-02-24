#ifndef CODEC_TYPE_H
#define CODEC_TYPE_H

enum class CodecType { Codec3gp, Codecmp4, CodecAvc, CodecHevc, CodecVp8, CodecVp9, Unknown };

[[maybe_unused]] static CodecType CodecTypeFromString(std::string type) {
    auto type_ = CodecType::Unknown;
    if (type == "codec3gp") {
        type_ = CodecType::Codec3gp;
    } else if (type == "codecmp4") {
        type_ = CodecType::Codecmp4;
    } else if (type == "codecavc") {
        type_ = CodecType::CodecAvc;
    } else if (type == "codechevc") {
        type_ = CodecType::CodecHevc;
    } else if (type == "codecVp8") {
        type_ = CodecType::CodecVp8;
    }
    else if (type == "codecvp9") {
        type_ = CodecType::CodecVp9;
    }
    return type_;
}

#endif // CODEC_TYPE_H
