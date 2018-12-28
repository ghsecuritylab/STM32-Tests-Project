/**
 ******************************************************************************
 * @file    USB_Device/HID_Standalone/Src/usbd_conf.c
 * @author  MCD Application Team
 * @brief   This file implements the USB Device library callbacks and MSP
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics International N.V.
 * All rights reserved.</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32F7xx_HAL_Applications
 * @{
 */

/** @addtogroup USB_Device_HID_Standalone
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CURSOR_STEP 5

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
PCD_HandleTypeDef hpcd;
uint8_t HID_Buffer[4];
extern USBD_HandleTypeDef USBD_Device;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
                       PCD BSP Routines
*******************************************************************************/

/**
 * @brief  Initializes the PCD MSP.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_MspInit(PCD_HandleTypeDef* hpcd)
{
   GPIO_InitTypeDef GPIO_InitStruct;

   /* Configure USB FS GPIOs */
   __HAL_RCC_GPIOA_CLK_ENABLE();

   /* Configure DM DP Pins */
   GPIO_InitStruct.Pin = (GPIO_PIN_11 | GPIO_PIN_12);
   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
   GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

   /* Configure VBUS Pin */
   GPIO_InitStruct.Pin = GPIO_PIN_9;
   GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

   /* Configure ID pin */
   GPIO_InitStruct.Pin = GPIO_PIN_10;
   GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
   GPIO_InitStruct.Pull = GPIO_PULLUP;
   GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

   /* Enable USB FS Clocks */
   __HAL_RCC_USB_OTG_FS_CLK_ENABLE();

   /* Set USBFS Interrupt priority */
   HAL_NVIC_SetPriority(OTG_FS_IRQn, 5, 0);

   /* Enable USBFS Interrupt */
   HAL_NVIC_EnableIRQ(OTG_FS_IRQn);

   if (hpcd->Init.low_power_enable == 1)
   {
      /* Enable EXTI Line 18 for USB wakeup*/
      __HAL_USB_OTG_FS_WAKEUP_EXTI_CLEAR_FLAG();
      __HAL_USB_OTG_FS_WAKEUP_EXTI_ENABLE_RISING_EDGE();
      __HAL_USB_OTG_FS_WAKEUP_EXTI_ENABLE_IT();

      /* Set EXTI Wakeup Interrupt priority*/
      HAL_NVIC_SetPriority(OTG_FS_WKUP_IRQn, 0, 0);

      /* Enable EXTI Interrupt */
      HAL_NVIC_EnableIRQ(OTG_FS_WKUP_IRQn);
   }
}

/**
 * @brief  De-Initializes the PCD MSP.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_MspDeInit(PCD_HandleTypeDef* hpcd)
{
   /* Disable USB FS Clock */
   __HAL_RCC_USB_OTG_FS_CLK_DISABLE();
   __HAL_RCC_SYSCFG_CLK_DISABLE();
}

/*******************************************************************************
                       LL Driver Callbacks (PCD -> USB Device Library)
*******************************************************************************/

/**
 * @brief  SetupStage callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef* hpcd)
{
   USBD_LL_SetupStage(hpcd->pData, (uint8_t*)hpcd->Setup);
}

/**
 * @brief  DataOut Stage callback.
 * @param  hpcd: PCD handle
 * @param  epnum: Endpoint Number
 * @retval None
 */
void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef* hpcd, uint8_t epnum)
{
   USBD_LL_DataOutStage(hpcd->pData, epnum, hpcd->OUT_ep[epnum].xfer_buff);
}

/**
 * @brief  DataIn Stage callback.
 * @param  hpcd: PCD handle
 * @param  epnum: Endpoint Number
 * @retval None
 */
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef* hpcd, uint8_t epnum)
{
   USBD_LL_DataInStage(hpcd->pData, epnum, hpcd->IN_ep[epnum].xfer_buff);
}

/**
 * @brief  SOF callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_SOFCallback(PCD_HandleTypeDef* hpcd)
{
   USBD_LL_SOF(hpcd->pData);
}

/**
 * @brief  Reset callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_ResetCallback(PCD_HandleTypeDef* hpcd)
{
   USBD_SpeedTypeDef speed = USBD_SPEED_FULL;

   /* Set USB Current Speed */
   switch (hpcd->Init.speed)
   {
   case PCD_SPEED_HIGH:
      speed = USBD_SPEED_HIGH;
      break;

   case PCD_SPEED_FULL:
      speed = USBD_SPEED_FULL;
      break;

   default:
      speed = USBD_SPEED_FULL;
      break;
   }

   /* Reset Device */
   USBD_LL_Reset(hpcd->pData);

   USBD_LL_SetSpeed(hpcd->pData, speed);
}

