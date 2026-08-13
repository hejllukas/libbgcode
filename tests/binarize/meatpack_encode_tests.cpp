#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "binarize_test_utils.hpp"

#include <string>

using namespace bgcode::core;
using namespace bgcode::binarize;
using namespace bgcode_test;

namespace {

// Round-trip raw_data through GCodeBlock write and read, returning what was read
// back. The write must succeed.
std::string roundtrip_gcode_block(EGCodeEncodingType encoding, const std::string& raw_data) {
    GCodeBlock read_block;
    REQUIRE(roundtrip_block(make_gcode_block(encoding, raw_data), ECompressionType::None, read_block) == EResult::Success);
    return read_block.raw_data;
}

} // namespace

// A literal 0x00 must travel through MeatPack's verbatim-escape path. The
// payloads use an explicit length so the embedded NUL is not truncated, and end
// in '\n' so the codec's trailing-newline normalisation does not perturb the
// comparison. They place 0x00 in both escape-nibble positions: next to another
// unpackable byte, and immediately after a packable one.
TEST_CASE("MeatPack encode carries a literal 0x00 byte", "[Binarize][MeatPack]") {
    const EGCodeEncodingType encoding = GENERATE(EGCodeEncodingType::MeatPack, EGCodeEncodingType::MeatPackComments);
    CAPTURE(encoding);

    const std::string beside_unpackable("z\x00z\n", 4);
    const std::string after_packable("0\x00z\n", 4);

    REQUIRE(roundtrip_gcode_block(encoding, beside_unpackable) == beside_unpackable);
    REQUIRE(roundtrip_gcode_block(encoding, after_packable) == after_packable);
}
