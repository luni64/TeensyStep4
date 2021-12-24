#pragma once

#define TSLOG_STREAM Serial

#if defined(TSLOG_STREAM)
#    define LOG(...)                      \
        while (!TSLOG_STREAM) {};         \
        TSLOG_STREAM.printf("LOG: ");     \
        TSLOG_STREAM.printf(__VA_ARGS__); \
        TSLOG_STREAM.printf("  (%s %s l:%d)\n", __func__, __FILE__, __LINE__);
#else
#    define LOG(...) ;
#endif
