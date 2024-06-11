#include "ShaderLoader.hpp"

#include <fstream>
#include <sstream>

#include "src/Utility.hpp"

namespace shaderManagement
{

ShaderLoader& ShaderLoader::get()
{
    static ShaderLoader instance;
    return instance;
}

ShaderLoader::~ShaderLoader()
{
    /* Free shaderIdPtr memory */
    for (auto& [key, value] : shaderPathToGenId)
    {
        if (!value || *value == -1) { utils::printlnw("Unloading shader that failed to load: {}", key); }
        else { utils::printlni("Unloading shader Id {} for key {}", *value, key); }
        delete value;
    }
}

// clang-format off
shaderIdPtr ShaderLoader::loadFromImmediate(
    const std::string& key, const std::string& vertCode, const std::string& fragCode)
// clang-format on
{
    shaderIdPtr idPtr = shaderPathToGenId[key];
    if (idPtr != nullptr) { return idPtr; }

    shaderIdPtr newId = new shaderId;
    *newId = linkShaders(compileShaderData(vertCode, GL_VERTEX_SHADER),
        compileShaderData(fragCode, GL_FRAGMENT_SHADER));

    shaderPathToGenId[key] = newId;
    if (*newId != -1) { utils::printlni("Loaded shader Id {} from key {}", *newId, key.c_str()); }

    return newId;
}

shaderIdPtr ShaderLoader::loadFromPath(const std::string& shaderPath)
{
    shaderIdPtr idPtr = shaderPathToGenId[shaderPath];
    if (idPtr != nullptr) { return idPtr; }

    std::ifstream shaderFile(shaderPath);
    if (!shaderFile)
    {
        utils::printlne("Could not open shader file at {}\n", shaderPath.c_str());
#if EXIT_ON_ERROR == 1
        exit(1);
#endif
        return nullptr;
    }

    std::stringstream stream;
    stream << shaderFile.rdbuf();
    std::string content = stream.str();
    shaderFile.close();

    const size_t fragCutoff = content.find("/// frag ///\n"); /* WRN: LF ending handled only */

    std::string vertData{content.begin(), content.begin() + fragCutoff};
    std::string fragData{content.begin() + fragCutoff, content.end()};

    return loadFromImmediate(shaderPath, vertData, fragData);
}

shaderIdPtr ShaderLoader::loadFromPath(const std::string& vertPath, const std::string& fragPath)
{
    const std::string combinedPath{vertPath + fragPath};
    shaderIdPtr idPtr = shaderPathToGenId[combinedPath];
    if (idPtr != nullptr) { return idPtr; }

    shaderIdPtr newId = new shaderId;
    *newId = linkShaders(compileShader(vertPath, GL_VERTEX_SHADER), compileShader(fragPath, GL_FRAGMENT_SHADER));

    shaderPathToGenId[combinedPath] = newId;

    if (*newId != -1) { utils::printlni("Loaded shader {}: {} + {}", *newId, vertPath.c_str(), fragPath.c_str()); }

    return newId;
}

void ShaderLoader::reloadFromPath(const std::string& vertPath, const std::string& fragPath)
{
    shaderId newId = linkShaders(compileShader(vertPath, GL_VERTEX_SHADER),
        compileShader(fragPath, GL_FRAGMENT_SHADER));
    if (newId != -1)
    {
        const std::string combinedPath{vertPath + fragPath};
        *shaderPathToGenId[combinedPath] = newId;
        return;
    }
}

void ShaderLoader::reloadFromPath(const std::string& shaderPath)
{
    std::ifstream shaderFile(shaderPath);
    std::stringstream stream;
    stream << shaderFile.rdbuf();
    std::string content = stream.str();
    shaderFile.close();

    const size_t fragCutoff = content.find("/// frag ///\n"); /* WRN: LF ending handled only */

    std::string vertData{content.begin(), content.begin() + fragCutoff};
    std::string fragData{content.begin() + fragCutoff, content.end()};

    shaderId newId = linkShaders(compileShaderData(vertData, GL_VERTEX_SHADER),
        compileShaderData(fragData, GL_FRAGMENT_SHADER));
    if (newId != -1)
    {
        *shaderPathToGenId[shaderPath] = newId;
        return;
    }
}

void ShaderLoader::set2DTextureUnit(const char* location, uint32_t textureId, uint32_t textureUnit)
{
    /* Shader needs texture unit location in ranges from [0..maxUnits], not from [GL_TEXTURE0..maxGL_TEXTURE] */
    setInt(location, textureUnit - GL_TEXTURE0);
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, textureId);
}

void ShaderLoader::setInt(const char* location, int value)
{
    int loc = glGetUniformLocation(activeShaderId, location);
    if (loc == -1) return handleNotFound(location);
    glUniform1i(loc, value);
}

void ShaderLoader::setIntVec(const char* location, uint32_t amount, int* flatValues)
{
    int loc = glGetUniformLocation(activeShaderId, location);
    if (loc == -1) return handleNotFound(location);
    glUniform1iv(loc, amount, flatValues);
}

