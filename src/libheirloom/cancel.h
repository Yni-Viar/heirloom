#pragma once

#include "libheirloom/pch.h"

namespace libheirloom {

class OperationCanceledException : public std::exception {
   public:
    OperationCanceledException();
};

class CancellationToken {
   public:
    CancellationToken();
    explicit CancellationToken(std::shared_ptr<std::atomic<bool>> cancelRequested);
    ~CancellationToken() = default;

    // Checks if cancellation has been requested
    bool isCancellationRequested() const;

    // Throws OperationCanceledException if cancellation is requested
    void throwIfCancellationRequested() const;

   private:
    std::shared_ptr<std::atomic<bool>> cancelRequested_;
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
    std::shared_ptr<std::atomic<bool>> cancelRequested_;
};

}  // namespace libheirloom
