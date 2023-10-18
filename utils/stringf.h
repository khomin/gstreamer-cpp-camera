#ifndef STRINGF_H
#define STRINGF_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <string>
#include <exception>

using namespace std;

class StringFormatter {
public:
    static string format(const char *format, ...)
    {
        va_list  argptr;

        va_start(argptr, format);

            char   *ptr;
            size_t  size;
            FILE   *fp_mem = open_memstream(&ptr, &size);
            assert(fp_mem);

            vfprintf (fp_mem, format, argptr);
            fclose (fp_mem);

        va_end(argptr);

        string ret = ptr;
        free(ptr);

        return ret;
    }
};

#endif // STRINGF_H
