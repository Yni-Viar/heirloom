#pragma once

#include "libprogman/pch.h"

namespace libprogman {

class OperationCanceledException : public std::exception {
   public:
    OperationCanceledException();
};

class CancellationToken {
   public:
    CancellationToken();
    explicit CancellationToken(std::shared_ptr<bool> cancelRequested);
    ~CancellationToken() = default;

    // Checks if cancellation has been requested
    bool isCancellationRequested() const;

    // Throws OperationCanceledException if cancellation is requested
    void throwIfCancellationRequested() const;

   private:
    std::shared_ptr<bool> cancelRequested_;
    bool isValid_;
};

class CancellationTokenSource {
   public:
    CancellationTokenSource();
    ~CancellationTokenSource() = default;

    // Creates a token linked to this source
    CancellationToken createToken();

    // Requests cancellation
    void cancel();

    // Checks if cancellation has been requested
    bool isCancellationRequested() const;

   private:
    std::shared_ptr<bool> cancelRequested_;
};

}  // namespace libprogman
