#include "libprogman/pch.h"
#include "libprogman/cancel.h"

namespace libprogman {

// OperationCanceledException implementation
OperationCanceledException::OperationCanceledException() : std::exception("The operation was canceled.") {}

// CancellationTokenSource implementation
CancellationTokenSource::CancellationTokenSource() : cancelRequested_(std::make_shared<bool>(false)) {}

CancellationToken CancellationTokenSource::createToken() {
    return CancellationToken(cancelRequested_);
}

void CancellationTokenSource::cancel() {
    *cancelRequested_ = true;
}

bool CancellationTokenSource::isCancellationRequested() const {
    return *cancelRequested_;
}

// CancellationToken implementation
CancellationToken::CancellationToken() : cancelRequested_(std::make_shared<bool>(false)), isValid_(false) {}

CancellationToken::CancellationToken(std::shared_ptr<bool> cancelRequested)
    : cancelRequested_(std::move(cancelRequested)), isValid_(true) {}

bool CancellationToken::isCancellationRequested() const {
    if (!isValid_) {
        return false;
    }
    return *cancelRequested_;
}

void CancellationToken::throwIfCancellationRequested() const {
    if (isCancellationRequested()) {
        throw OperationCanceledException();
    }
}

}  // namespace libprogman
