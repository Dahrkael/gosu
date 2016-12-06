#include <Gosu/Font.hpp>
#include <Gosu/Graphics.hpp>
#include <Gosu/Image.hpp>
#include <Gosu/Math.hpp>
#include <Gosu/Text.hpp>
#include <Gosu/TR1.hpp>
#include "../Graphics/Common.hpp"
#include "FormattedString.hpp"
#include <cassert>
#include <map>
using namespace std;

struct Gosu::Font::Impl
{
    string name;
    unsigned height, flags;

    // Unicode planes of 2^16 characters each. On Windows, where wchar_t is only 16 bits wide, only
    // the first plane will ever be touched.
    struct CharInfo
    {
        GOSU_UNIQUE_PTR<Image> image;
        double factor;
    };
    typedef tr1::array<CharInfo, 65536> Plane;
    GOSU_UNIQUE_PTR<Plane> planes[16][ffCombinations];
    
    map<string, tr1::shared_ptr<Image> > entityCache;
    
    CharInfo& charInfo(wchar_t wc, unsigned flags)
    {
        size_t planeIndex = wc / 65536;
        size_t charIndex  = wc % 65536;
        
        if (planeIndex >= 16)
            throw invalid_argument("Unicode plane out of reach");
        if (flags >= ffCombinations)
            throw invalid_argument("Font flags out of range");
        
        if (!planes[planeIndex][flags].get())
            planes[planeIndex][flags].reset(new Plane);
        return (*planes[planeIndex][flags])[charIndex];
    }
    
    const Image& imageAt(const FormattedString& fs, unsigned i)
    {
        if (fs.entityAt(i))
        {
            tr1::shared_ptr<Image>& ptr = entityCache[fs.entityAt(i)];
            if (!ptr)
                ptr.reset(new Image(entityBitmap(fs.entityAt(i)), ifSmooth));
            return *ptr;
        }
        
        char wc = fs.charAt(i);
        unsigned flags = fs.flagsAt(i);
        CharInfo& info = charInfo(wc, flags);
        
        if (info.image.get())
            return *info.image;
        
        string charString(1, wc);
        // TODO: Would be nice to have.
        // if (isFormattingChar(wc))
        //     charString.clear();
        unsigned charWidth = Gosu::textWidth(charString.c_str(), name.c_str(), height, flags);
        
        Bitmap bitmap(charWidth, height, 0x00ffffff);
        drawText(bitmap, charString.c_str(), 0, 0, Color::WHITE, name.c_str(), height, flags);
        info.image.reset(new Image(bitmap));
        info.factor = 0.5;
        return *info.image;
    }
    
    double factorAt(const FormattedString& fs, unsigned index)
    {
        if (fs.entityAt(index))
            return 1;
        return charInfo(fs.charAt(index), fs.flagsAt(index)).factor;
    }
};

Gosu::Font::Font(unsigned fontHeight, const char* fontName, unsigned fontFlags)
: pimpl(new Impl)
{
    pimpl->name = fontName;
    pimpl->height = fontHeight * 2;
    pimpl->flags = fontFlags;
}

const char* Gosu::Font::name() const
{
    return pimpl->name.c_str();
}

unsigned Gosu::Font::height() const
{
    return pimpl->height / 2;
}

unsigned Gosu::Font::flags() const
{
    return pimpl->flags;
}

double Gosu::Font::textWidth(const char* text, double factorX) const
{
    FormattedString fs(text, flags());
    double result = 0;
    for (unsigned i = 0; i < fs.length(); ++i)
    {
        const Image& image = pimpl->imageAt(fs, i);
        double factor = pimpl->factorAt(fs, i);
        result += image.width() * factor;
    }
    return result * factorX;
}

void Gosu::Font::draw(const char* text, double x, double y, ZPos z,
    double factorX, double factorY, Color c, AlphaMode mode) const
{
    FormattedString fs(text, flags());
    
    for (unsigned i = 0; i < fs.length(); ++i)
    {
        const Image& image = pimpl->imageAt(fs, i);
        double factor = pimpl->factorAt(fs, i);
        Gosu::Color color = fs.entityAt(i)
                          ? Gosu::Color(fs.colorAt(i).alpha() * c.alpha() / 255, 255, 255, 255)
                          : Gosu::multiply(fs.colorAt(i), c);
        image.draw(x, y, z, factorX * factor, factorY * factor, color, mode);
        x += image.width() * factorX * factor;
    }
}

void Gosu::Font::drawRel(const char* text, double x, double y, ZPos z,
    double relX, double relY, double factorX, double factorY, Color c,
    AlphaMode mode) const
{
    x -= textWidth(text) * factorX * relX;
    y -= height() * factorY * relY;
    
    draw(text, x, y, z, factorX, factorY, c, mode);
}

void Gosu::Font::setImage(wchar_t wc, const Image& image)
{
    for (unsigned flags = 0; flags < ffCombinations; ++flags)
        setImage(wc, flags, image);
}

void Gosu::Font::setImage(wchar_t wc, unsigned fontFlags, const Image& image)
{
    Impl::CharInfo& ci = pimpl->charInfo(wc, fontFlags);
    if (ci.image.get())
        throw logic_error("Cannot set image for the same Font character twice or after it has been drawn");
    ci.image.reset(new Gosu::Image(image));
    ci.factor = 1.0;
}

void Gosu::Font::drawRot(const char* text, double x, double y, ZPos z, double angle,
    double factorX, double factorY, Color c, AlphaMode mode) const
{
    Gosu::Graphics::pushTransform(rotate(angle, x, y));
    draw(text, x, y, z, factorX, factorY, c, mode);
    Gosu::Graphics::popTransform();
}

// Deprecated constructors

Gosu::Font::Font(Graphics& graphics, const char* fontName, unsigned fontHeight,
    unsigned fontFlags)
{
    Font(fontHeight, fontName, fontFlags).pimpl.swap(pimpl);
}
