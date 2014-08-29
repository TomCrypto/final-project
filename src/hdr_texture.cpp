#include "hdr_texture.hpp"

#include <stdexcept>
#include <cassert>

static bool bitmap_to_rgbaf(FIBITMAP* src, FIBITMAP* dst, size_t w, size_t h)
{
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

hdr_texture& hdr_texture::operator=(const hdr_texture &other)
{
    if (!(this->dib = FreeImage_Clone(other.dib)))
        throw std::runtime_error("hdr_texture::=");

    return *this;
}

hdr_texture::hdr_texture(size_t width, size_t height)
{
    this->dib = FreeImage_AllocateT(FIT_RGBAF, width, height, 128);
    if (!this->dib) throw std::runtime_error("hdr_texture()");
}

hdr_texture::hdr_texture(const hdr_texture &other)
{
    if (!(this->dib = FreeImage_Clone(other.dib)))
        throw std::runtime_error("hdr_texture::=");
}

hdr_texture::hdr_texture(const std::string& path)
{
    load(path);
}

hdr_texture::hdr_texture(FIBITMAP *dib)
{
    this->dib = dib;
}

hdr_texture::~hdr_texture()
{
    FreeImage_Unload(this->dib);
}

void hdr_texture::load(const std::string& path)
{
    FIBITMAP* loaded;

    FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(path.c_str(), 0);
    if(fif == FIF_UNKNOWN) fif = FreeImage_GetFIFFromFilename(path.c_str());
    if ((fif == FIF_UNKNOWN) || !FreeImage_FIFSupportsReading(fif))
        throw std::runtime_error("hdr_texture::load -> unknown file type");

    if ((loaded = FreeImage_Load(fif, path.c_str(), 0)) == nullptr)
        throw std::runtime_error("hdr_texture::load -> failed to load file");

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
            throw std::runtime_error("hdr_texture::load -> allocation error");
        }

        bool success = false;

        if (fit == FIT_BITMAP)
            success = bitmap_to_rgbaf(loaded, this->dib, w, h);
        else if (fit == FIT_RGBF)
            success = rgbf_to_rgbaf(loaded, this->dib, w, h);

        FreeImage_Unload(loaded);

        if (!success)
            throw std::runtime_error("hdr_texture::load -> bad format");
    }
}

void hdr_texture::save(const std::string& path) const
{
    if (!FreeImage_Save(FIF_EXR, this->dib, path.c_str()))
        throw std::runtime_error("hdr_texture::save");
}

void hdr_texture::add(const hdr_texture& other, const channels& which)
{
    size_t w = width();
    size_t h = height();

    assert((other.width() == w) && (other.height() == h));

    for (size_t y = 0; y < h; ++y)
    {
        const glm::vec4* srcPtr = other[y];
        glm::vec4* dstPtr = (*this)[y];
        
        for (size_t x = 0; x < w; ++x)
        {
            if (channels::R & which != 0)
                dstPtr->x += srcPtr->x;
            if (channels::G & which != 0)
                dstPtr->y += srcPtr->y;
            if (channels::B & which != 0)
                dstPtr->z += srcPtr->z;

            ++srcPtr;
            ++dstPtr;
        }
    }
}

void hdr_texture::sub(const hdr_texture& other, const channels& which)
{
    size_t w = width();
    size_t h = height();

    assert((other.width() == w) && (other.height() == h));

    for (size_t y = 0; y < h; ++y)
    {
        const glm::vec4* srcPtr = other[y];
        glm::vec4* dstPtr = (*this)[y];
        
        for (size_t x = 0; x < w; ++x)
        {
            if (channels::R & which != 0)
                dstPtr->x -= srcPtr->x;
            if (channels::G & which != 0)
                dstPtr->y -= srcPtr->y;
            if (channels::B & which != 0)
                dstPtr->z -= srcPtr->z;

            ++srcPtr;
            ++dstPtr;
        }
    }
}

void hdr_texture::normalize(const channels& which)
{
    size_t w = width();
    size_t h = height();

    glm::vec4 total;

    for (size_t y = 0; y < h; ++y)
    {
        glm::vec4* ptr = (*this)[y];

        for (size_t x = 0; x < w; ++x)
            total += *(ptr++);
    }

    if (channels::R & which != 0)
        total.x = 1.0f / total.x;
    if (channels::G & which != 0)
        total.y = 1.0f / total.y;
    if (channels::B & which != 0)
        total.z = 1.0f / total.z;

    for (size_t y = 0; y < h; ++y)
    {
        glm::vec4* ptr = (*this)[y];

        for (size_t x = 0; x < w; ++x)
        {
            if (channels::R & which != 0)
                ptr->x *= total.x;
            if (channels::G & which != 0)
                ptr->y *= total.y;
            if (channels::B & which != 0)
                ptr->z *= total.z;

            ++ptr;
        }
    }
}

void hdr_texture::resize(size_t newWidth, size_t newHeight, FREE_IMAGE_FILTER filter)
{
    FIBITMAP *resized = FreeImage_Rescale(this->dib, newWidth, newHeight, filter);
    if (!resized) throw std::runtime_error("hdr_texture::resized -> memory error");

    FreeImage_Unload(this->dib);
    this->dib = resized;
}

glm::vec4* hdr_texture::operator[](size_t y)
{
    assert(y < height());

    auto ptr = (glm::vec4*)FreeImage_GetScanLine(this->dib, y);
    if (!ptr) throw std::runtime_error("hdr_texture::[]");
    return ptr;
}

const glm::vec4* hdr_texture::operator[](size_t y) const
{
    assert(y < height());

    auto ptr = (const glm::vec4*)FreeImage_GetScanLine(this->dib, y);
    if (!ptr) throw std::runtime_error("hdr_texture::[]");
    return ptr;
}

glm::vec4& hdr_texture::operator()(size_t x, size_t y)
{
    assert((x < width()) && (y < height()));
    return *((*this)[y] + x);
}

const glm::vec4& hdr_texture::operator()(size_t x, size_t y) const
{
    assert((x < width()) && (y < height()));
    return *((*this)[y] + x);
}

size_t hdr_texture::width() const
{
    return FreeImage_GetWidth(this->dib);
}

size_t hdr_texture::height() const
{
    return FreeImage_GetHeight(this->dib);
}
