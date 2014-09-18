#include "utils/image.hpp"

#include <functional>
#include <stdexcept>
#include <cassert>
#include <cstring>
#include <vector>
#include <cmath>

static void rop1n(image &src, const std::function<glm::vec4(const glm::vec4&)>& rop)
{
    int w = src.width();
    int h = src.height();

    for (int y = 0; y < h; ++y)
    {
        glm::vec4* ptr = src[y];

        for (int x = 0; x < w; ++x)
        {
            *ptr = rop(*ptr);
            ++ptr;
        }
    }
}

static void rop2n(image &src, const image &img, const std::function<glm::vec4(const glm::vec4&, const glm::vec4&)>& rop)
{
    assert((src.width() == img.width()) && (src.height() == img.height()));

    int w = src.width();
    int h = src.height();

    for (int y = 0; y < h; ++y)
    {
        glm::vec4* srcPtr = src[y];
        const glm::vec4* imgPtr = img[y];

        for (int x = 0; x < w; ++x)
        {
            *srcPtr = rop(*srcPtr, *imgPtr);
            ++srcPtr;
            ++imgPtr;
        }
    }
}

static void rop1c(const image &src, const std::function<void(const glm::vec4&)>& rop)
{
    int w = src.width();
    int h = src.height();

    for (int y = 0; y < h; ++y)
    {
        const glm::vec4* ptr = src[y];

        for (int x = 0; x < w; ++x)
            rop(*(ptr++));
    }
}

#if 0
static void rop2c(const image &src, const image &img, const std::function<void(const glm::vec4&, const glm::vec4&)>& rop)
{
    assert((src.width() == img.width()) && (src.height() == img.height()));

    int w = src.width();
    int h = src.height();

    for (int y = 0; y < h; ++y)
    {
        const glm::vec4* srcPtr = src[y];
        const glm::vec4* imgPtr = img[y];

        for (int x = 0; x < w; ++x)
            rop(*(srcPtr++), *(imgPtr++));
    }
}
#endif

static bool bitmap_to_rgbaf(FIBITMAP* src, FIBITMAP* dst, int w, int h)
{
    assert(src && dst);
    assert(w + h > 0);

    int bpp = FreeImage_GetBPP(src);
    if ((bpp != 8) && (bpp != 24) && (bpp != 32))
        return false;

    RGBQUAD* palette = FreeImage_GetPalette(src);

    for (int y = 0; y < h; ++y)
    {
        uint8_t* srcPtr = (uint8_t*)FreeImage_GetScanLine(src, y);
        FIRGBAF* dstPtr = (FIRGBAF*)FreeImage_GetScanLine(dst, y);

        for (int x = 0; x < w; ++x)
        {
            if ((bpp == 24) || (bpp == 32))
            {
                dstPtr->red   = (float)srcPtr[FI_RGBA_RED] / 255.0f;
                dstPtr->green = (float)srcPtr[FI_RGBA_GREEN] / 255.0f;
                dstPtr->blue  = (float)srcPtr[FI_RGBA_BLUE] / 255.0f;
            }
            else
            {
                dstPtr->red   = (float)palette[*srcPtr].rgbRed / 255.0f;
                dstPtr->green = (float)palette[*srcPtr].rgbGreen / 255.0f;
                dstPtr->blue  = (float)palette[*srcPtr].rgbBlue / 255.0f;
            }

            dstPtr->alpha = 0;

            srcPtr += (bpp / 8);
            dstPtr += 1;
        }
    }

    return true;
}

