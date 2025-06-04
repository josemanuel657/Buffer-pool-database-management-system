
#include "CommandParser.h"

CommandParser::CommandParser() {
  command_map["EXIT"] = [](std::istringstream& stream) { return ExitCommand{}; };
  command_map["GET"] = [this](std::istringstream& stream) { return parseGetCommand(stream); };
  command_map["SET"] = [this](std::istringstream& stream) { return parseSetCommand(stream); };
  command_map["PIN"] = [this](std::istringstream& stream) { return parsePinCommand(stream); };
  command_map["UNPIN"] = [this](std::istringstream& stream) { return parseUnpinCommand(stream); };
}

Command CommandParser::parseCommand(const std::string& input) {
    std::istringstream stream(input);
    std::string command_type;
    stream >> command_type;

    auto command = command_map.find(command_type);

    if (command == command_map.end())
      return InvalidCommand{"Unknown command type: " + command_type};

    return command->second(stream);
}

Command CommandParser::parseGetCommand(std::istringstream& stream) {
    int record_id;
    if (!(stream >> record_id)) {
        return InvalidCommand{"GET command requires a valid integer record ID"};
    }
    return GetCommand{record_id};
}

Command CommandParser::parseSetCommand(std::istringstream& stream) {
    int record_id;
    std::string value;
    if (!(stream >> record_id)) {
        return InvalidCommand{"SET command requires a valid integer record ID"};
    }

    std::getline(stream, value);
    if (value.empty() || value.length() < 2) {
        return InvalidCommand{"SET command requires a value"};
    }

    value = value.substr(1); // Remove leading space
    if (value.length() != RECORD_SIZE) {
        return InvalidCommand{"SET value must be exactly 40 characters long"};
    }

    Record record{};
    std::copy_n(value.begin(), RECORD_SIZE, record.data.begin());
    return SetCommand{record_id, record};
}

Command CommandParser::parsePinCommand(std::istringstream& stream) {
    int block_id;
    if (!(stream >> block_id)) {
        return InvalidCommand{"PIN command requires a valid integer block ID"};
    }
    return PinCommand{block_id};
}

Command CommandParser::parseUnpinCommand(std::istringstream& stream) {
    int block_id;
    if (!(stream >> block_id)) {
        return InvalidCommand{"UNPIN command requires a valid integer block ID"};
    }
    return UnpinCommand{block_id};
}
