#pragma once

#include <cstdint>
#include <map>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>

namespace shaderManagement
{

typedef int shaderId;
typedef shaderId* shaderIdPtr;

/* Enable or disable shader prints regarding uniforms not found */
#define UNIFORMS_DEBUG_PRINT 1 /* 0 - disabled; 1 - hard exit; 2 - soft continue */

/* Enable or disable program exit on shader loading/reloading */
#define EXIT_ON_ERROR 0 /* 0 - disabled; 1 - enabled */
/**
 * @brief Singleton providing shader load/reload and uniform access capabilities.
 *
 * Singleton class providing easy means to load/reload shaders and access to their uniforms.
 * Class also provides caching capabilities for shaders.
 */
class ShaderLoader
{
public:
    /**
     * @brief Get class instance.
     *
     * @return A reference to the class instance.
     */
    static ShaderLoader& get();

    /* Setters */
    /**
     * @brief Load and retrieve a shader **Id** pointer handle from the specified vertex and fragment shader paths.
     *
     * This function checks if a shader with the specified vertex and fragment shader already
     * exists in the shader cache. If it does, it returns a pointer to its **Id**. Otherwise, it compiles
     * and links the shaders, stores the **Id** in the cache, and returns a pointer to the new **Id**.
     *
     * @note Returned dereferenced pointer value will be -1 in case any errors occured. It will never be **nullptr**.
     * @note Reason for pointer return instead of plain integer is to have easier shader reloading capabilities.
     *
     * @param vertPath The file path to the vertex shader source code.
     * @param fragPath The file path to the fragment shader source code.
     *
     * @return A pointer to the shader program **Id**.
     */
    shaderIdPtr loadFromImmediate(const std::string& key, const std::string& vertCode, const std::string& fragCode);

    /**
     * @brief Load and retrieve a shader **Id** pointer handle from the specified shader path. File should contain both
     * vertex and fragment shader parts.
     *
     * This function checks if a shader with the specified shader path already
     * exists in the shader cache. If it does, it returns a pointer to its **Id**. Otherwise, it compiles
     * and links the shaders, stores the **Id** in the cache, and returns a pointer to the new **Id**.
     *
     * @note Returned dereferenced pointer value will be -1 in case any errors occured. It will never be **nullptr**.
     * @note Reason for pointer return instead of plain integer is to have easier shader reloading capabilities.
     * @note Vertex part is denoted by: /// vertex /// and Fragment part is denoted by: /// frag ///
     *
     * @param shaderPath The file path to the vertex + fragment shader
     *
     * @return A pointer to the shader program **Id**.
     */
    shaderIdPtr loadFromPath(const std::string& shaderPath);

    /**
     * @brief Load and retrieve a shader **Id** pointer handle from the specified vertex and fragment shader paths.
     *
     * This function checks if a shader with the specified vertex and fragment shader already
     * exists in the shader cache. If it does, it returns a pointer to its **Id**. Otherwise, it compiles
     * and links the shaders, stores the **Id** in the cache, and returns a pointer to the new **Id**.
     *
     * @note Returned dereferenced pointer value will be -1 in case any errors occured. It will never be **nullptr**.
     * @note Reason for pointer return instead of plain integer is to have easier shader reloading capabilities.
     *
     * @param vertPath The file path to the vertex shader source code.
     * @param fragPath The file path to the fragment shader source code.
     *
     * @return A pointer to the shader program **Id**.
     */
    shaderIdPtr loadFromPath(const std::string& vertPath, const std::string& fragPath);

    /**
     * @brief Reload a shader from the specified vertex and fragment shader paths.
     *
     * This function creates a new shader **Id** from the provided combined paths and overrides
     * the cached value at combined paths location with the newly created shader **Id** if successfull,
     * otherwise it keeps previous shader **Id** value.
     *
     * @param vertPath The file path to the vertex shader source code.
     * @param fragPath The file path to the fragment shader source code.
     *
     * @return Doesn't return anything.
     */
    void reloadFromPath(const std::string& vertPath, const std::string& fragPath);

    /**
     * @brief Reload a shader from the specified shader path. File should contain both
     * vertex and fragment shader parts.
     *
     * This function creates a new shader **Id** from the provided combined paths and overrides
     * the cached value at combined paths location with the newly created shader **Id** if successfull,
     * otherwise it keeps previous shader **Id** value.
     *
     * @param shaderPath The file path to the vertex + fragment shader
     *
     * @return Doesn't return anything.
     */
    void reloadFromPath(const std::string& shaderPath);

    /**
     * @brief Set currently active shader **Id**.
     *
     * @param id   Shader **Id** to be bound.
     *
     * @note All functions from this class will only affect the currenly active shader.
     *
     * @return Doesn't return anything.
     */
    void setActiveShaderId(const shaderId id);

