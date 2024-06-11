#include "TextureLoader.hpp"

#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION

#include "src/Utility.hpp"
#include "src/vendors/stb_image.h"

namespace assetloaders
{
TextureLoader& TextureLoader::get()
{
    static TextureLoader instance;
    return instance;
}

TextureLoader::~TextureLoader()
{
    for (auto& [key, value] : texturePathToGenId)
    {
        /* Free textureIdPtr memory */
        utils::printlni("Unloading texture Id {} for key {}", value->id, key);
        delete value;
    }
}

TextureLoader::TextureDataPtr TextureLoader::loadTexture(const std::string& filePath)
{
    TextureDataPtr dataPtr = texturePathToGenId[filePath];
    if (dataPtr != nullptr) { return dataPtr; }

    TextureDataPtr newDataPtr = new TextureData;
    *newDataPtr = loadInternal(filePath);

    texturePathToGenId[filePath] = newDataPtr;

    return newDataPtr;
}

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
    else
    {
        // Empty
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, newData.width, newData.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    utils::printlni("Loaded texture (ch:{}/w:{}/h:{}) Id {} from {} ", newData.numChannels, newData.width,
        newData.height, newData.id, filePath);

    return newData;
}

} // namespace assetloaders