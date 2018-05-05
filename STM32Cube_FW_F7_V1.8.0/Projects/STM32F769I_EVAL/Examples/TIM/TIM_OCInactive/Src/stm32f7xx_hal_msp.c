/**
 ******************************************************************************
 * @file    TIM/TIM_OCInactive/Src/stm32f7xx_hal_msp.c
 * @author  MCD Application Team
 * @brief   HAL MSP module.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32F7xx_HAL_Examples
 * @{
 */

/** @defgroup HAL_MSP
 * @brief HAL MSP module.
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions
 * @{
 */

/**
 * @brief TIM MSP Initialization
 *        This function configures the hardware resources used in this example:
 *           - Peripheral's clock enable
 *           - Peripheral's GPIO Configuration
 * @param htim: TIM handle pointer
 * @retval None
 */
void HAL_TIM_OC_MspInit(TIM_HandleTypeDef* htim)
{
   GPIO_InitTypeDef GPIO_InitStruct;

   /*##-1- Enable peripherals and GPIO Clocks #################################*/
   /* TIM1 Peripheral clock enable */
   TIMx_CLK_ENABLE();

   /* Enable GPIO Channels Clock */
   TIMx_CHANNEL_GPIO_PORT;

   /*##-2- Configure I/Os #####################################################*/
   /* Configure PA.08 (TIM1_Channel1), PA.09 (TIM1_Channel2), PA.10 (TIM1_Channel3),
      PE.14 (TIM1_Channel4) in output, pull-up, alternate function mode
   */
   /* Common configuration for all channels */
   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStruct.Pull = GPIO_PULLUP;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

   GPIO_InitStruct.Alternate = TIMx_GPIO_AF_CHANNEL1;
   GPIO_InitStruct.Pin = TIMx_GPIO_PIN_CHANNEL1;
   HAL_GPIO_Init(TIMx_GPIO_PORT_CHANNEL1, &GPIO_InitStruct);

   GPIO_InitStruct.Alternate = TIMx_GPIO_AF_CHANNEL2;
   GPIO_InitStruct.Pin = TIMx_GPIO_PIN_CHANNEL2;
   HAL_GPIO_Init(TIMx_GPIO_PORT_CHANNEL2, &GPIO_InitStruct);

   GPIO_InitStruct.Alternate = TIMx_GPIO_AF_CHANNEL3;
   GPIO_InitStruct.Pin = TIMx_GPIO_PIN_CHANNEL3;
   HAL_GPIO_Init(TIMx_GPIO_PORT_CHANNEL3, &GPIO_InitStruct);

   GPIO_InitStruct.Alternate = TIMx_GPIO_AF_CHANNEL4;
   GPIO_InitStruct.Pin = TIMx_GPIO_PIN_CHANNEL4;
   HAL_GPIO_Init(TIMx_GPIO_PORT_CHANNEL4, &GPIO_InitStruct);

   /* Enable the TIM1 global Interrupt & set priority */
   HAL_NVIC_SetPriority(TIM1_CC_IRQn, 0, 1);
   HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
}

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
