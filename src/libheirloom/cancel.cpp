#include "libheirloom/pch.h"
#include "libheirloom/cancel.h"

namespace libheirloom {

// OperationCanceledException implementation
OperationCanceledException::OperationCanceledException() : std::exception("The operation was canceled.") {}

// CancellationTokenSource implementation
CancellationTokenSource::CancellationTokenSource() : cancelRequested_(std::make_shared<std::atomic<bool>>(false)) {}

CancellationToken CancellationTokenSource::createToken() {
    return CancellationToken(cancelRequested_);
}

void CancellationTokenSource::cancel() {
    cancelRequested_->store(true);
}

bool CancellationTokenSource::isCancellationRequested() const {
    return cancelRequested_->load();
}

// CancellationToken implementation
CancellationToken::CancellationToken() : cancelRequested_(std::make_shared<std::atomic<bool>>(false)), isValid_(false) {}

CancellationToken::CancellationToken(std::shared_ptr<std::atomic<bool>> cancelRequested)
    : cancelRequested_(std::move(cancelRequested)), isValid_(true) {}

bool CancellationToken::isCancellationRequested() const {
    if (!isValid_) {
        return false;
    }
    return cancelRequested_->load();
}

void CancellationToken::throwIfCancellationRequested() const {
    if (isCancellationRequested()) {
        throw OperationCanceledException();
    }
}

}  // namespace libheirloom
