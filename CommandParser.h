
#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <iostream>
#include <string>
#include <sstream>
#include <variant>
#include <unordered_map>
#include <functional>

#include "DiskStorageConfig.h"
#include "BufferPool.h"
#include "RequestResults.h"

struct GetCommand {
    int record_id;

    Result execute(BufferPool& buffer_pool) const {
        return buffer_pool.get_record(record_id);
    }
};

struct SetCommand {
    int record_id;
    Record record;

    Result execute(BufferPool& buffer_pool) const {
        return buffer_pool.set_record(record_id, record);
    }

};

struct PinCommand {
    int block_id;

    Result execute(BufferPool& buffer_pool) const {
        return buffer_pool.pin_block(block_id);
    }
};

struct UnpinCommand {
    int block_id;

    Result execute(BufferPool& buffer_pool) const {
        return buffer_pool.unpin_block(block_id);
    }
};

struct InvalidCommand {
    std::string reason;

    static Result execute(BufferPool& buffer_pool) {
        return ErrorRecordNotFound("Invalid command");
    }

};
struct ExitCommand {
    static Result execute(BufferPool& buffer_pool) {
        return ErrorRecordNotFound("Good bye");
    }
};

using Command = std::variant<GetCommand, SetCommand, PinCommand, UnpinCommand, InvalidCommand, ExitCommand>;

class CommandParser {
public:
    CommandParser();
    Command parseCommand(const std::string& input);

private:
    std::unordered_map<std::string, std::function<Command(std::istringstream&)>> command_map;
    Command parseGetCommand(std::istringstream& stream);
    Command parseSetCommand(std::istringstream& stream);
    Command parsePinCommand(std::istringstream& stream);
    Command parseUnpinCommand(std::istringstream& stream);
};

#endif //COMMAND_PARSER_H