static bool rgbf_to_rgbaf(FIBITMAP* src, FIBITMAP* dst, int w, int h)
{
    assert(src && dst);
    assert(w + h > 0);

    for (int y = 0; y < h; ++y)
    {
        FIRGBF*  srcPtr = (FIRGBF* )FreeImage_GetScanLine(src, y);
        FIRGBAF* dstPtr = (FIRGBAF*)FreeImage_GetScanLine(dst, y);

        for (int x = 0; x < w; ++x)
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

image::image(int width, int height, GLuint tex)
{
    std::vector<glm::vec4> bits;
    bits.resize(width * height);
    glm::vec4* ptr = &bits[0];

    glBindTexture(GL_TEXTURE_2D, tex);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, ptr);

    this->dib = FreeImage_AllocateT(FIT_RGBAF, width, height, 128);
    if (!this->dib) {
        LOG(ERROR) << "Failed to allocate memory for image.";
        throw 0;
    }

    for (int y = 0; y < height; ++y)
    {
        glm::vec4* dst = (*this)[y];

        memcpy(dst, ptr, sizeof(glm::vec4) * width);

        ptr += width;
    }
}

image& image::operator=(const image &other)
{
    FreeImage_Unload(this->dib); // delete old image
    if (!(this->dib = FreeImage_Clone(other.dib))) {
        LOG(ERROR) << "Failed to clone image.";
        throw 0;
    }

    return *this;
}

image::image(int width, int height)
{
    assert(width + height > 0);

    this->dib = FreeImage_AllocateT(FIT_RGBAF, width, height, 128);
    if (!this->dib) {
        LOG(ERROR) << "Failed to allocate memory for image.";
        throw 0;
    }
}

image::image(const std::string& path)
{
    load(path);
}

image::image(const image &other)
{
    if (!(this->dib = FreeImage_Clone(other.dib))) {
        LOG(ERROR) << "Failed to clone image.";
        throw 0;
    }
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
    if ((fif == FIF_UNKNOWN) || !FreeImage_FIFSupportsReading(fif)) {
        LOG(ERROR) << "'" << path << "': unknown file format.";
        throw 0;
    }

    if ((loaded = FreeImage_Load(fif, path.c_str(), 0)) == nullptr) {
        LOG(ERROR) << "'" << path << "': failed to load file.";
        throw 0;
    }

    if (!FreeImage_HasPixels(loaded))
    {
        FreeImage_Unload(loaded);
        LOG(ERROR) << "'" << path << "': file has no pixels.";
        throw 0;
    }

    int w = FreeImage_GetWidth(loaded);
    int h = FreeImage_GetHeight(loaded);

    FREE_IMAGE_TYPE fit = FreeImage_GetImageType(loaded);
    if (fit == FIT_RGBAF) this->dib = loaded;
    else
    {
        this->dib = FreeImage_AllocateT(FIT_RGBAF, w, h, 128);
        if (!this->dib)
        {
            FreeImage_Unload(loaded);
            LOG(ERROR) << "Failed to allocate memory for image.";
            throw 0;
        }

        bool success = false;

        if (fit == FIT_BITMAP)
            success = bitmap_to_rgbaf(loaded, this->dib, w, h);
        else if (fit == FIT_RGBF)
            success = rgbf_to_rgbaf(loaded, this->dib, w, h);

        FreeImage_Unload(loaded);

        if (!success) {
            LOG(ERROR) << "'" << path << "': bad image type.";
            throw 0;
        }
    }

    assert(FreeImage_GetLine(dib) == FreeImage_GetPitch(dib));
}

void image::save(const std::string& path) const
{
    if (!FreeImage_Save(FIF_EXR, this->dib, path.c_str())) {
        LOG(ERROR) << "'" << path << "': failed to save.";
        throw 0;
    }
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

void image::grayscale()
{
    rop1n(*this, [](const glm::vec4& a){
        float avg = (a.x + a.y + a.z) / 3.0f;
        return glm::vec4(avg, avg, avg, 0.0f);
    });
}

void image::colorize(const glm::vec4& color)
{
    rop1n(*this, [color](const glm::vec4& a){
        return a * color;
    });
}

void image::reproduce(const channels& which)
{
    assert((which == channels::R)
        || (which == channels::G)
        || (which == channels::B));

    if (which == channels::R)
    {
        rop1n(*this, [](const glm::vec4& a){
            return glm::vec4(a.x, a.x, a.x, 0.0f);
        });
    }
    else if (which == channels::G)
    {
        rop1n(*this, [](const glm::vec4& a){
            return glm::vec4(a.y, a.y, a.y, 0.0f);
        });
    }
    else if (which == channels::B)
    {
        rop1n(*this, [](const glm::vec4& a){
            return glm::vec4(a.z, a.z, a.z, 0.0f);
        });
    }
}

image image::compose(const image& r, const image& g, const image& b)
{
    assert((r.width() == g.width()) && (r.height() == g.height()));
    assert((g.width() == b.width()) && (g.height() == b.height()));
    assert((b.width() == r.width()) && (b.height() == r.height()));

    image out(r.width(), r.height());
    int w = r.width(), h = r.height();

    for (int y = 0; y < h; ++y)
    {
        glm::vec4* dstPtr = out[y];
        const glm::vec4* r_ptr = r[y];
        const glm::vec4* g_ptr = g[y];
        const glm::vec4* b_ptr = b[y];

        for (int x = 0; x < w; ++x)
        {
            dstPtr->r = r_ptr->x;
            dstPtr->g = g_ptr->x;
            dstPtr->b = b_ptr->x;
            dstPtr->w = 0;

            ++dstPtr;
            ++r_ptr;
            ++g_ptr;
            ++b_ptr;
        }
    }

    return out;
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

image image::resize(int newWidth, int newHeight, FREE_IMAGE_FILTER filter) const
{
    assert(newWidth + newHeight > 0);

    FIBITMAP *resized = FreeImage_Rescale(this->dib, newWidth, newHeight, filter);
    if (!resized) {
        LOG(ERROR) << "Failed to resize image.";
        throw 0;
    }

    return image(resized);
}

image image::enlarge(int newWidth, int newHeight) const
{
    assert((newWidth >= width()) && (newHeight >= height()));
    assert(newWidth + newHeight > 0);

    int padL = (newWidth - width()) / 2;
    int padR = newWidth - width() - padL;
    int padT = (newHeight - height()) / 2;
    int padB = newHeight - height() - padT;

    return zero_pad(padL, padT, padR, padB);
}

image image::subregion(int rectX, int rectY, int rectW, int rectH) const
{
    assert(rectW + rectH > 0);
    assert(rectX + rectW <= width());
    assert(rectY + rectH <= height());

    FIBITMAP *region = FreeImage_Copy(this->dib, rectX, rectY, rectX + rectW, rectY + rectH);
    if (!region) {
        LOG(ERROR) << "Failed to take image subregion.";
        throw 0;
    }

    return image(region);
}

image image::zero_pad(int left, int top, int right, int bottom) const
{
    glm::vec4 black;

    FIBITMAP *padded = FreeImage_EnlargeCanvas(this->dib, left, top, right, bottom, &black);
    if (!padded) {
        LOG(ERROR) << "Failed to zero-pad image.";
        throw 0;
    }

    return image(padded);
}

glm::vec4* image::operator[](int y)
{
    assert(y < height());

    auto ptr = (glm::vec4*)FreeImage_GetScanLine(this->dib, y);
    assert(ptr);
    return ptr;
}

const glm::vec4* image::operator[](int y) const
{
    assert(y < height());

    auto ptr = (const glm::vec4*)FreeImage_GetScanLine(this->dib, y);
    assert(ptr);
    return ptr;
}

glm::vec4& image::operator()(int x, int y)
{
    assert((x < width()) && (y < height()));
    return *((*this)[y] + x);
}

const glm::vec4& image::operator()(int x, int y) const
{
    assert((x < width()) && (y < height()));
    return *((*this)[y] + x);
}

glm::vec4* image::data()
{
    return (*this)[0];
}

const glm::vec4* image::data() const
{
    return (*this)[0];
}

int image::width() const
{
    return FreeImage_GetWidth(this->dib);
}

int image::height() const
{
    return FreeImage_GetHeight(this->dib);
}

image utils::draw_circle(int radius, bool anti_alias, const glm::vec4& color)
{
    if (anti_alias) radius *= 2;
    image circle(2 * radius, 2 * radius);
    for (int y = 0; y < 2 * radius; ++y)
    {
        glm::vec4* ptr = circle[y];

        for (int x = 0; x < 2 * radius; ++x)
        {
            float px = ((float)x - radius) / (radius);
            float py = ((float)y - radius) / (radius);

            if (px * px + py * py <= 1.0f)
                *ptr = color;

            ++ptr;
        }
    }

    if (anti_alias) circle = circle.resize(radius, radius);

    return circle;
}
