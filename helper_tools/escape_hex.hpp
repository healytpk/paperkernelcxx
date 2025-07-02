#pragma once

#include <string>
#include <sstream>

inline void Lreplace_non_ascii_with_hex(std::string &str)
{
    std::string result;

    size_t i = 0u;
    while ( i < str.size() )
    {
        unsigned char c = static_cast<unsigned char>(str[i]);
        size_t charLen = 0u;

        if ((c & 0x80) == 0x00)
        {
            // ASCII
            result += c;
            ++i;
            continue;
        }
        else if ((c & 0xE0) == 0xC0)
        {
            charLen = 2; // 2-byte UTF-8 character
        }
        else if ((c & 0xF0) == 0xE0)
        {
            charLen = 3; // 3-byte UTF-8 character
        }
        else if ((c & 0xF8) == 0xF0)
        {
            charLen = 4; // 4-byte UTF-8 character
        }
        else
        {
            // Invalid UTF-8 start byte
            // std::abort();  // Invalid sequence
            ++i;
            continue;
        }

        // Handle multi-byte UTF-8 sequence
        if ( (i + charLen) <= str.size() )
        {
            // Convert to Unicode code point
            unsigned int codePoint = 0;
            for ( size_t j = 0; j < charLen; ++j )
            {
                unsigned char byte = static_cast<unsigned char>(str[i + j]);
                if ( j == 0 )
                {
                    // Handle the first byte
                    if ( charLen == 2 )
                    {
                        codePoint = byte & 0x1F;
                    }
                    else if ( charLen == 3 )
                    {
                        codePoint = byte & 0x0F;
                    }
                    else if ( charLen == 4 )
                    {
                        codePoint = byte & 0x07;
                    }
                }
                else
                {
                    // Handle continuation bytes
                    codePoint = (codePoint << 6) | (byte & 0x3F);
                }
            }

            // Now, generate the correct escape sequence
            if ( codePoint < 0x80 )
            {
                // ASCII character (this should already be handled earlier)
                result += static_cast<char>(codePoint);
            }
            else if ( codePoint <= 0xFFFF )
            {
                // Unicode character (use \uXXXX)
                std::ostringstream oss;
                oss << "\\u" << std::setw(4) << std::setfill('0') << std::hex << std::uppercase << codePoint;
                result += oss.str();
            }
            else
            {
                // Code point exceeds 16-bit (use \UXXXXXXXX)
                std::ostringstream oss;
                oss << "\\U" << std::setw(8) << std::setfill('0') << std::hex << std::uppercase << codePoint;
                result += oss.str();
            }

            i += charLen; // Move to the next character
        }
        else
        {
            // Incomplete UTF-8 sequence, break out
            // std::abort();  // Invalid sequence
            break;
        }
    }

    str = std::move(result); // Update the original string with the result
}
