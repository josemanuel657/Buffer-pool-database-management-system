#ifndef RESULTDISPLAYER_H
#define RESULTDISPLAYER_H

#include "RequestResults.h"
#include <iostream>
#include <string>

class ResultDisplayer {
public:

    // Visitor pattern using variant, to display the different types of results of the commands
    static void display(const Result& result);

private:

    static void display_result(const SuccessRecordFoundInBuffer& result);
    static void display_result(const ErrorCantUnpinUnloadedBuffer& result);
    static void display_result(const SuccessRecordLoadedToEmptyBuffer& result);
    static void display_result(const SuccessRecordLoadedToReplacedBuffer& result);
    static void display_result(const SuccessRecordSetFromMemory& result);
    static void display_result(const SuccessRecordSetFromDisk& result);
    static void display_result(const SuccessRecordSetFromDiskEvictedFrame& result);
    static void display_result(const ErrorAllBuffersOccupiedAndPinned& result);
    static void display_result(const ErrorRecordNotFound& result);
    static void display_result(const ErrorRecordNot40Bytes& result);
    static void display_result(const ErrorRecordIncorrectFormat& result);
    static void display_result(const ErrorInexistantBlockNumber& result);
    static void display_result(const SuccessPinningDirectlyFromBuffer& result);
    static void display_result(const SuccessPinningLoadedToEmptyBuffer& result);
    static void display_result(const SuccessPinningLoadedToReplacedBuffer& result);
};

#endif // RESULTDISPLAYER_H