/**
 * @brief  Suspend callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_SuspendCallback(PCD_HandleTypeDef* hpcd)
{
   USBD_LL_Suspend(hpcd->pData);
   __HAL_PCD_GATE_PHYCLOCK(hpcd);

   /*Enter in STOP mode */
   if (hpcd->Init.low_power_enable)
   {
      /* Set SLEEPDEEP bit and SleepOnExit of Cortex System Control Register */
      SCB->SCR |= (uint32_t)((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
   }
}

/**
 * @brief  Resume callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_ResumeCallback(PCD_HandleTypeDef* hpcd)
{
   USBD_LL_Resume(hpcd->pData);
}

/**
 * @brief  ISOOUTIncomplete callback.
 * @param  hpcd: PCD handle
 * @param  epnum: Endpoint Number
 * @retval None
 */
void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef* hpcd, uint8_t epnum)
{
   USBD_LL_IsoOUTIncomplete(hpcd->pData, epnum);
}

/**
 * @brief  ISOINIncomplete callback.
 * @param  hpcd: PCD handle
 * @param  epnum: Endpoint Number
 * @retval None
 */
void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef* hpcd, uint8_t epnum)
{
   USBD_LL_IsoINIncomplete(hpcd->pData, epnum);
}

/**
 * @brief  ConnectCallback callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_ConnectCallback(PCD_HandleTypeDef* hpcd)
{
   USBD_LL_DevConnected(hpcd->pData);
}

/**
 * @brief  Disconnect callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef* hpcd)
{
   USBD_LL_DevDisconnected(hpcd->pData);
}

/*******************************************************************************
                       LL Driver Interface (USB Device Library --> PCD)
*******************************************************************************/

/**
 * @brief  Initializes the Low Level portion of the Device driver.
 * @param  pdev: Device handle
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_Init(USBD_HandleTypeDef* pdev)
{
   /* Set LL Driver parameters */
   hpcd.Instance = USB_OTG_FS;
   hpcd.Init.dev_endpoints = 4;
   hpcd.Init.use_dedicated_ep1 = 0;
   hpcd.Init.ep0_mps = 0x40;
   hpcd.Init.dma_enable = 0;
   hpcd.Init.low_power_enable = 1;
   hpcd.Init.phy_itface = PCD_PHY_EMBEDDED;
   hpcd.Init.Sof_enable = 0;
   hpcd.Init.speed = PCD_SPEED_FULL;
   hpcd.Init.vbus_sensing_enable = 1;
   hpcd.Init.lpm_enable = 0;

   /* Link The driver to the stack */
   hpcd.pData = pdev;
   pdev->pData = &hpcd;

   /* Initialize LL Driver */
   HAL_PCD_Init(&hpcd);

   HAL_PCDEx_SetRxFiFo(&hpcd, 0x80);
   HAL_PCDEx_SetTxFiFo(&hpcd, 0, 0x40);
   HAL_PCDEx_SetTxFiFo(&hpcd, 1, 0x80);

   return USBD_OK;
}

/**
 * @brief  De-Initializes the Low Level portion of the Device driver.
 * @param  pdev: Device handle
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_DeInit(USBD_HandleTypeDef* pdev)
{
   HAL_PCD_DeInit(pdev->pData);
   return USBD_OK;
}

/**
 * @brief  Starts the Low Level portion of the Device driver.
 * @param  pdev: Device handle
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_Start(USBD_HandleTypeDef* pdev)
{
   HAL_PCD_Start(pdev->pData);
   return USBD_OK;
}

/**
 * @brief  Stops the Low Level portion of the Device driver.
 * @param  pdev: Device handle
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_Stop(USBD_HandleTypeDef* pdev)
{
   HAL_PCD_Stop(pdev->pData);
   return USBD_OK;
}

/**
 * @brief  Opens an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @param  ep_type: Endpoint Type
 * @param  ep_mps: Endpoint Max Packet Size
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_OpenEP(USBD_HandleTypeDef* pdev, uint8_t ep_addr, uint8_t ep_type, uint16_t ep_mps)
{
   HAL_PCD_EP_Open(pdev->pData, ep_addr, ep_mps, ep_type);

   return USBD_OK;
}

/**
 * @brief  Closes an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_CloseEP(USBD_HandleTypeDef* pdev, uint8_t ep_addr)
{
   HAL_PCD_EP_Close(pdev->pData, ep_addr);
   return USBD_OK;
}

/**
 * @brief  Flushes an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_FlushEP(USBD_HandleTypeDef* pdev, uint8_t ep_addr)
{
   HAL_PCD_EP_Flush(pdev->pData, ep_addr);
   return USBD_OK;
}

/**
 * @brief  Sets a Stall condition on an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_StallEP(USBD_HandleTypeDef* pdev, uint8_t ep_addr)
{
   HAL_PCD_EP_SetStall(pdev->pData, ep_addr);
   return USBD_OK;
}

/**
 * @brief  Clears a Stall condition on an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_ClearStallEP(USBD_HandleTypeDef* pdev, uint8_t ep_addr)
{
   HAL_PCD_EP_ClrStall(pdev->pData, ep_addr);
   return USBD_OK;
}

/**
 * @brief  Returns Stall condition.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval Stall (1: Yes, 0: No)
 */
