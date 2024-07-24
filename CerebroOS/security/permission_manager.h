#ifndef PERMISSION_MANAGER_H
#define PERMISSION_MANAGER_H

#include <vector>
#include <unordered_map>

// Forward declare to avoid circular dependency
class PermissionManager;

// Use an enum to represent permissions for clarity and type safety
enum Permission {
    PERMISSION_CAMERA,
    PERMISSION_MICROPHONE,
    PERMISSION_LOCATION,
    PERMISSION_ACCESS_FILES,
    // Add more permissions as needed...
    MAX_PERMISSIONS // Denotes the total number of permissions
};

// Define a class for encapsulating the permission logic
class PermissionManager {
public:
    PermissionManager();  // Constructor to initialize

    void setPermission(int appId, Permission permission, bool granted);
    bool checkPermission(int appId, Permission permission) const;
    // ... additional methods (e.g., removeApp, getAllPermissions) could be added

private:
    // Use a map for efficient lookup by appId
    std::unordered_map<int, std::vector<bool>> appPermissions;
};

#endif // PERMISSION_MANAGER_H
