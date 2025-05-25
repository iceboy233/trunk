#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "absl/strings/escaping.h"
#include "base/flags.h"
#include "base/logging.h"
#include "io/file-utils.h"
#include "io/native-file.h"
#include "io/stream.h"

DEFINE_FLAG(std::string, variable_name, "", "");
DEFINE_FLAG(std::string, cc_namespace, "", "");
DEFINE_FLAG(std::string, output_cc, "", "");
DEFINE_FLAG(std::string, output_h, "", "");

int main(int argc, char *argv[]) {
    base::init_logging();

    std::vector<char *> positional_args;
    base::parse_flags(argc, argv, positional_args);

    std::vector<std::string_view> files;
    if (positional_args.size() > 1) {
        files.reserve(positional_args.size() - 1);
        for (size_t i = 1; i < positional_args.size(); ++i) {
            files.push_back(positional_args[i]);
        }
        std::sort(files.begin(), files.end());
        files.erase(std::unique(files.begin(), files.end()), files.end());
    }

    io::NativeFile output_cc_file;
    std::error_code ec = io::create(output_cc_file, flags::output_cc);
    if (ec) {
        LOG(fatal) << "create failed";
        return ec.value();
    }

    io::NativeFile output_h_file;
    ec = io::create(output_h_file, flags::output_h);
    if (ec) {
        LOG(fatal) << "create failed";
        return ec.value();
    }

    io::OStream output_h(output_h_file);
    output_h << "#include \"util/embed-data.h\"\n";
    output_h << "\n";
    if (!flags::cc_namespace.empty()) {
        output_h << "namespace " << flags::cc_namespace << " {\n";
        output_h << "\n";
    }
    output_h << "extern const ::util::EmbedData " << flags::variable_name
             << ";\n";
    if (!flags::cc_namespace.empty()) {
        output_h << "\n";
        output_h << "}  // namespace " << flags::cc_namespace << "\n";
    }

    io::OStream output_cc(output_cc_file);
    output_cc << "#include <array>\n";
    output_cc << "#include <string_view>\n";
    output_cc << "\n";
    output_cc << "#include \"base/types.h\"\n";
    output_cc << "#include \"util/embed-data.h\"\n";
    output_cc << "\n";
    if (!flags::cc_namespace.empty()) {
        output_cc << "namespace " << flags::cc_namespace << " {\n";
    }
    output_cc << "namespace {\n";
    output_cc << "\n";
    output_cc << "std::array<std::string_view, " << files.size()
              << "> names{{\n";
    for (std::string_view file : files) {
        output_cc << "    \"" << absl::CHexEscape(file) << "\",\n";
    }
    output_cc << "}};\n";
    output_cc << "\n";
    for (size_t i = 0; i < files.size(); ++i) {
        io::NativeFile file;
        ec = io::open(file, files[i]);
        if (ec) {
            LOG(fatal) << "open failed";
            return ec.value();
        }
        std::vector<uint8_t> content;
        ec = io::read_to_end(file, content);
        if (ec) {
            LOG(fatal) << "read failed";
            return ec.value();
        }
        output_cc << "std::array<uint8_t, " << content.size() << "> content"
                  << i << "{{\n";
        output_cc << std::hex;
        for (size_t x = 0; x < content.size(); x += 8) {
            output_cc << "    ";
            for (size_t y = 0; y < 8 && x + y < content.size(); ++y) {
                if (y != 0) {
                    output_cc << ", ";
                }
                output_cc << "0x" << static_cast<int>(content[x + y]);
            }
            output_cc << ",\n";
        }
        output_cc << std::dec;
        output_cc << "}};\n";
        output_cc << "\n";
    }
    output_cc << "std::array<ConstBufferSpan, "  << files.size()
              << "> contents{{\n";
    for (size_t i = 0; i < files.size(); i += 4) {
        output_cc << "    ";
        for (size_t j = 0; j < 4 && i + j < files.size(); ++j) {
            if (j != 0) {
                output_cc << ", ";
            }
            output_cc << "content" << i + j;
        }
        output_cc << ",\n";
    }
    output_cc << "}};\n";
    output_cc << "\n";
    output_cc << "}  // namespace\n";
    output_cc << "\n";
    output_cc << "extern const ::util::EmbedData " << flags::variable_name
              << ";\n";
    output_cc << "const ::util::EmbedData " << flags::variable_name
              << "(names, contents);\n";
    if (!flags::cc_namespace.empty()) {
        output_cc << "\n";
        output_cc << "}  // namespace " << flags::cc_namespace << "\n";
    }
    return 0;
}
