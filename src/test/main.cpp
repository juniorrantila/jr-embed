#include <stdio.h>

using u8 = unsigned char;
using u32 = unsigned int;

#pragma JR embed THIS_FILE __FILE__
#ifndef THIS_FILE
#    define THIS_FILE 0,
#    error "not using pragma embed plugin"
#endif

static constexpr u8 const this_file[] = { THIS_FILE };
static constexpr u32 const this_file_size = sizeof(this_file);

int main()
{
    printf("%.*s\n", this_file_size, this_file);
    return 0;
}
