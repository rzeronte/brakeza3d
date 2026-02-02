//
// Created by Claude on 02/02/2026.
//

#ifndef BRAKEZA3D_FILEPATHS_H
#define BRAKEZA3D_FILEPATHS_H

#include <string>
#include <utility>

namespace FilePath {

    template<typename Derived>
    struct PathBase {
        std::string path;

        PathBase() = default;

        // Implicit constructor from std::string for backward compatibility
        PathBase(std::string p) : path(std::move(p)) {}  // NOLINT(google-explicit-constructor)

        // Implicit constructor from const char* for backward compatibility
        PathBase(const char* p) : path(p) {}  // NOLINT(google-explicit-constructor)

        // Implicit conversion to std::string for backward compatibility
        operator const std::string&() const { return path; }  // NOLINT(google-explicit-constructor)

        [[nodiscard]] const char* c_str() const { return path.c_str(); }
        [[nodiscard]] bool empty() const { return path.empty(); }
        [[nodiscard]] const std::string& str() const { return path; }

        // Comparison operators
        bool operator==(const PathBase& other) const { return path == other.path; }
        bool operator==(const std::string& other) const { return path == other; }
        bool operator!=(const PathBase& other) const { return path != other.path; }
        bool operator!=(const std::string& other) const { return path != other; }

        // Assignment operator
        PathBase& operator=(const std::string& p) { path = p; return *this; }

        // Concatenation operator
        Derived operator+(const std::string& suffix) const {
            return Derived(path + suffix);
        }

        Derived operator+(const char* suffix) const {
            return Derived(path + suffix);
        }
    };

    // Project files (.json)
    struct ProjectFile : PathBase<ProjectFile> {
        using PathBase::PathBase;
    };

    // Scene files (.json)
    struct SceneFile : PathBase<SceneFile> {
        using PathBase::PathBase;
    };

    // Vertex shader files (.vs)
    struct VertexShaderFile : PathBase<VertexShaderFile> {
        using PathBase::PathBase;
    };

    // Fragment shader files (.fs)
    struct FragmentShaderFile : PathBase<FragmentShaderFile> {
        using PathBase::PathBase;
    };

    // Shader configuration files (.json)
    struct ShaderConfigFile : PathBase<ShaderConfigFile> {
        using PathBase::PathBase;
    };

    // LUA script files (.lua)
    struct ScriptFile : PathBase<ScriptFile> {
        using PathBase::PathBase;
    };

    // 3D model files (.fbx, .obj, etc.)
    struct ModelFile : PathBase<ModelFile> {
        using PathBase::PathBase;
    };

    // Image files (.png, .jpg, etc.)
    struct ImageFile : PathBase<ImageFile> {
        using PathBase::PathBase;
    };

    // Types definition files (.json)
    struct TypesFile : PathBase<TypesFile> {
        using PathBase::PathBase;
    };

} // namespace FilePath

#endif //BRAKEZA3D_FILEPATHS_H
