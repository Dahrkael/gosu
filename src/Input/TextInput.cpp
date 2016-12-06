#include <Gosu/TextInput.hpp>
#include <Gosu/Input.hpp>
#include <Gosu/Platform.hpp>
#include <Gosu/Utility.hpp>
#include <SDL.h>
#include <cwctype>

struct Gosu::TextInput::Impl
{
    // These two strings contain UTF-8 data.
    // See this Wiki page for an overview of what is going on here:
    // http://wiki.libsdl.org/Tutorials/TextInput#CandidateList
    std::string text, composition;
    unsigned caretPos, selectionStart;
    
    Impl()
    : caretPos(0), selectionStart(0)
    {
    }
    
    void insertText(const char* newText)
    {
		std::string newText2 = std::string(newText);
        // Stop IME composition.
        composition.clear();

        // Delete (overwrite) previous selection.
        if (caretPos != selectionStart) {
            unsigned min = std::min(caretPos, selectionStart);
            unsigned max = std::max(caretPos, selectionStart);
            text.erase(text.begin() + min, text.begin() + max);
            caretPos = selectionStart = min;
        }
        
        text.insert(text.begin() + caretPos, newText2.begin(), newText2.end());
        caretPos += newText2.size();
        selectionStart = caretPos;
    }
    
    void moveLeft(bool modifySelection)
    {
        if (caretPos > 0)
            caretPos -= 1;
        
        if (modifySelection)
            selectionStart = caretPos;
    }
    
    void moveRight(bool modifySelection)
    {
        if (caretPos < text.length())
            caretPos += 1;
        
        if (modifySelection)
            selectionStart = caretPos;
    }
    
    void moveWordLeft(bool modifySelection)
    {
        if (caretPos == text.length())
            --caretPos;
        
        while (caretPos > 0 && std::iswspace(text.at(caretPos - 1)))
            --caretPos;
        
        while (caretPos > 0 && !std::iswspace(text.at(caretPos - 1)))
            --caretPos;
        
        if (modifySelection)
            selectionStart = caretPos;
    }

    void moveWordRight(bool modifySelection)
    {
        while (caretPos < text.length() && std::iswspace(text.at(caretPos)))
            ++caretPos;
        
        while (caretPos < text.length() && !std::iswspace(text.at(caretPos)))
            ++caretPos;
        
        if (modifySelection)
            selectionStart = caretPos;
    }
    
    void moveToBeginningOfLine(bool modifySelection)
    {
        caretPos = 0;
        
        if (modifySelection)
            selectionStart = caretPos;
    }
    
    void moveToEndOfLine(bool modifySelection)
    {
        caretPos = static_cast<unsigned>(text.length());
        
        if (modifySelection)
            selectionStart = caretPos;
    }
    
    void deleteBackward()
    {
        if (selectionStart != caretPos) {
            unsigned min = std::min(caretPos, selectionStart);
            unsigned max = std::max(caretPos, selectionStart);
            text.erase(text.begin() + min, text.begin() + max);
            selectionStart = caretPos = min;
        }
        else if (caretPos > 0) {
            unsigned oldCaret = caretPos;
            caretPos -= 1;
            text.erase(text.begin() + caretPos, text.begin() + oldCaret);
            selectionStart = caretPos;
        }
    }
    
    void deleteForward()
    {
        if (selectionStart != caretPos) {
            unsigned min = std::min(caretPos, selectionStart);
            unsigned max = std::max(caretPos, selectionStart);
            text.erase(text.begin() + min, text.begin() + max);
            selectionStart = caretPos = min;
        }
        else if (caretPos < text.length()) {
            unsigned oldCaret = caretPos;
            caretPos += 1;
            text.erase(text.begin() + oldCaret, text.begin() + caretPos);
            selectionStart = caretPos = oldCaret;
        }
        
    }
};

Gosu::TextInput::TextInput()
: pimpl(new Impl)
{
}

Gosu::TextInput::~TextInput()
{
}

const char* Gosu::TextInput::text() const
{
    std::string composedText = pimpl->text;
    if (! pimpl->composition.empty()) {
        composedText.insert(pimpl->caretPos, pimpl->composition);
    }
    return pimpl->text.c_str();
}

void Gosu::TextInput::setText(const char* text)
{
	pimpl->text = text;
    pimpl->composition.clear();
	pimpl->caretPos = pimpl->selectionStart = static_cast<unsigned>(std::string(text).length());
}

unsigned Gosu::TextInput::caretPos() const
{
    return static_cast<unsigned>(pimpl->caretPos);
}

void Gosu::TextInput::setCaretPos(unsigned pos)
{
    pimpl->caretPos = pos;
}

unsigned Gosu::TextInput::selectionStart() const
{
    return static_cast<unsigned>(pimpl->selectionStart);
}

void Gosu::TextInput::setSelectionStart(unsigned pos)
{
    pimpl->selectionStart = pos;
}

bool Gosu::TextInput::feedSDLEvent(void* event)
{
    const SDL_Event* e = static_cast<SDL_Event*>(event);
    
    switch (e->type) {
        // Direct text input, and sent after IME composition completes.
        case SDL_TEXTINPUT: {
            std::string textToInsert = e->text.text;
            textToInsert = filter(textToInsert.c_str());
            pimpl->insertText(textToInsert.c_str());
            return true;
        }
        // IME composition in progress.
        case SDL_TEXTEDITING: {
            pimpl->composition = e->edit.text;
            return true;
        }
        // Emulate "standard" Windows/X11 keyboard behavior.
        case SDL_KEYDOWN: {
            // ...but not if the IME is currently compositing.
            if (! pimpl->composition.empty()) {
                return false;
            }
            
        #ifdef GOSU_IS_MAC
            bool words = (e->key.keysym.mod & (KMOD_LALT | KMOD_RALT));
            bool commandDown = (e->key.keysym.mod & (KMOD_LGUI | KMOD_RGUI));
        #else
            bool words = (e->key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL));
        #endif
            bool shiftDown = (e->key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT));
            SDL_Keycode key = e->key.keysym.sym;
            
            switch (key) {
                case SDLK_LEFT:
                #ifdef GOSU_IS_MAC
                    if (commandDown)
                        pimpl->moveToBeginningOfLine(! shiftDown);
                    else
                #endif
                    if (words)
                        pimpl->moveWordLeft(! shiftDown);
                    else
                        pimpl->moveLeft(! shiftDown);
                    return true;
                case SDLK_RIGHT:
                #ifdef GOSU_IS_MAC
                    if (commandDown)
                        pimpl->moveToEndOfLine(! shiftDown);
                    else
                #endif
                    if (words)
                        pimpl->moveWordRight(! shiftDown);
                    else
                        pimpl->moveRight(! shiftDown);
                    return true;
            #ifdef GOSU_IS_MAC
                case SDLK_UP:
            #endif
                case SDLK_HOME:
                    pimpl->moveToBeginningOfLine(! shiftDown);
                    return true;
            #ifdef GOSU_IS_MAC
                case SDLK_DOWN:
            #endif
                case SDLK_END:
                    pimpl->moveToEndOfLine(! shiftDown);
                    return true;
                case SDLK_BACKSPACE:
                    pimpl->deleteBackward();
                    return true;
                case SDLK_DELETE:
                    pimpl->deleteForward();
                    return true;
            }
            break;
        }
            
        // TODO: Handle copy & paste.
    }
    
    return false;
}
