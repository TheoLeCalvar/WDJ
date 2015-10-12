#ifndef _log_h_
#define _log_h_

#define INFO_LEVEL 4
#define WARN_LEVEL 2
#define ERR_LEVEL  0

#ifndef LOG_LEVEL
#define LOG_LEVEL  2
#endif

#ifdef DEBUG
#define log__(_type, _stream, _format, ...) \
        ({ \
        fprintf(_stream, \
                _type": %s:%d in %s: "_format"\n", \
                __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
                fflush(_stream); \
        })
#else
#define log__(_type, _stream, _format, ...) \
        ({ \
        fprintf(_stream, \
                _format"\n", \
                ##__VA_ARGS__); \
                fflush(_stream); \
        })
#endif

#if LOG_LEVEL >= ERR_LEVEL
#define log_err(_format, ...) \
        ({ \
        log__("error", stderr, "\033[31m" _format "\033[0m", ##__VA_ARGS__); \
        })
#else
#define log_err(_format, ...)
#endif

#if LOG_LEVEL >= INFO_LEVEL
#define log_info(_format, ...) \
        ({ \
        log__("info", stderr, "\033[32m" _format "\033[0m", ##__VA_ARGS__); \
        })
#else
#define log_info(_format, ...)
#endif

#if LOG_LEVEL >= WARN_LEVEL
#define log_warn(_format, ...) \
        ({ \
        log__("warning", stderr, "\033[33m" _format "\033[0m", ##__VA_ARGS__); \
        })
#else
#define log_warn(_format, ...)
#endif

#endif
