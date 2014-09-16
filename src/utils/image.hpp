#ifndef IMAGE_H
#define IMAGE_H

#include <FreeImage.h>
#include <glm/glm.hpp>
#include <GL/glew.h>

#include <string>

//<< Describes a set of channels (out of R, G, B)
enum channels
{
    R   = 1 << 0,
    G   = 1 << 1,
    B   = 1 << 2,
    RG  = R | G,
    RB  = R | B,
    GB  = G | B,
    RGB = R | G | B,
};

inline channels operator|(const channels& x, const channels& y)
{
    return (channels)((unsigned)x | (unsigned)y);
}

//<< General purpose image class, backed by a FreeImage bitmap
//<< Pixels are represented as an OpenGL-compatible glm::vec4
class image
{
public:
    image(int width, int height, GLuint tex);
    image& operator=(const image &other);
    image(int width, int height);
    image(const std::string& path);
    image(const image &other);
    image(FIBITMAP *dib);
    ~image();

    //<< Loads a new image file, or saves this image to an EXR file
    void load(const std::string& path);
    void save(const std::string& path) const;

    //<< Pointwise addition/etc of channels of another image to this one
    void add(const image& other, const channels& which = channels::RGB);
    void sub(const image& other, const channels& which = channels::RGB);
    void mul(const image& other, const channels& which = channels::RGB);

    //<< Fills channels of this image with a given background color
    void fill(const glm::vec4& bkgd, const channels& which = channels::RGB);

    //<< Sets each channel of this image to the average of all channels
    void grayscale();

    //<< Multiplies every pixel of this image with a given color
    void colorize(const glm::vec4& color);

    //<< Copies one channel (which must be R, G, or B) to all others
    void reproduce(const channels& which);

    //<< Composes an image out the the R channel of r, g, b images
    static image compose(const image& r, const image& g, const image& b);

    //<< Normalizes channels of this image such that:
    //<<  - the channel sums up to 1, if local is false
    //<<  - each pixel channel value is at most 1, otherwise
    void normalize(bool local, const channels& which = channels::RGB);

    //<< Resizes this image to new dimensions with a given filter, and returns the result
    image resize(int newWidth, int newHeight, FREE_IMAGE_FILTER filter = FILTER_CATMULLROM) const;

    //<< Enlarges this image, centering the contents and padding with black on the sides
    image enlarge(int newWidth, int newHeight) const;

    //<< Returns the subregion of this image specified by a given rectangle
    image subregion(int rectX, int rectY, int rectW, int rectH) const;

    //<< Returns this image, padded on the sides with a black background
    image zero_pad(int left, int top, int right, int bottom) const;

    //<< Accesses the scanline pointer for a given row
    glm::vec4* operator[](int y);
	const glm::vec4* operator[](int y) const;

    //<< Accesses the pixel at a given point
    glm::vec4& operator()(int x, int y);
	const glm::vec4& operator()(int x, int y) const;

    //<<Returns a raw pointer to the bitmap data
    glm::vec4* data();
    const glm::vec4* data() const;

    //<< Accesses the width and height of the image
    int width() const;
    int height() const;
private:
    FIBITMAP *dib;
};

namespace utils
{
    //<< Draws a circle of a given radius and color
    //<< The returned image has side 2 * radius
    image draw_circle(int radius, bool anti_alias, const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));
}

#endif
