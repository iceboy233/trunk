#include <cstdint>
#include <vector>

#include "absl/random/random.h"
#include "io/posix/file.h"
#include "io/stream.h"
#include "util/hash-filter.h"

int main() {
    io::OStream os(io::posix::stdout);
    absl::InsecureBitGen gen;
    util::HashFilter32 filter(262144);
    std::vector<uint64_t> fingerprints;
    for (int i = 0; i < 950000; ++i) {
        uint64_t fingerprint = absl::Uniform<uint64_t>(gen);
        filter.insert(fingerprint);
        fingerprints.push_back(fingerprint);
    }
    os << "inserted " << filter.size() << std::endl;

    int true_positives = 0;
    int false_negatives = 0;
    for (uint64_t fingerprint : fingerprints) {
        if (filter.test(fingerprint)) {
            ++true_positives;
        } else {
            ++false_negatives;
        }
    }
    os << "true positives " << true_positives << std::endl;
    os << "false negatives " << false_negatives << std::endl;

    int false_positives = 0;
    int true_negatives = 0;
    for (int i = 0; i < 10000000; ++i) {
        uint64_t fingerprint = absl::Uniform<uint64_t>(gen);
        if (filter.test(fingerprint)) {
            ++false_positives;
        } else {
            ++true_negatives;
        }
    }
    os << "false positives " << false_positives << std::endl;
    os << "true negatives " << true_negatives << std::endl;
}
