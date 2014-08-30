#include "image.hpp"

#include <functional>
#include <stdexcept>
#include <cassert>
#include <cmath>

static void rop1n(image &src, const std::function<glm::vec4(const glm::vec4&)>& rop)
{
    size_t w = src.width();
    size_t h = src.height();

    for (size_t y = 0; y < h; ++y)
    {
        glm::vec4* ptr = src[y];

        for (size_t x = 0; x < w; ++x)
        {
            *ptr = rop(*ptr);
            ++ptr;
        }
    }
}

static void rop2n(image &src, const image &img, const std::function<glm::vec4(const glm::vec4&, const glm::vec4&)>& rop)
{
    assert((src.width() == img.width()) && (src.height() == img.height()));

    size_t w = src.width();
    size_t h = src.height();

    for (size_t y = 0; y < h; ++y)
    {
        glm::vec4* srcPtr = src[y];
        const glm::vec4* imgPtr = img[y];

        for (size_t x = 0; x < w; ++x)
        {
            *srcPtr = rop(*srcPtr, *imgPtr);
            ++srcPtr;
            ++imgPtr;
        }
    }
}

static void rop1c(const image &src, const std::function<void(const glm::vec4&)>& rop)
{
    size_t w = src.width();
    size_t h = src.height();

    for (size_t y = 0; y < h; ++y)
    {
        const glm::vec4* ptr = src[y];

        for (size_t x = 0; x < w; ++x)
            rop(*(ptr++));
    }
}

static void rop2c(const image &src, const image &img, const std::function<void(const glm::vec4&, const glm::vec4&)>& rop)
{
    assert((src.width() == img.width()) && (src.height() == img.height()));

    size_t w = src.width();
    size_t h = src.height();

    for (size_t y = 0; y < h; ++y)
    {
        const glm::vec4* srcPtr = src[y];
        const glm::vec4* imgPtr = img[y];

        for (size_t x = 0; x < w; ++x)
            rop(*(srcPtr++), *(imgPtr++));
    }
}

static bool bitmap_to_rgbaf(FIBITMAP* src, FIBITMAP* dst, size_t w, size_t h)
{
    assert(src && dst);
    assert(w + h > 0);

    int bpp = FreeImage_GetBPP(src);
    if ((bpp != 24) && (bpp != 32))
        return false;

    for (size_t y = 0; y < h; ++y)
    {
        uint8_t* srcPtr = (uint8_t*)FreeImage_GetScanLine(src, y);
        FIRGBAF* dstPtr = (FIRGBAF*)FreeImage_GetScanLine(dst, y);

        for (size_t x = 0; x < w; ++x)
        {
            dstPtr->red   = (float)srcPtr[FI_RGBA_RED] / 255.0f;
            dstPtr->green = (float)srcPtr[FI_RGBA_GREEN] / 255.0f;
            dstPtr->blue  = (float)srcPtr[FI_RGBA_BLUE] / 255.0f;
            dstPtr->alpha = 0;

            srcPtr += (bpp / 8);
            dstPtr += 1;
        }
    }

    return true;
}

static bool rgbf_to_rgbaf(FIBITMAP* src, FIBITMAP* dst, size_t w, size_t h)
{
    assert(src && dst);
    assert(w + h > 0);

    for (size_t y = 0; y < h; ++y)
    {
        FIRGBF*  srcPtr = (FIRGBF* )FreeImage_GetScanLine(src, y);
        FIRGBAF* dstPtr = (FIRGBAF*)FreeImage_GetScanLine(dst, y);

        for (size_t x = 0; x < w; ++x)
        {
            dstPtr->red   = srcPtr->red;
            dstPtr->green = srcPtr->green;
            dstPtr->blue  = srcPtr->blue;
            dstPtr->alpha = 0;

            ++srcPtr;
            ++dstPtr;
        }
    }

    return true;
}

image& image::operator=(const image &other)
{
    FreeImage_Unload(this->dib); // delete old image
    if (!(this->dib = FreeImage_Clone(other.dib)))
        throw std::runtime_error("image::=");

    return *this;
}

image::image(size_t width, size_t height)
{
    assert(width + height > 0);

    this->dib = FreeImage_AllocateT(FIT_RGBAF, width, height, 128);
    if (!this->dib) throw std::runtime_error("image::image()");
}

image::image(const std::string& path)
{
    load(path);
}

image::image(const image &other)
{
    if (!(this->dib = FreeImage_Clone(other.dib)))
        throw std::runtime_error("image::image()");
}

image::image(FIBITMAP *dib)
{
    assert(dib);
    this->dib = dib;
}

image::~image()
{
    FreeImage_Unload(this->dib);
}

