/* Allows to manipulate an HDR texture. */

#ifndef HDR_TEXTURE_H
#define HDR_TEXTURE_H

#include <string>
#include <cmath>

#include <FreeImage.h>
#include <glm/glm.hpp>

enum channels
{
    R = 1 << 0,
    G = 1 << 1,
    B = 1 << 2,
    RG = R | G,
    RB = R | B,
    GB = G | B,
    RGB = R | G | B,
};

inline channels operator+(const channels& a, const channels& b)
{
    return (channels)((unsigned)a | (unsigned)b);
}

class hdr_texture
{
public:
    hdr_texture& operator=(const hdr_texture &other);
    hdr_texture(size_t width, size_t height);
    hdr_texture(const hdr_texture &other);
    hdr_texture(const std::string& path);
    hdr_texture(FIBITMAP *dib);
    ~hdr_texture();

    // Load/Save functions

    void load(const std::string& path);
    void save(const std::string& path) const;

    // Superposition functions

    void add(const hdr_texture& other, const channels &which = channels::RGB);
    void sub(const hdr_texture& other, const channels &which = channels::RGB);

    // Global functions

    void normalize(const channels &which = channels::RGB);

    // Resize functions

    void resize(size_t newWidth, size_t newHeight, FREE_IMAGE_FILTER filter = FILTER_CATMULLROM);

    // Pixel access functions

    glm::vec4* operator[](size_t y);
	const glm::vec4* operator[](size_t y) const;

    glm::vec4& operator()(size_t x, size_t y);
	const glm::vec4& operator()(size_t x, size_t y) const;

    // Image property functions

    size_t width() const;
    size_t height() const;
private:
    FIBITMAP *dib;
};

#endif