    /**
     * @brief Reset currently active shader **Id**.
     *
     * @return Doesn't return anything.
     */
    void resetBoundShader() const;

    /* Uniforms */
    /**
     * @brief Set the active texture unit and upload **location** uniform with **textureId** id.
     *
     * @param location - Location of uniform to be set in the shader.
     * @param textureId - Id of the loaded texture to be referenced by **textureUnit**.
     * @param textureUnit - Texture unit to be enabled. Starts from GL_TEXTURE0.
     */
    void set2DTextureUnit(const char* location, uint32_t textureId, uint32_t textureUnit);

    /**
     * @brief Load integer uniform to currently active shader **Id**.
     *
     * @param location Location of uniform to be set in the shader.
     * @param value    Value to be set.
     *
     * @return Doesn't return anything.
     */
    void setInt(const char* location, int value);

    /**
     * @brief Load integer vector uniform to currently active shader **Id**.
     *
     * @param location     Location of uniform to be set in the shader.
     * @param amount       Amount of values to be set.
     * @param flatValues   Array of values to be set.
     *
     * @return Doesn't return anything.
     */
    void setIntVec(const char* location, uint32_t amount, int* flatValues);

    /**
     * @brief Load float uniform to currently active shader **Id**.
     *
     * @param location Location of uniform to be set in the shader.
     * @param value    Value to be set.
     *
     * @return Doesn't return anything.
     */
    void setVec1f(const char* location, float value);

    /**
     * @brief Load 2D-float uniform to currently active shader **Id**.
     *
     * @param location Location of uniform to be set in the shader.
     * @param value    2D-Value to be set.
     *
     * @return Doesn't return anything.
     */
    void setVec2f(const char* location, glm::vec2 value);

    /**
     * @brief Load 3D-float uniform to currently active shader **Id**.
     *
     * @param location Location of uniform to be set in the shader.
     * @param value    3D-Value to be set.
     *
     * @return Doesn't return anything.
     */
    void setVec3f(const char* location, glm::vec3 value);

    /**
     * @brief Load 4D-float uniform to currently active shader **Id**.
     *
     * @param location Location of uniform to be set in the shader.
     * @param value    4D-Value to be set.
     *
     * @return Doesn't return anything.
     */
    void setVec4f(const char* location, glm::vec4& value) const;

    // TODO:
    void setVec4fVec(const char* location, uint32_t amount, const float* value) const;

    /**
     * @brief Load 4x4 Matrix uniform to currently active shader **Id**.
     *
     * @param location Location of uniform to be set in the shader.
     * @param value    Matrix value to be set.
     *
     * @return Doesn't return anything.
     */
    void setMatrix4(const char* location, const glm::mat4 value) const;

    /**
     * @brief Load 4x4 Matrix vector uniform to currently active shader **Id**.
     *
     * @param location   Location of uniform to be set in the shader.
     * @param amount     Amount of matrices to be set in the shader.
     * @param flatValues Vector of matrices values to be set.
     *
     * @return Doesn't return anything.
     */
    void setMatrix4Vec(const char* location, uint32_t amount, const float* flatValues);

    /* Getters */
    /**
     * @brief Get currently active shader
     *
     * @return Returns currently active shader **Id**.
     */
    shaderId getActiveShaderId() const;

private:
    ShaderLoader() = default;

    /**
     * @brief Deallocate cached shader pointers at end of life.
     *
     */
    ~ShaderLoader();

    /* Private copy and assignment ctor to prevent duplicates */
    ShaderLoader(const ShaderLoader&) = delete;
    ShaderLoader& operator=(const ShaderLoader&) = delete;

    /**
     * @brief Simply print uniform not found if UNIFORMS_DEBUG_PRINT allows.
     *
     * @return Nothing.
     */
    void handleNotFound(const char* location) const;

    /**
     * @brief Link shader stages.
     *
     * @return -1 on Failure or >0 value on success.
     */
    int linkShaders(int vertShaderId, int fragShaderId);

    /**
     * @brief Compile shader from source path loaded file.
     *
     * @return -1 on Failure or >0 value on success.
     */
    int compileShader(const std::string& sourcePath, int32_t shaderType);

    /**
     * @brief Compile shader data directly. Doesn't load from file.
     *
     * @return -1 on Failure or >0 value on success.
     */
    int compileShaderData(const std::string& data, int32_t shaderType);

    /* In case a lot of objects use the same shader, it's pointless to allocate memory for each SAME shader.
       We can reference the same one, just changing uniforms as needed.
       This has the side effect that if shader A has A.color and A is used by objects X and Y and we set X color to
       "RED", because Y also uses A shader, if we don't set any color for Y, it will also render as "RED". The solution
       is to reset parameters/values that we don't want for that particular object.*/
    std::map<std::string, int*> shaderPathToGenId;

    shaderId activeShaderId{-1};
};
} // namespace shaderManagement