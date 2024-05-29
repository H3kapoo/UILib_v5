#include "TextureLoader.hpp"

#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../vendors/stb_image.h"

#include "../Utility.hpp"

namespace assetloaders
{
/**
 * @brief Get class instance.
 *
 * @return A reference to the class instance.
 */
TextureLoader& TextureLoader::get()
{
    static TextureLoader instance;
    return instance;
}

/**
 * @brief Deallocate cached texture pointers at end of life.
 *
 */
TextureLoader::~TextureLoader()
{
    for (auto& [key, value] : texturePathToGenId)
    {
        /* Free textureIdPtr memory */
        utils::printlni("Unloading texture Id {} for key {}", value->id, key);
        delete value;
    }
}

/**
 * @brief Load and retrieve a texture **TextureData** pointer handle from the specified texture path.
 *
 * This function checks if a texture with the specified texture path already exists in the textures cache.
 * If it does, it returns a pointer to its **TextureData**. Otherwise, it loads the texture, stores the
 * loaded **TextureData** in the cache and returns a pointer to the newly loaded **TextureData**.
 *
 * @note Returned dereferenced pointer value **Id** will be 0 in case any errors occured. It will never be **nullptr**.
 * @note Reason for pointer return instead of plain structure is to have easier texture reloading capabilities.
 *
 * @param filePath The file path to the texture.
 *
 * @return A pointer to the texture data structure.
 */
TextureLoader::TextureDataPtr TextureLoader::loadTexture(const std::string& filePath)
{
    TextureDataPtr dataPtr = texturePathToGenId[filePath];
    if (dataPtr != nullptr) { return dataPtr; }

    TextureDataPtr newDataPtr = new TextureData;
    *newDataPtr = loadInternal(filePath);

    texturePathToGenId[filePath] = newDataPtr;

    return newDataPtr;
}

/**
 * @brief Reload a texture from the specified texture path.
 *
 * This function creates a new **TextureData** from the provided texture path and overrides
 * the cached value at **filePath** location with the newly created **TextureData** if successfull,
 * otherwise it keeps previous data.
 *
 * @param filePath The file path to the texture.
 *
 * @return Doesn't return anything.
 */
void TextureLoader::reloadFromPath(const std::string& filePath)
{

    /* Previously allocated data needs NOT to be freed as we only need to overwrite it, not make a new memory region */
    if (!texturePathToGenId.contains(filePath))
    {
        utils::printlnw("Trying to reload texture that wasn't loaded before: {}", filePath);
        return;
    }

    TextureData reloadedData = loadInternal(filePath);
    if (reloadedData.id == 0)
    {
        utils::printlne("Texture couldn't be reloaded: {}", filePath);
        return;
    }

    *texturePathToGenId[filePath] = reloadedData;
}

/**
 * @brief Internal - helper for loading the texture data using stbi and ogl.
 *
 */
TextureLoader::TextureData TextureLoader::loadInternal(const std::string& filePath)
{
    TextureData newData;

    unsigned char* data = stbi_load(filePath.c_str(), &newData.width, &newData.height, &newData.numChannels, 0);
    if (!data)
    {
        utils::printlne("Failed to load texture. Check path correctness for {}", filePath);
        return newData;
    }

    glGenTextures(1, &newData.id);
    glBindTexture(GL_TEXTURE_2D, newData.id);

    // filters and repeat method
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (filePath.ends_with(".png"))
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newData.width, newData.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    else { glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, newData.width, newData.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); }
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    utils::printlni("Loaded texture (ch:{}/w:{}/h:{}) Id {} from {} ", newData.numChannels, newData.width,
        newData.height, newData.id, filePath);

    return newData;
}

} // namespace assetloaders