#include "permission_manager.h"
#include <vector>

// Use a vector to manage app permissions dynamically.
std::vector<std::pair<int, std::vector<bool>>> app_permissions;

// Maximum permissions per app (adjust as needed).
const int MAX_PERMISSIONS = 16;

void permission_init() {
    // Vector initialization is automatic in C++
}

void set_permission(int app_id, int permission, bool value) {
    // Find the app in the vector.
    for (auto& entry : app_permissions) {
        if (entry.first == app_id) {
            // Ensure the permission index is valid.
            if (permission >= 0 && permission < MAX_PERMISSIONS) {
                entry.second[permission] = value;
            } else {
                // Handle invalid permission index (log, throw exception, etc.)
            }
            return;
        }
    }

    // App not found, add it with a new permissions vector.
    std::vector<bool> permissions(MAX_PERMISSIONS, false);
    permissions[permission] = value;
    app_permissions.emplace_back(app_id, std::move(permissions));
}

bool check_permission(int app_id, int permission) {
    // Find the app in the vector.
    for (const auto& entry : app_permissions) {
        if (entry.first == app_id) {
            // Ensure the permission index is valid.
            if (permission >= 0 && permission < MAX_PERMISSIONS) {
                return entry.second[permission];
            } else {
                // Handle invalid permission index (log, throw exception, etc.)
            }
        }
    }
    // App not found or permission index invalid, default to false.
    return false; 
}
