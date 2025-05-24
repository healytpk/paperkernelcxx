#include "paper.hpp"
#include <cctype>                           // isdigit, tolower
#include <stdexcept>                        // runtime_error

Paper::Paper(std::string_view const p) noexcept(false)
{
    using std::isdigit, std::tolower;

    do
    {
        if ( 'p' != tolower(p[0]) ) break;
        if (    !isdigit(p[1])    ) break;
        if (    !isdigit(p[2])    ) break;
        if (    !isdigit(p[3])    ) break;
        if (    !isdigit(p[4])    ) break;
        num  = (p[1]-'0') * 1000u;
        num += (p[2]-'0') *  100u;
        num += (p[3]-'0') *   10u;
        num += (p[4]-'0') *    1u;
        if ( 'r' != tolower(p[5]) ) break;
        if (    !isdigit(p[6])    ) break;
        rev =  (p[6]-'0') *    1u;

        if ( isdigit(p[7]) )
        {
            rev *= 10u;
            rev += (p[7]-'0');
            if ( ('\0' != p[8]) && ('.' != p[8]) ) break;
            // fall through
        }
        else if ( ('\0' == p[7]) || ('.' == p[7]) )
        {
            // fall through
        }
        else
        {
            break;
        }
        
        return;
    } while (false);

    throw std::runtime_error("invalid paper name");
}

char const *Paper::c_str(void) const noexcept
{
    static thread_local char s[] = "PxxxxRxx";

    s[1] = '0' + num / 1000u % 10u;
    s[2] = '0' + num /  100u % 10u;
    s[3] = '0' + num /   10u % 10u;
    s[4] = '0' + num /    1u % 10u;

    if ( rev < 10u )
    {
        s[6] = '0' + rev;
        s[7] = '\0';
    }
    else
    {
        s[6] = '0' + rev / 10u % 10u;
        s[7] = '0' + rev / 1u % 10u;
        s[8] = '\0';
    }

    return s;
}
