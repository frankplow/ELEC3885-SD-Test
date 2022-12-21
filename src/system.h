///
/// @file   system.h
/// @author Frank Plowman <post@frankplowman.com>
/// @brief  System configuration definitions
///

#ifndef SYSTEM_H_
#define SYSTEM_H_

/// @addtogroup Target
/// @{
/// @addtogroup Configuration
///@{
/// @addtogroup Clock
///@{

/// @brief Frequency of the high-speed internal clock source in Hz
#define HSI_VALUE (16000000U)

/// @brief Frequency of the low-speed internal clock source in Hz
#define LSI_VALUE (32000U)

/// @brief Frequency of the high-speed external clock source in Hz
#define HSE_VALUE (8000000U)

/// @brief Time for high-speed external clock source to start in ms
#define HSE_STARTUP_TIMEOUT (100U)

/// @brief Frequency of the low-speed external clock source in Hz
#define LSE_VALUE (32768U)

/// @brief Time for low-speed external clock source to start in ms
#define LSE_STARTUP_TIMEOUT (5000U)

/// @} Clock
/// @} Configuration
/// @} Target

#endif  // SYSTEM_H_