void ShaderLoader::setVec1f(const char* location, float value)
{
    int loc = glGetUniformLocation(activeShaderId, location);
    if (loc == -1) return handleNotFound(location);
    glUniform1f(loc, value);
}

void ShaderLoader::setVec2f(const char* location, glm::vec2 value)
{
    int loc = glGetUniformLocation(activeShaderId, location);
    if (loc == -1) return handleNotFound(location);
    glUniform2f(loc, value.x, value.y);
}

void ShaderLoader::setVec3f(const char* location, glm::vec3 value)
{
    int loc = glGetUniformLocation(activeShaderId, location);
    if (loc == -1) return handleNotFound(location);
    glUniform3f(loc, value.x, value.y, value.z);
}

void ShaderLoader::setVec4f(const char* location, glm::vec4& value) const
{
    int loc = glGetUniformLocation(activeShaderId, location);
    if (loc == -1) return handleNotFound(location);
    glUniform4f(loc, value.r, value.g, value.b, value.a);
}

void ShaderLoader::setVec4fVec(const char* location, uint32_t amount, const float* flatValues) const
{
    int loc = glGetUniformLocation(activeShaderId, location);
    if (loc == -1) return handleNotFound(location);
    glUniform4fv(loc, amount, flatValues);
}

void ShaderLoader::setMatrix4(const char* location, const glm::mat4 value) const
{
    int loc = glGetUniformLocation(activeShaderId, location);
    if (loc == -1) return handleNotFound(location);
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderLoader::setMatrix4Vec(const char* location, uint32_t amount, const float* flatValues)
{
    int loc = glGetUniformLocation(activeShaderId, location);
    if (loc == -1) return handleNotFound(location);
    glUniformMatrix4fv(loc, amount, GL_FALSE, flatValues);
}

void ShaderLoader::setActiveShaderId(const shaderId id)
{
    if (activeShaderId == id) { return; }
    activeShaderId = id;
    glUseProgram(activeShaderId);
}

void ShaderLoader::resetBoundShader() const
{
    glUseProgram(0);
}

shaderId ShaderLoader::getActiveShaderId() const
{
    return activeShaderId;
}

int ShaderLoader::linkShaders(int vertShaderId, int fragShaderId)
{
    if (vertShaderId == -1 || fragShaderId == -1) { return -1; }

    auto shId = glCreateProgram();

    glAttachShader(shId, vertShaderId);
    glAttachShader(shId, fragShaderId);
    glLinkProgram(shId);

    int success;
    char infoLog[512];
    glGetProgramiv(shId, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(shId, 512, nullptr, infoLog);
        utils::printlne("Could not link program because:\n\t{}\n", infoLog);
#if EXIT_ON_ERROR == 1
        exit(1);
#endif
        return -1;
    }

    glDeleteShader(vertShaderId);
    glDeleteShader(fragShaderId);

    return shId;
}

int ShaderLoader::compileShader(const std::string& sourcePath, int32_t shaderType)
{
    std::string type = shaderType == GL_VERTEX_SHADER ? "VERTEX" : "FRAG";
    std::ifstream shaderFile(sourcePath);
    if (!shaderFile)
    {
        utils::printlne("Could not open {} shader file at {}\n", type.c_str(), sourcePath.c_str());
#if EXIT_ON_ERROR == 1
        exit(1);
#endif
        return -1;
    }

    std::stringstream stream;
    stream << shaderFile.rdbuf();
    std::string content = stream.str();
    shaderFile.close();

    return compileShaderData(content, shaderType);
}

int ShaderLoader::compileShaderData(const std::string& data, int32_t shaderType)
{
    const char* src = data.c_str();
    uint32_t shaderPart = glCreateShader(shaderType);
    glShaderSource(shaderPart, 1, &src, NULL);
    glCompileShader(shaderPart);

    int success;
    char infoLog[512];
    glGetShaderiv(shaderPart, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaderPart, 512, NULL, infoLog);
        std::string type = shaderType == GL_VERTEX_SHADER ? "VERTEX" : "FRAG";
        utils::printlne("Compile failed for shader {} because:\n\t{}", type.c_str(), infoLog);
#if EXIT_ON_ERROR == 1
        exit(1);
#endif
        return -1;
    }

    return shaderPart;
}

#if UNIFORMS_DEBUG_PRINT == 1
void ShaderLoader::handleNotFound(const char* location) const
{
    utils::printlne("Uniform '{}' has not been found in bound shader: {}\n", location, activeShaderId);
    exit(1);
}
#elif UNIFORMS_DEBUG_PRINT == 2
void ShaderLoader::handleNotFound(const char* location) const
{
    utils::printlne("Uniform '{}' has not been found in bound shader: {}\n", location, gActiveShaderId);
}
#else
void ShaderLoader::handleNotFound(const char* location) {}
#endif
} // namespace shaderManagement