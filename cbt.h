#pragma once
#include "cbt_config.h"

namespace cbt
{
    enum class Result
    {
        SUCCESS,
        FAIL,
    };

    Result build();
};