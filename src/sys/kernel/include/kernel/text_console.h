#pragma once

namespace kernel {
namespace device {
/// A console capable of recieving ASCII text and outputting it to the screen.
/// Each recieved character in the string will be output to the console.
/// The console will also be able to interpret control codes such as `\n` and `\b` correctly.
class text_console {
   public:
    /// Clear the console
    virtual void clear() = 0;
    /// Write a character to the console
    virtual void write(char c) = 0;
    
    /// Return the width in characters of the device
    virtual int width() = 0;
    /// Returns the height in characters of the device 
    virtual int height() = 0;

    /// Returns true if color is supported in this terminal.
    virtual bool supports_color() = 0;
    /// Write a character to the console in color, if supported
    virtual void write_color(char c, char color) = 0;

    /// Returns true if a 'cursor' is supported in this terminal.
    virtual bool supports_cursor_position() = 0;
    /// Sets the X cursor position
    virtual void setX(int x) = 0;
    /// Sets the Y cursor position
    virtual void setY(int x) = 0;
    /// Gets the X cursor position
    virtual int getX() = 0;
    /// Gets the Y cursor position
    virtual int getY() = 0;
};

}
}