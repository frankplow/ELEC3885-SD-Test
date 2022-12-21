///
/// @file   stm32f4xx_it.c
/// @author MCD Application Team
/// @brief  Main Interrupt Service Routines.
///

#include <stm32f4xx_hal.h>
#include <stm32f4xx_nucleo_144.h>

/// @addtogroup Target
/// @{
/// @addtogroup Interrupts
///@{

///
/// @brief  This function handles NMI exception.
/// @param  None
/// @retval None
///
void NMI_Handler(void) {}

///
/// @brief  This function handles Hard Fault exception.
/// @param  None
/// @retval None
///
void HardFault_Handler(void) {
  while (1) {
  }
}

///
/// @brief  This function handles Memory Manage exception.
/// @param  None
/// @retval None
///
void MemManage_Handler(void) {
  while (1) {
  }
}

///
/// @brief  This function handles Bus Fault exception.
/// @param  None
/// @retval None
///
void BusFault_Handler(void) {
  while (1) {
  }
}

///
/// @brief  This function handles Usage Fault exception.
/// @param  None
/// @retval None
///
void UsageFault_Handler(void) {
  while (1) {
  }
}

///
/// @brief  This function handles SVCall exception.
/// @param  None
/// @retval None
///
void SVC_Handler(void) {}

///
/// @brief  This function handles Debug Monitor exception.
/// @param  None
/// @retval None
///
void DebugMon_Handler(void) {}

///
/// @brief  This function handles PendSVC exception.
/// @param  None
/// @retval None
///
void PendSV_Handler(void) {}

///
/// @brief  This function handles SysTick Handler.
/// @param  None
/// @retval None
///
void SysTick_Handler(void) { HAL_IncTick(); }

///
/// @brief  This function handles PPP interrupt request.
/// @param  None
/// @retval None
///
/*void PPP_IRQHandler(void)
{
}*/

/// @} Interrupts
/// @} Target