void image::load(const std::string& path)
{
    FIBITMAP* loaded;

    FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(path.c_str(), 0);
    if(fif == FIF_UNKNOWN) fif = FreeImage_GetFIFFromFilename(path.c_str());
    if ((fif == FIF_UNKNOWN) || !FreeImage_FIFSupportsReading(fif))
        throw std::runtime_error("image::load -> unknown file format");

    if ((loaded = FreeImage_Load(fif, path.c_str(), 0)) == nullptr)
        throw std::runtime_error("image::load -> failed to load file");

    if (!FreeImage_HasPixels(loaded))
    {
        FreeImage_Unload(loaded);
        throw std::runtime_error("image::load -> file has no pixels");
    }

    size_t w = FreeImage_GetWidth(loaded);
    size_t h = FreeImage_GetHeight(loaded);

    FREE_IMAGE_TYPE fit = FreeImage_GetImageType(loaded);
    if (fit == FIT_RGBAF) this->dib = loaded;
    else
    {
        this->dib = FreeImage_AllocateT(FIT_RGBAF, w, h, 128);
        if (!this->dib)
        {
            FreeImage_Unload(loaded);
            throw std::runtime_error("image::load -> allocation error");
        }

        bool success = false;

        if (fit == FIT_BITMAP)
            success = bitmap_to_rgbaf(loaded, this->dib, w, h);
        else if (fit == FIT_RGBF)
            success = rgbf_to_rgbaf(loaded, this->dib, w, h);

        FreeImage_Unload(loaded);

        if (!success)
            throw std::runtime_error("image::load -> bad image type");
    }
}

void image::save(const std::string& path) const
{
    if (!FreeImage_Save(FIF_EXR, this->dib, path.c_str()))
        throw std::runtime_error("image::save");
}

void image::add(const image& other, const channels& which)
{
    glm::vec4 mask((channels::R & which) ? 1.0f : 0.0f,
                   (channels::G & which) ? 1.0f : 0.0f,
                   (channels::B & which) ? 1.0f : 0.0f,
                   0.0f);

    rop2n(*this, other, [mask](const glm::vec4& a, const glm::vec4& b){
        return a + b * mask;
    });
}

void image::sub(const image& other, const channels& which)
{
    glm::vec4 mask((channels::R & which) ? 1.0f : 0.0f,
                   (channels::G & which) ? 1.0f : 0.0f,
                   (channels::B & which) ? 1.0f : 0.0f,
                   0.0f);

    rop2n(*this, other, [mask](const glm::vec4& a, const glm::vec4& b){
        return a - b * mask;
    });
}

void image::mul(const image& other, const channels& which)
{
    glm::vec4 t((channels::R & which) ? 1.0f : 0.0f,
                (channels::G & which) ? 1.0f : 0.0f,
                (channels::B & which) ? 1.0f : 0.0f,
                0.0f);

    rop2n(*this, other, [t](const glm::vec4& a, const glm::vec4& b){
        return (1.0f - t) * a + t * (a * b);
    });
}

void image::fill(const glm::vec4& bkgd, const channels& which)
{
    glm::vec4 t((channels::R & which) ? 1.0f : 0.0f,
                (channels::G & which) ? 1.0f : 0.0f,
                (channels::B & which) ? 1.0f : 0.0f,
                0.0f);

    rop1n(*this, [bkgd, t](const glm::vec4& a){
        return (1.0f - t) * a + t * bkgd;
    });
}

void image::normalize(bool local, const channels& which)
{
    glm::vec4 measure, inv_norm;

    if (!local)
    {
        rop1c(*this, [&measure](const glm::vec4& a) mutable {
            measure += a;
        });
    }
    else
    {
        rop1c(*this, [&measure](const glm::vec4& a) mutable {
            measure = glm::max(measure, a);
        });
    }

    inv_norm.x = (channels::R & which) ? 1.0f / measure.x : 1.0f;
    inv_norm.y = (channels::G & which) ? 1.0f / measure.y : 1.0f;
    inv_norm.z = (channels::B & which) ? 1.0f / measure.z : 1.0f;

    rop1n(*this, [inv_norm](const glm::vec4& a){
        return a * inv_norm;
    });
}

image image::resize(size_t newWidth, size_t newHeight, FREE_IMAGE_FILTER filter) const
{
    assert(newWidth + newHeight > 0);

    FIBITMAP *resized = FreeImage_Rescale(this->dib, newWidth, newHeight, filter);
    if (!resized) throw std::runtime_error("image::resize");

    return image(resized);
}

image image::subregion(size_t rectX, size_t rectY, size_t rectW, size_t rectH) const
{
    assert(rectW + rectH > 0);
    assert(rectX + rectW < width());
    assert(rectY + rectH < height());

    FIBITMAP *region = FreeImage_Copy(this->dib, rectX, rectY, rectX + rectW, rectY + rectH);
    if (!region) throw std::runtime_error("image::subregion");

    return image(region);
}

image image::zero_pad(size_t left, size_t top, size_t right, size_t bottom) const
{
    glm::vec4 black;

    FIBITMAP *padded = FreeImage_EnlargeCanvas(this->dib, left, top, right, bottom, &black);
    if (!padded) throw std::runtime_error("image::zero_pad");

    return image(padded);
}

glm::vec4* image::operator[](size_t y)
{
    assert(y < height());

    auto ptr = (glm::vec4*)FreeImage_GetScanLine(this->dib, y);
    assert(ptr);
    return ptr;
}

const glm::vec4* image::operator[](size_t y) const
{
    assert(y < height());

    auto ptr = (const glm::vec4*)FreeImage_GetScanLine(this->dib, y);
    assert(ptr);
    return ptr;
}

glm::vec4& image::operator()(size_t x, size_t y)
{
    assert((x < width()) && (y < height()));
    return *((*this)[y] + x);
}

const glm::vec4& image::operator()(size_t x, size_t y) const
{
    assert((x < width()) && (y < height()));
    return *((*this)[y] + x);
}

size_t image::width() const
{
    return FreeImage_GetWidth(this->dib);
}

size_t image::height() const
{
    return FreeImage_GetHeight(this->dib);
}
