#ifndef REQUESTRESULTS_H
#define REQUESTRESULTS_H

#include <string>
#include <variant>

#include "DiskStorageConfig.h"

// Encapsulate the different types of Results from command inputs in a generic Result type

// Success results
struct SuccessRecordFoundInBuffer {
    int block_id{};
    int frame_id{};
    Record record;
};

struct SuccessRecordLoadedToEmptyBuffer {
    int block_id{};
    int frame_id{};
    Record record;
};

struct SuccessRecordLoadedToReplacedBuffer {
    int old_block_id{};
    bool old_frame_was_dirty{};
    int block_id{};
    int frame_id{};
    Record record;
};

struct SuccessRecordSetFromMemory {
    Record old_record;
    int block_id{};
    int frame_id{};
};

struct SuccessRecordSetFromDisk {
    Record old_record;
    int block_id{};
    int frame_id{};
};

struct SuccessRecordSetFromDiskEvictedFrame {
    Record old_record;
    int block_id{};
    int frame_id{};
    int evicted_block_id{};
};

struct SuccessPinningDirectlyFromBuffer {
    bool pinning;
    int frame_number;
    int block_number;
    bool was_pinned;
};

struct SuccessPinningLoadedToEmptyBuffer {
    bool pinning;
    int frame_number;
};

struct SuccessPinningLoadedToReplacedBuffer {
    bool pinning;
    int old_block_number;
    int frame_number;
    bool was_dirty;
};

// Error results
struct ErrorAllBuffersOccupiedAndPinned {};
struct ErrorRecordNotFound {std::string message;};
struct ErrorRecordNot40Bytes {};
struct ErrorRecordIncorrectFormat {};
struct ErrorInexistantBlockNumber {std::string message;};
struct ErrorCantUnpinUnloadedBuffer {};

using Result = std::variant<
    // Success results
    SuccessRecordFoundInBuffer,
    SuccessRecordLoadedToEmptyBuffer,
    SuccessRecordLoadedToReplacedBuffer,
    SuccessRecordSetFromMemory,
    SuccessRecordSetFromDisk,
    SuccessRecordSetFromDiskEvictedFrame,
    SuccessPinningDirectlyFromBuffer,
    SuccessPinningLoadedToEmptyBuffer,
    SuccessPinningLoadedToReplacedBuffer,

    // Error results
    ErrorAllBuffersOccupiedAndPinned,
    ErrorRecordNotFound,
    ErrorRecordNot40Bytes,
    ErrorRecordIncorrectFormat,
    ErrorInexistantBlockNumber,
    ErrorCantUnpinUnloadedBuffer
>;

#endif // REQUESTRESULTS_H
