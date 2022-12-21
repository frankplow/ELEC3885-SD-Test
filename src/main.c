///
/// @file   main.c
/// @author Frank Plowman <post@frankplowman.com>
/// @brief  Application-specific code and main loop
///

#include <stm32f4xx_hal.h>
#include <stm32f4xx_nucleo_144.h>

// function prototypes
static void sysclk_cfg(void);
static void error_handler(void);
static void configure_onboard_leds(void);

int main(void) {
  // initialise
  HAL_Init();
  sysclk_cfg();
  configure_onboard_leds();

  while (1) {
    HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);
    HAL_Delay(100);
    HAL_GPIO_TogglePin(LED2_GPIO_PORT, LED2_PIN);
    HAL_Delay(100);
    HAL_GPIO_TogglePin(LED3_GPIO_PORT, LED3_PIN);
    HAL_Delay(100);
  }
}

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSE)
 *            SYSCLK(Hz)                     = 180000000
 *            HCLK(Hz)                       = 180000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 4
 *            APB2 Prescaler                 = 2
 *            HSE Frequency(Hz)              = 8000000
 *            PLL_M                          = 8
 *            PLL_N                          = 360
 *            PLL_P                          = 2
 *            PLL_Q                          = 7
 *            VDD(V)                         = 3.3
 *            Main regulator output voltage  = Scale1 mode
 *            Flash Latency(WS)              = 5
 * @param  None
 * @retval None
 */
static void sysclk_cfg(void) {
  RCC_OscInitTypeDef rccosc_cfg;
  RCC_ClkInitTypeDef rccclk_cfg;

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  // configure oscillator
  rccosc_cfg.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  rccosc_cfg.HSEState = RCC_HSE_BYPASS;
  rccosc_cfg.PLL.PLLState = RCC_PLL_ON;
  rccosc_cfg.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  rccosc_cfg.PLL.PLLM = 8;
  rccosc_cfg.PLL.PLLN = 360;
  rccosc_cfg.PLL.PLLP = RCC_PLLP_DIV2;
  rccosc_cfg.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&rccosc_cfg) != HAL_OK) {
    error_handler();
  }

  if (HAL_PWREx_EnableOverDrive() != HAL_OK) {
    error_handler();
  }

  // configure clock
  rccclk_cfg.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                          RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  rccclk_cfg.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  rccclk_cfg.AHBCLKDivider = RCC_SYSCLK_DIV1;
  rccclk_cfg.APB1CLKDivider = RCC_HCLK_DIV4;
  rccclk_cfg.APB2CLKDivider = RCC_HCLK_DIV2;
  if (HAL_RCC_ClockConfig(&rccclk_cfg, FLASH_LATENCY_5) != HAL_OK) {
    error_handler();
  }
}

static void error_handler(void) {
  BSP_LED_Off(LED1);
  BSP_LED_Off(LED2);
  BSP_LED_Off(LED3);
  while (1) {
  }
}

static void configure_onboard_leds() {
  GPIO_InitTypeDef gpio_cfg;

  // configure LEDs
  configure_onboard_leds();
  LED1_GPIO_CLK_ENABLE();
  LED2_GPIO_CLK_ENABLE();
  LED3_GPIO_CLK_ENABLE();
  gpio_cfg.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_cfg.Pull = GPIO_PULLUP;
  gpio_cfg.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_cfg.Pin = LED1_PIN;
  HAL_GPIO_Init(LED1_GPIO_PORT, &gpio_cfg);
  gpio_cfg.Pin = LED2_PIN;
  HAL_GPIO_Init(LED2_GPIO_PORT, &gpio_cfg);
  gpio_cfg.Pin = LED3_PIN;
  HAL_GPIO_Init(LED3_GPIO_PORT, &gpio_cfg);
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) { error_handler(); }
#endif
