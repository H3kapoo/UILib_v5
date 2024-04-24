#pragma once

#include <map>
#include <string>

namespace assetloaders
{

/**
 * @brief Simple class that deals with loading texture assets.
 *
 * Class that deals with texture objects such as loading them and provinding
 * a gateway to their object id and relevant other information.
 *
 */
class TextureLoader
{
public:
    struct TextureData
    {
        unsigned int id{0};
        int32_t width{-1};
        int32_t height{-1};
        int32_t numChannels{-1};
    };

    typedef TextureData* TextureDataPtr;
    typedef unsigned int textureId;
    typedef textureId* textureIdPtr;

    static TextureLoader& get();

    TextureDataPtr loadTexture(const std::string& filePath);
    void reloadFromPath(const std::string& filePath);

private:
    TextureLoader() = default;
    ~TextureLoader();
    TextureLoader(const TextureLoader&) = delete;
    TextureLoader& operator=(const TextureLoader&) = delete;

    TextureData loadInternal(const std::string& filePath);

    /* In case a lot of objects use the same texture, it's pointless to allocate memory for each SAME texture.
       We can reference the same one, just changing uniforms as needed. */
    std::map<std::string, TextureDataPtr> texturePathToGenId;
};
} // namespace assetloaders