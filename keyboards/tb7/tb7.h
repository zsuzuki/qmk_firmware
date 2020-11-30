#pragma once

#include "quantum.h"

#define LAYOUT( \
        A2, A3, \
    B1, B2, B3, \
    C1, C2, C3 \
) { \
    { KC_NO, A2, A3 }, \
    { B1, B2, B3 }, \
    { C1, C2, C3 }, \
}
