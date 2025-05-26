#include "libprogman/pch.h"
#include "libprogman/InstalledAppList.h"
#include "libprogman/Shortcut.h"

namespace libprogman {

InstalledAppList::InstalledAppList(ShortcutFactory* shortcutFactory, immer::vector<std::filesystem::path> foldersToScan)
    : shortcutFactory_(shortcutFactory), foldersToScan_(std::move(foldersToScan)), apps_() {}

immer::vector<std::shared_ptr<Shortcut>> InstalledAppList::apps(CancellationToken cancel) {
    std::lock_guard<std::mutex> lock(mutex_);

    // Keep track of file paths we've processed in this pass
    std::set<std::filesystem::path> processedPaths;

    // First, create a new vector to hold our updated list
    auto newApps = immer::vector<std::shared_ptr<Shortcut>>{};
    auto newAppsTransient = newApps.transient();

    // Scan folders for shortcuts
    for (const auto& folder : foldersToScan_) {
        cancel.throwIfCancellationRequested();

        try {
            std::filesystem::path folderPath = folder;

            if (!std::filesystem::exists(folderPath)) {
                continue;
            }

            // Recursively scan for .lnk files
            for (const auto& entry : std::filesystem::recursive_directory_iterator(folderPath)) {
                if (!entry.is_regular_file() || entry.path().extension() != L".lnk") {
                    continue;
                }

                cancel.throwIfCancellationRequested();

                const auto& filePath = entry.path();
                const auto lastWriteTime = std::filesystem::last_write_time(filePath);

                // Add the path to our processed set
                processedPaths.insert(filePath);

                // Check if we already have this shortcut and it's up to date
                bool needsUpdate = true;
                for (size_t i = 0; i < apps_.size(); ++i) {
                    if (apps_[i]->path() == filePath) {
                        if (apps_[i]->lastWriteTime() == lastWriteTime) {
                            // The shortcut hasn't changed, add it to our new vector
                            newAppsTransient.push_back(apps_[i]);
                            needsUpdate = false;
                        }
                        break;
                    }
                }

                // If we need to update the shortcut, load it from disk
                if (needsUpdate) {
                    try {
                        auto shortcut = shortcutFactory_->open(filePath, lastWriteTime);
                        if (shortcut) {
                            newAppsTransient.push_back(shortcut);
                        }
                    } catch (const std::exception&) {
                        // Skip shortcuts that fail to load
                    }
                }
            }
        } catch (const std::exception&) {
            // Skip folders that fail to scan
        }
    }

    // Add existing shortcuts that weren't found in the scan
    // (they might be from other directories we're not scanning now)
    for (size_t i = 0; i < apps_.size(); ++i) {
        cancel.throwIfCancellationRequested();

        auto shortcut = apps_[i];
        const auto& path = shortcut->path();

        // Only keep shortcuts that weren't processed in this scan
        if (processedPaths.find(path) == processedPaths.end()) {
            newAppsTransient.push_back(shortcut);
        }
    }

    // Update the persistent vector
    cancel.throwIfCancellationRequested();
    apps_ = newAppsTransient.persistent();

    return apps_;
}

}  // namespace libprogman