uint8_t USBD_LL_IsStallEP(USBD_HandleTypeDef* pdev, uint8_t ep_addr)
{
   PCD_HandleTypeDef* hpcd = pdev->pData;

   if ((ep_addr & 0x80) == 0x80)
   {
      return hpcd->IN_ep[ep_addr & 0x7F].is_stall;
   }
   else
   {
      return hpcd->OUT_ep[ep_addr & 0x7F].is_stall;
   }
}

/**
 * @brief  Assigns a USB address to the device.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_SetUSBAddress(USBD_HandleTypeDef* pdev, uint8_t dev_addr)
{
   HAL_PCD_SetAddress(pdev->pData, dev_addr);
   return USBD_OK;
}

/**
 * @brief  Transmits data over an endpoint.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @param  pbuf: Pointer to data to be sent
 * @param  size: Data size
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_Transmit(USBD_HandleTypeDef* pdev, uint8_t ep_addr, uint8_t* pbuf, uint16_t size)
{
   HAL_PCD_EP_Transmit(pdev->pData, ep_addr, pbuf, size);
   return USBD_OK;
}

/**
 * @brief  Prepares an endpoint for reception.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @param  pbuf: Pointer to data to be received
 * @param  size: Data size
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_PrepareReceive(USBD_HandleTypeDef* pdev, uint8_t ep_addr, uint8_t* pbuf, uint16_t size)
{
   HAL_PCD_EP_Receive(pdev->pData, ep_addr, pbuf, size);
   return USBD_OK;
}

/**
 * @brief  Returns the last transferred packet size.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval Received Data Size
 */
uint32_t USBD_LL_GetRxDataSize(USBD_HandleTypeDef* pdev, uint8_t ep_addr)
{
   return HAL_PCD_EP_GetRxCount(pdev->pData, ep_addr);
}

/**
 * @brief  Configures system clock after wakeup from STOP mode.
 * @param  None
 * @retval None
 */
void SystemClockConfig_STOP(void)
{
   RCC_ClkInitTypeDef RCC_ClkInitStruct;
   RCC_OscInitTypeDef RCC_OscInitStruct;

   /* Enable HSE Oscillator and activate PLL with HSE as source */
   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
   RCC_OscInitStruct.HSEState = RCC_HSE_ON;
   RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
   RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
   RCC_OscInitStruct.PLL.PLLM = 8;
   RCC_OscInitStruct.PLL.PLLN = 432;
   RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
   RCC_OscInitStruct.PLL.PLLQ = 9;
   HAL_RCC_OscConfig(&RCC_OscInitStruct);

   /* Activate the OverDrive to reach the 216 Mhz Frequency */
   HAL_PWREx_EnableOverDrive();

   /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
      clocks dividers */
   RCC_ClkInitStruct.ClockType =
      (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
   RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
   RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
   RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
   RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
   HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
}

/**
 * @brief  GPIO EXTI Callback function
 *         Handle remote-wakeup through user button
 * @param  GPIO_Pin
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
   if (GPIO_Pin == KEY_BUTTON_PIN)
   {
      if ((((USBD_HandleTypeDef*)hpcd.pData)->dev_remote_wakeup == 1) &&
          (((USBD_HandleTypeDef*)hpcd.pData)->dev_state == USBD_STATE_SUSPENDED))
      {
         if ((&hpcd)->Init.low_power_enable)
         {
            /* Reset SLEEPDEEP bit of Cortex System Control Register */
            SCB->SCR &= (uint32_t) ~((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));

            SystemClockConfig_STOP();
         }

         /* Ungate PHY clock */
         __HAL_PCD_UNGATE_PHYCLOCK((&hpcd));

         /* Activate Remote wakeup */
         HAL_PCD_ActivateRemoteWakeup((&hpcd));

         /* Remote wakeup delay */
         HAL_Delay(10);

         /* Disable Remote wakeup */
         HAL_PCD_DeActivateRemoteWakeup((&hpcd));

         /* change state to configured */
         ((USBD_HandleTypeDef*)hpcd.pData)->dev_state = USBD_STATE_CONFIGURED;

         /* Change remote_wakeup feature to 0*/
         ((USBD_HandleTypeDef*)hpcd.pData)->dev_remote_wakeup = 0;
      }
      else
      {
         GetPointerData(HID_Buffer);
         USBD_HID_SendReport(&USBD_Device, HID_Buffer, 4);
      }
   }
}

/**
 * @brief  Gets Pointer Data.
 * @param  pbuf: Pointer to report
 * @retval None
 */
void GetPointerData(uint8_t* pbuf)
{
   static int8_t cnt = 0;
   int8_t x = 0, y = 0;

   if (cnt++ > 0)
   {
      x = CURSOR_STEP;
   }
   else
   {
      x = -CURSOR_STEP;
   }
   pbuf[0] = 0;
   pbuf[1] = x;
   pbuf[2] = y;
   pbuf[3] = 0;
}

/**
 * @brief  Delays routine for the USB Device Library.
 * @param  Delay: Delay in ms
 * @retval None
 */
void USBD_LL_Delay(uint32_t Delay)
{
   HAL_Delay(Delay);
}

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/