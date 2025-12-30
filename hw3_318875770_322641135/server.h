#pragma once
#include <netinet/in.h>
#include "macros.h"

typedef struct{
    int fd;
    struct in_addr addr;
    char name[MAX_LEN_USER_MSG + 1];
    int has_name;
} client_info;