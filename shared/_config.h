#pragma once

#define BS_AUDIOLINK_EXPORT __attribute__((visibility("default")))

#ifdef __cplusplus
#define BS_AUDIOLINK_EXPORT_FUNC extern "C" BS_AUDIOLINK_EXPORT
#else
#define BS_AUDIOLINK_EXPORT_FUNC BS_AUDIOLINK_EXPORT
#endif
