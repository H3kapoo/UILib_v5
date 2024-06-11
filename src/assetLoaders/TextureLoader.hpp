#pragma once

#include <cstdint>
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
        uint32_t id{0};
        int32_t width{-1};
        int32_t height{-1};
        int32_t numChannels{-1};
    };

    typedef TextureData* TextureDataPtr;
    typedef unsigned int textureId;
    typedef textureId* textureIdPtr;

    /**
     * @brief Get class instance.
     *
     * @return A reference to the class instance.
     */
    static TextureLoader& get();

    /**
     * @brief Load and retrieve a texture **TextureData** pointer handle from the specified texture path.
     *
     * This function checks if a texture with the specified texture path already exists in the textures cache.
     * If it does, it returns a pointer to its **TextureData**. Otherwise, it loads the texture, stores the
     * loaded **TextureData** in the cache and returns a pointer to the newly loaded **TextureData**.
     *
     * @note Returned dereferenced pointer value **Id** will be 0 in case any errors occured. It will never be
     * **nullptr**. No nullptr will be returned from this function.
     * @note Reason for pointer return instead of plain structure is to have easier texture reloading capabilities.
     *
     * @param filePath The file path to the texture.
     *
     * @return A pointer to the texture data structure.
     */
    TextureDataPtr loadTexture(const std::string& filePath);

    /**
     * @brief Reload a texture from the specified texture path.
     *
     * This function creates a new **TextureData** from the provided texture path and overrides
     * the cached value at **filePath** location with the newly created **TextureData** if successful,
     * otherwise it keeps previous data.
     *
     * @param filePath The file path to the texture.
     *
     */
    void reloadFromPath(const std::string& filePath);

private:
    TextureLoader() = default;
    ~TextureLoader();
    TextureLoader(const TextureLoader&) = delete;
    TextureLoader& operator=(const TextureLoader&) = delete;

    /**
     * @brief Internal - helper for loading the texture data using stbi and ogl.
     *
     * @param filePath The file path to the texture.
     */
    TextureData loadInternal(const std::string& filePath);

    /* In case a lot of objects use the same texture, it's pointless to allocate memory for each SAME texture.
       We can reference the same one, just changing uniforms as needed. */
    std::map<std::string, TextureDataPtr> texturePathToGenId;
};
} // namespace assetloaders