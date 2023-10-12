//--------------------------------------------------------------------------------------------------
//
//	By Karol Grzybowski, 2017
//
//--------------------------------------------------------------------------------------------------

#include "common.h"
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <limits.h>

const char rtl_string_digits_lowercase[] = "0123456789abcdefghijklmnopqrstuvwxyz";
const char rtl_string_digits_uppercase[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

#define RTL_MIN(_X, _Y)         (((_X) > (_Y)) ? (_Y) : (_X))
#define RTL_MAX(_X, _Y)         (((_X) > (_Y)) ? (_X) : (_Y))
#define RTL_MAKE_FLAG(_Index)                   (1U << (_Index))
#define RTL_FLAGS_SET(_Value, _Mask)            ((_Value) | (_Mask))
#define RTL_FLAGS_REF_SET(_Reference, _Mask)    (((_Reference) |= (_Mask)))
#define RTL_FLAGS_UNSET(_Value, _Mask)          ((_Value) & (~(_Mask)))
#define RTL_FLAGS_REF_UNSET(_Reference, _Mask)  (((_Reference) &= ~(_Mask)))
#define RTL_FLAGS_HAS(_Value, _Mask)            (((_Value) & (_Mask)) == (_Mask))
#define RTL_FLAGS_ANY(_Value, _Mask)            (((_Value) & (_Mask)) != 0)
#define RTL_FLAGS_NONE(_Value, _Mask)           (((_Value) & (_Mask)) == 0)


#define FORMAT_FLAGS_NONE                   (0)
#define FORMAT_FLAGS_MINUS                  (1 << 0)
#define FORMAT_FLAGS_PLUS                   (1 << 1)
#define FORMAT_FLAGS_ALT                    (1 << 2)
#define FORMAT_FLAGS_SPACE                  (1 << 3)
#define FORMAT_FLAGS_ZERO                   (1 << 4)
#define FORMAT_FLAGS_DONE                   (1 << 5)
#define FORMAT_FLAGS_LOWERCASE              (1 << 6)
#define FORMAT_FLAGS_WIDE                   (1 << 7)

//
// Fundamental C integer types.
//

#define FORMAT_TYPE_INT                     (0)
#define FORMAT_TYPE_CHAR                    (1)
#define FORMAT_TYPE_SHORT                   (2)
#define FORMAT_TYPE_LONG                    (3)
#define FORMAT_TYPE_LONGLONG                (4)

#define FORMAT_TYPE_LONGDOUBLE              (5)

#define FORMAT_TYPE_UNSIGNED                (1U << 7)

#define FORMAT_TYPE_NONE                    FORMAT_TYPE_INT

#if defined(__x86_64__)

#define FORMAT_TYPE_INTMAX                  FORMAT_TYPE_LONG
#define FORMAT_TYPE_PTRDIFF                 FORMAT_TYPE_LONG
#define FORMAT_TYPE_SIZE                    FORMAT_TYPE_LONG | FORMAT_TYPE_UNSIGNED
#define FORMAT_TYPE_UINTPTR                 FORMAT_TYPE_LONG | FORMAT_TYPE_UNSIGNED

#elif defined(__aarch64__)

#define FORMAT_TYPE_INTMAX                  FORMAT_TYPE_LONG
#define FORMAT_TYPE_PTRDIFF                 FORMAT_TYPE_LONG
#define FORMAT_TYPE_SIZE                    FORMAT_TYPE_LONG | FORMAT_TYPE_UNSIGNED
#define FORMAT_TYPE_UINTPTR                 FORMAT_TYPE_LONG | FORMAT_TYPE_UNSIGNED

#else

#error "Unknown architecture."

#endif

//
// Format state.
//
struct format_state {
    //
    // Integral number base.
    //
    uint32_t format_base;

    //
    // Format flags.
    //
    uint32_t format_flags;

    //
    // Format specifier type.
    //
    uint32_t format_type;

    //
    // Format specifier width.
    //
    int32_t format_width;

    //
    // Format specifier precision.
    //
    int32_t format_precision;

    //
    // Buffer.
    //
    char* buffer;

    //
    // Buffer capacity.
    //
    size_t buffer_capacity;

    //
    // Number of total characters processed.
    //
    size_t processed;

    //
    // Number of characters processed by current handler.
    //
    size_t current;

    //
    // arglist iterator.
    //
    va_list arglist;
};

// Disable call to stdlib function (don't remember which one :D)
__attribute__((__optimize__("-fno-tree-loop-distribute-patterns")))
static int
rtl_string_to_int32(
    int32_t* result,
    const char* string,
    char** end_string,
    int base
)
{
    if (result == NULL || string == NULL || end_string == NULL)
    {
        return -1;
    }

    //
    // Check integer base range.
    //
    if (base < 0 || base == 1 || base > 36)
    {
        (*end_string) = (char*)string;
        (*result) = 0;
        return -1;
    }

    const char* it = string;
    int character;

    //
    // Skip whitespaces.
    //
    do
    {
        character = (int)(unsigned char)(*it++);
    } while (character == ' ');

    //
    // Check potential sign of value.
    //
    int is_negative;
    if (character == '-')
    {
        is_negative = 1;
        character = (int)(unsigned char)(*it++);
    }
    else
    {
        is_negative = 0;
        if (character == '+')
        {
            character = (int)(unsigned char)(*it++);
        }
    }

    //
    // Check string prefix.
    //
    if ((base == 0 || base == 16) && (character == '0' && (*it == 'x' || *it == 'X')))
    {
        //
        // This is hex string.
        //
        character = (int)(unsigned char)it[1];
        it += 2;
        base = 16;
    }
    else if ((base == 0 || base == 2) && (character == '0' && (*it == 'b' || *it == 'B')))
    {
        //
        // Binary string.
        //
        character = (int)(unsigned char)it[1];
        it += 2;
        base = 2;
    }

    //
    // Default to decimal or octal strings.
    //
    if (base == 0)
    {
        base = (character == '0') ? 8 : 10;
    }

    //
    // Compute string cutoff bounds.
    //
    int32_t cutoff = (is_negative ? INT32_MIN : INT32_MAX);
    int32_t cutlim = cutoff % base;
    cutoff /= base;

    //
    // Adjust limits for negative ranges.
    //
    if (is_negative)
    {
        if (cutlim > 0)
        {
            cutlim -= base;
            cutoff += 1;
        }
        cutlim = -cutlim;
    }

    int32_t accumulator = 0;
    int direction = 0;

    int status = 0;

    //
    // Parse string.
    //
    for (;; character = (int)(unsigned char)(*it++))
    {
        if ('0' <= character && character <= '9')
        {
            character -= '0';
        }
        else if ('a' <= character && character <= 'z')
        {
            character -= 'a' - 10;
        }
        else if ('A' <= character && character <= 'Z')
        {
            character -= 'A' - 10;
        }
        else
        {
            //
            // Invalid string range.
            //
            break;
        }

        if (character >= base)
        {
            //
            // Disallow digits larger than base.
            //
            break;
        }

        if (direction < 0)
        {
            //
            // Skip parsing until end of invalid / out of range number.
            //
            continue;
        }

        if (is_negative)
        {
            if (accumulator < cutoff || (accumulator == cutoff && character > cutlim))
            {
                //
                // Value out of valid range.
                //
                direction = -1;
                accumulator = INT32_MIN;
                status = -1;
            }
            else
            {
                //
                // Update accumulator.
                //
                direction = 1;
                accumulator *= base;
                accumulator -= character;
            }
        }
        else
        {
            if (accumulator > cutoff || (accumulator == cutoff && character > cutlim))
            {
                //
                // Value ot of valid range.
                //
                direction = -1;
                accumulator = INT32_MAX;
                status = -1;
            }
            else
            {
                //
                // Update accumulator.
                //
                direction = 1;
                accumulator *= base;
                accumulator += character;
            }
        }
    }

    //
    // Set result arguments.
    //
    (*end_string) = (char*)(direction ? (it - 1) : string);
    (*result) = accumulator;

    return status;
}

//
// Emits single character to format buffer.
//
static inline
void
rtlp_format_emit_char(
    struct format_state* state,
    int character,
    size_t count
)
{
    size_t processed = 0;

    for (size_t i = 0; i < count; ++i)
    {
        if (state->processed < state->buffer_capacity)
        {
            ++processed;
            state->buffer[state->processed] = (char)character;
        }

        ++(state->processed);
    }

    state->current += processed;
}

static inline
void
rtlp_format_emit_string_n(
    struct format_state* state,
    const char* string,
    size_t length
)
{
    size_t processed = 0;

    for (size_t i = 0; i < length; ++i)
    {
        if (state->processed < state->buffer_capacity)
        {
            ++processed;
            state->buffer[state->processed] = string[i];
        }

        ++(state->processed);
    }

    state->current += processed;
}

static inline
void
format_emit_string(
    struct format_state* state,
    const char* string
)
{
    size_t processed = 0;

    for (; *string != '\0'; ++string)
    {
        if (state->processed < state->buffer_capacity)
        {
            ++processed;
            state->buffer[state->processed] = *string;
        }

        ++(state->processed);
    }

    state->current += processed;
}

static inline
void
rtlp_format_write_integer(
    struct format_state* state,
    uintmax_t value
)
{
    char sign = '\0';

    if (!RTL_FLAGS_HAS(state->format_type, FORMAT_TYPE_UNSIGNED))
    {
        const intmax_t svalue = (intmax_t)value;
        const bool is_negative = svalue < 0;

        value = (is_negative ? ((uintmax_t)-svalue) : ((uintmax_t)svalue));

        if (is_negative)
        {
            sign = '-';
        }
        else if (RTL_FLAGS_HAS(state->format_flags, FORMAT_FLAGS_PLUS))
        {
            sign = '+';
        }
        else if (RTL_FLAGS_HAS(state->format_flags, FORMAT_FLAGS_SPACE))
        {
            sign = ' ';
        }
    }

    enum { MAX_INTEGER_LENGTH = 96 };

    char output_buffer[MAX_INTEGER_LENGTH];
    char* output_buffer_end = output_buffer + MAX_INTEGER_LENGTH;

    ptrdiff_t written = 0;

    const char* digits = RTL_FLAGS_HAS(state->format_flags, FORMAT_FLAGS_LOWERCASE)
        ? rtl_string_digits_lowercase
        : rtl_string_digits_uppercase;

    uintmax_t remaining = value;

    if ((state->format_precision != 0) || (remaining != 0))
    {
        do
        {
            uintmax_t digit = remaining % state->format_base;
            remaining /= state->format_base;
            output_buffer_end[-++written] = digits[digit];
        } while (remaining != 0);
    }

    while ((int32_t)written < state->format_precision)
    {
        output_buffer_end[-++written] = '0';
    }

    size_t padding = 0;

    if (!RTL_FLAGS_HAS(state->format_flags, FORMAT_FLAGS_MINUS) && RTL_FLAGS_HAS(state->format_flags, FORMAT_FLAGS_ZERO))
    {
        while (written < (ptrdiff_t)state->format_width)
        {
            output_buffer_end[-++written] = '0';
            ++padding;
        }
    }

    if (sign != '\0')
    {
        if (padding == 0)
        {
            ++written;
        }

        output_buffer_end[-written] = sign;
    }
    else if (RTL_FLAGS_HAS(state->format_flags, FORMAT_FLAGS_ALT))
    {
        switch (state->format_base)
        {
            case 2:
            {
                written += (padding < 2) ? (2 - padding) : 0;
                output_buffer_end[-written + 0] = '0';
                output_buffer_end[-written + 1] = RTL_FLAGS_HAS(state->format_flags, FORMAT_FLAGS_LOWERCASE) ? 'b' : 'B';
                break;
            }

            case 8:
            {
                if (output_buffer_end[-written] != '0')
                {
                    output_buffer_end[-++written] = '0';
                }
                break;
            }

            case 16:
            {
                written += (padding < 2) ? (2 - padding) : 0;
                output_buffer_end[-written + 0] = '0';
                output_buffer_end[-written + 1] = RTL_FLAGS_HAS(state->format_flags, FORMAT_FLAGS_LOWERCASE) ? 'x': 'X';
                break;
            }

            default:
            {
                break;
            }
        }
    }

    if (!RTL_FLAGS_HAS(state->format_flags, FORMAT_FLAGS_MINUS))
    {
        char leading = (!RTL_FLAGS_HAS(state->format_flags, FORMAT_FLAGS_MINUS) && RTL_FLAGS_HAS(state->format_flags, FORMAT_FLAGS_ZERO)) ? '0' : ' ';

        while (written < (ptrdiff_t)state->format_width)
        {
            output_buffer_end[-++written] = leading;
        }
    }

    rtlp_format_emit_string_n(state, output_buffer_end - written, (size_t)written);
}

static inline
void
rtlp_format_write_char(
    struct format_state* state,
    int value
)
{
    if (RTL_FLAGS_HAS(state->format_flags, FORMAT_FLAGS_MINUS))
    {
        rtlp_format_emit_char(state, value, 1);

        if ((size_t)(uint32_t)state->format_width > state->current)
        {
            rtlp_format_emit_char(state, ' ', (size_t)(uint32_t)state->format_width - state->current);
        }
    }
    else
    {
        if (state->format_width != 0)
        {
            size_t adjust = (size_t)(uint32_t)state->format_width - state->current - 1;

            rtlp_format_emit_char(state, ' ', adjust);
        }

        rtlp_format_emit_char(state, value, 1);
    }
}

// Disable call to strlen :)
__attribute__((__optimize__("-fno-tree-loop-distribute-patterns")))
static size_t (strlen)(const char* _String)
{
    const char* eos = _String;

    while (*eos != '\0')
    {
        ++eos;
    }

    return eos - _String;
}

static inline
void
rtlp_format_write_string(
    struct format_state* state,
    const char* string
)
{
    size_t length = strlen(string);

    if (state->format_precision >= 0)
    {
        size_t precision = (size_t)(uint32_t)state->format_precision;
        length = RTL_MIN(length, precision);
    }

    if ((state->format_width == 0) || RTL_FLAGS_HAS(state->format_flags, FORMAT_FLAGS_MINUS))
    {
        if (state->format_precision > 0)
        {
            size_t precision = (size_t)(uint32_t)state->format_precision;
            length = RTL_MIN(length, precision);
        }

        rtlp_format_emit_string_n(state, string, length);

        if ((size_t)(uint32_t)state->format_width > state->current)
        {
            length = (size_t)(uint32_t)state->format_width - state->current;

            rtlp_format_emit_char(state, ' ', length);
        }
    }
    else
    {
        if ((size_t)(uint32_t)state->format_width > length)
        {
            size_t padding = (size_t)(uint32_t)state->format_width - length;

            rtlp_format_emit_char(state, ' ', padding);
        }

        rtlp_format_emit_string_n(state, string, length);
    }
}

static inline int
rtlp_format_handle_specifier(
    struct format_state* state,
    const char* specifier,
    size_t* processed
)
{
    int status = 0;

    const char* mark = specifier;

    if (*(++specifier) == '%')
    {
        rtlp_format_emit_char(state, *specifier, 1);

        ++specifier;
        (*processed) = (size_t)(ptrdiff_t)(specifier - mark);
        return status;
    }

    state->format_flags = 0;
    state->format_base = 0;
    state->format_type = 0;
    state->format_width = 0;
    state->format_precision = -1;
    state->current = 0;

    do
    {
        switch (*specifier++)
        {
            case '-':
            {
                state->format_flags |= FORMAT_FLAGS_MINUS;
                break;
            }
            case '+':
            {
                state->format_flags |= FORMAT_FLAGS_PLUS;
                break;
            }
            case '#':
            {
                state->format_flags |= FORMAT_FLAGS_ALT;
                break;
            }
            case ' ':
            {
                state->format_flags |= FORMAT_FLAGS_SPACE;
                break;
            }
            case '0':
            {
                state->format_flags |= FORMAT_FLAGS_ZERO;
                break;
            }
            default:
            {
                state->format_flags |= FORMAT_FLAGS_DONE;
                --specifier;
                break;
            }
        }
    } while (!RTL_FLAGS_HAS(state->format_flags, FORMAT_FLAGS_DONE));

    if (*specifier == '*')
    {
        int32_t width = (int32_t)va_arg(state->arglist, int);

        if (width < 0)
        {
            state->format_flags |= FORMAT_FLAGS_MINUS;
            width = -width;
        }

        state->format_width = width;
        ++specifier;
    }
    else
    {
        int32_t width = 0;

        status = rtl_string_to_int32(&width, specifier, (char**)&specifier, 10);
        if (status != 0)
        {
            (*processed) = 0;
            return status;
        }
        
        state->format_width = width;
    }

    if (*specifier == '.')
    {
        ++specifier;

        if (*specifier == '*')
        {
            state->format_precision = (int32_t)va_arg(state->arglist, int);
            ++specifier;
        }
        else
        {
            int32_t precision = 0;

            status = rtl_string_to_int32(&precision, specifier, (char**)&specifier, 10);
            if (status != 0)
            {
                (*processed) = 0;
                return status;
            }

            state->format_precision = precision;
        }

        state->format_flags = RTL_FLAGS_UNSET(state->format_flags, FORMAT_FLAGS_ZERO);
    }

    switch (*(specifier++))
    {
        case 'h':
        {
            if (*specifier == 'h')
            {
                state->format_type = FORMAT_TYPE_CHAR;
                ++specifier;
            }
            else
            {
                state->format_type = FORMAT_TYPE_SHORT;
            }
            break;
        }

        case 'l':
        {
            if (*specifier == 'l')
            {
                state->format_type = FORMAT_TYPE_LONGLONG;
                ++specifier;
            }
            else
            {
                state->format_type = FORMAT_TYPE_LONG;
            }
            break;
        }

        case 'j':
        {
            state->format_type = FORMAT_TYPE_INTMAX;
            break;
        }

        case 'z':
        {
            state->format_type = FORMAT_TYPE_SIZE;
            break;
        }

        case 't':
        {
            state->format_type = FORMAT_TYPE_PTRDIFF;
            break;
        }

        case 'L':
        {
            state->format_type = FORMAT_TYPE_LONGDOUBLE;
            break;
        }

        default:
        {
            --specifier;
            break;
        }
    }

    switch (*specifier)
    {
        case 'b':
        {
            state->format_flags |= FORMAT_FLAGS_LOWERCASE;
        }
        case 'B':
        {
            state->format_type |= FORMAT_TYPE_UNSIGNED;
            state->format_base = 2;
            break;
        }

        case 'o':
        {
            state->format_type |= FORMAT_TYPE_UNSIGNED;
            state->format_base = 8;
            break;
        }

        case 'u':
        {
            state->format_type |= FORMAT_TYPE_UNSIGNED;
        }
        case 'd':
        case 'i':
        {
            state->format_base = 10;
            break;
        }

        case 'x':
        {
            state->format_flags |= FORMAT_FLAGS_LOWERCASE;
        }
        case 'X':
        {
            state->format_type |= FORMAT_TYPE_UNSIGNED;
            state->format_base = 16;
            break;
        }

        case 'p':
        {
            state->format_flags |= FORMAT_FLAGS_LOWERCASE;
        }
        case 'P':
        {
            state->format_flags |= FORMAT_FLAGS_ALT;
            state->format_type = FORMAT_TYPE_UINTPTR;
            state->format_precision = sizeof(uintptr_t) * 2;
            state->format_base = 16;
            break;
        }

        case 'a':
        case 'e':
        case 'f':
        case 'g':
        {
            state->format_flags |= FORMAT_FLAGS_LOWERCASE;
        }
        case 'A':
        case 'E':
        case 'F':
        case 'G':
        {
            return -1;
        }

        case 'c':
        {
            int value = (int)va_arg(state->arglist, int);
            rtlp_format_write_char(state, value);

            ++specifier;
            (*processed) = (size_t)(ptrdiff_t)(specifier - mark);
            return 0;
        }

        case 's':
        {
            const char* value = (const char*)va_arg(state->arglist, const char*);

            if (value == NULL)
            {
                value = "(null)";
            }

            rtlp_format_write_string(state, value);

            ++specifier;
            (*processed) = (size_t)(ptrdiff_t)(specifier - mark);
            return 0;
        }

        default:
        {
            (*processed) = 0;
            return -1;
        }
    }

    // Integral output.

    if (state->format_base != 0)
    {
        uintmax_t value;

        switch (state->format_type)
        {
            case FORMAT_TYPE_CHAR:
                value = (uintmax_t)(intmax_t)(char)va_arg(state->arglist, int);
                break;
            case FORMAT_TYPE_SHORT:
                value = (uintmax_t)(intmax_t)(short)va_arg(state->arglist, int);
                break;
            case FORMAT_TYPE_INT:
                value = (uintmax_t)(intmax_t)(int)va_arg(state->arglist, int);
                break;
            case FORMAT_TYPE_LONG:
                value = (uintmax_t)(intmax_t)(long)va_arg(state->arglist, long);
                break;
            case FORMAT_TYPE_LONGLONG:
                value = (uintmax_t)(intmax_t)(long long)va_arg(state->arglist, long long);
                break;
            case FORMAT_TYPE_UNSIGNED | FORMAT_TYPE_CHAR:
                value = (uintmax_t)(unsigned char)va_arg(state->arglist, int);
                break;
            case FORMAT_TYPE_UNSIGNED | FORMAT_TYPE_SHORT:
                value = (uintmax_t)(unsigned short)va_arg(state->arglist, int);
                break;
            case FORMAT_TYPE_UNSIGNED | FORMAT_TYPE_INT:
                value = (uintmax_t)(unsigned int)va_arg(state->arglist, unsigned int);
                break;
            case FORMAT_TYPE_UNSIGNED | FORMAT_TYPE_LONG:
                value = (uintmax_t)(unsigned long)va_arg(state->arglist, unsigned long);
                break;
            case FORMAT_TYPE_UNSIGNED | FORMAT_TYPE_LONGLONG:
                value = (uintmax_t)(unsigned long long)va_arg(state->arglist, unsigned long long);
                break;
            default:
                (*processed) = 0;
                return -1;
        }

        rtlp_format_write_integer(state, value);

        if (RTL_FLAGS_HAS(state->format_flags, FORMAT_FLAGS_MINUS) && (state->current - (size_t)(uint32_t)state->format_width) != 0) // Check: possible overrun
        {
            rtlp_format_emit_char(state, ' ', (size_t)(uint32_t)state->format_width - state->current);
        }
    }

    ++specifier;

    (*processed) = (size_t)(ptrdiff_t)(specifier - mark);
    return status;
}

int
rtl_string_format_v(
    char* buffer,
    size_t buffer_length,
    size_t* processed,
    const char* format,
    va_list arglist
)
{
    if (format == NULL)
    {
        return -1;
    }

    if ((buffer == NULL) && (buffer_length != 0))
    {
        return -1;
    }

    struct format_state state = {
        .format_base = 0,
        .format_flags = 0,
        .format_type = 0,
        .format_width = 0,
        .format_precision = 0,
        .buffer = buffer,
        .buffer_capacity = buffer_length,
        .processed = 0,
        .current = 0,
    };
    va_copy(state.arglist, arglist);

    int status = 0;

    while (*format != '\0')
    {
        const char* mark = format;

        while (*format != '\0' && *format != '%')
        {
            ++format;
        }

        if (mark != format)
        {
            rtlp_format_emit_string_n(&state, mark, format - mark);
        }

        if (*format == '%')
        {
            size_t consumed = 0;
            status = rtlp_format_handle_specifier(&state, format, &consumed);

            if (status != 0)
            {
                if (processed != NULL)
                {
                    (*processed) = 0;
                }
                return status;
            }

            format += consumed;

            if (consumed == 0)
            {
                ++format;
                rtlp_format_emit_char(&state, *format, 1);
            }
        }
    }

    if (state.buffer != NULL)
    {
        if (state.processed < state.buffer_capacity)
        {
            state.buffer[state.processed] = '\0';
        }
        else
        {
            state.buffer[state.buffer_capacity - 1] = '\0';
        }
    }

    va_end(state.arglist);

    if (processed != NULL)
    {
        (*processed) = state.processed;
    }

    return status;
}

int
rtl_string_format(
    char* buffer,
    size_t buffer_length,
    size_t* processed,
    const char* format,
    ...
)
{
    va_list arglist;
    va_start(arglist, format);
    int status = rtl_string_format_v(
        buffer,
        buffer_length,
        processed,
        format,
        arglist
    );
    va_end(arglist);

    return status;
}
