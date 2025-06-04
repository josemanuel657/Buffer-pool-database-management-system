#include <iostream>
#include <string>
#include <stdexcept>

#include "BufferPool.h"
#include "CommandParser.h"
#include "RequestResults.h"
#include "ResultDisplayer.h"

int parseBufferPoolSize(int argc, const char* argv[]);
void display_commands();

int main(const int argc, const char* argv[]) {

    if (argc != 2) {
        std::cerr << "Correct Usage: " << argv[0] << " <buffer_pool_size>" << std::endl;
        return EXIT_FAILURE;
    }

    int buffer_pool_size = parseBufferPoolSize(argc,argv);
    if (buffer_pool_size == -1) return EXIT_FAILURE;

    BufferPool buffer_pool{buffer_pool_size};
    std::cout << "Initializing the buffer pool look up tables by reading from disk blocks... " <<std::endl;
    buffer_pool.initialize();

    std::cout << "Buffer pool initialized." << std::endl;
    std::cout << "Buffer pool size: " << buffer_pool_size << std::endl;


    display_commands();


    std::cout << "The program is ready for the next command" << std::endl;

    CommandParser command_parser;

    while (true) {
        std::cout << "Enter a command (type 'EXIT' to quit): ";

        std::string user_input;
        std::getline(std::cin, user_input);

        Command command = command_parser.parseCommand(user_input);

        if (std::holds_alternative<ExitCommand>(command)) {
            std::cout << "Goodbye!" << std::endl;
            return EXIT_SUCCESS;
        }

        if (std::holds_alternative<InvalidCommand>(command)) {
            const auto& [reason] = std::get<InvalidCommand>(command);
            std::cout << "Invalid command: " << reason << std::endl;
            continue;
        }

        Result result = std::visit([&buffer_pool](auto&& cmd) -> Result {
            return cmd.execute(buffer_pool);
        }, command);

        ResultDisplayer::display(result);
    }

}

int parseBufferPoolSize(int argc, const char* argv[]) {
    if (argc < 2) {
        std::cerr << "Buffer pool size argument is missing.\n";
        return -1;
    }
    int buffer_pool_size;
    try {
        buffer_pool_size = std::stoi(argv[1]);
        if (buffer_pool_size <= 0) {
            std::cerr << "Buffer pool size must be positive.\n";
            return -1;
        }
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument: " << argv[1] << " is not an integer.\n";
        return -1;
    } catch (const std::out_of_range& e) {
        std::cerr << "Out of range error: The number is too large.\n";
        return -1;
    }

    return buffer_pool_size;
}

void display_commands() {
    std::cout << "Welcome to my Database Management System!\n";
    std::cout << "You can interact with the system using the following commands:\n\n";

    std::cout << "Commands:\n";
    std::cout << "  - GET <Record_number>            : Example: GET 001\n";
    std::cout << "  - SET <Record_number> <Record>   : Example: SET 200 F04-Rec301, Name301, address301, age301.\n";
    std::cout << "  - PIN <Block_number>             : Example: PIN 10\n";
    std::cout << "  - UNPIN <Block_number>           : Example: UNPIN 10\n";
    std::cout << "  - EXIT                          : Exit the program\n";

    std::cout << "\n<Record_number>: XXX\n";
    std::cout << "Record: 40 bytes\n";
}

