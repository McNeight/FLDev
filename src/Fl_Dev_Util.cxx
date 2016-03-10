/*
    FLDev - FLTK Integrated Development Environment
    Copyright (C) 2005-2016  Philipp Pracht, Georg Potthast, Neil McNeight

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "Fl_Dev_Util.h"

std::string trim(std::string const& source, const char* delims) {
    std::string result(source);
    std::string::size_type index = result.find_last_not_of(delims);
    if (index != std::string::npos)
        result.erase(++index);

    index = result.find_first_not_of(delims);
    if (index != std::string::npos)
        result.erase(0, index);
    else
        result.erase();
    return result;
}

#ifdef _WIN32
char *strsep(char **from, const char *delim) {
    char *s, *dp, *ret;

    if ((s = *from) == NULL)
        return NULL;

    ret = s;
    while (*s != '\0')
    {
        /* loop until the end of s, checking against each delimiting character,
        * if we find a delimiter set **s to '\0' and return our previous token
        * to the user. */
        dp = (char *)delim;
        while (*dp != '\0')
        {
            if (*s == *dp)
            {
                *s = '\0';
                *from = s + 1;
                return ret;
            }
            dp++;
        }
        s++;
    }
    /* end of string case */
    *from = NULL;
    return ret;
}
#endif // _WIN32
