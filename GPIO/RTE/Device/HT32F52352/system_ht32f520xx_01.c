/**************************************************************************//**
 * @file    library/Device/Holtek/HT32F520xx/Source/system_ht32f520xx_01.c
 * @brief   CMSIS Cortex-M0+ Device Peripheral Access Layer Source File
 *          for the Holtek HT32F520xx Device Series
 * @version $Rev:: 644          $
 * @date    $Date:: 2016-05-25 #$
 *
 * @note
 * Copyright (C) Holtek Semiconductor Inc. All rights reserved.
 *
 * @par
 * ARM Limited (ARM) supplies this software for Cortex-M processor-based
 * microcontrollers. This file can be freely distributed within development
 * tools that are supporting such ARM based processors.
 *
 * @par
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/

// Supported Device
// ========================================
//   HT32F52220, HT32F52230
//   HT32F52231, HT32F52241
//   HT32F52331, HT32F52341
//   HT32F52342. HT32F52352

//#define USE_HT32F52220_30
//#define USE_HT32F52231_41
//#define USE_HT32F52331_41
//#define USE_HT32F52342_52

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup HT32F520xx_system HT32F520xx System
  * @{
  */


#include "ht32f520xx_01.h"

/** @addtogroup HT32F520xx_System_Private_Defines
  * @{
  */
/*
//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
*/

/*--------------------- Clock Configuration ----------------------------------
//
//    <q0> Enable High Speed Internal RC Oscillator(HSI)
//          <i> Default HSI = ENABLE
//
//    <q1> Enable High Speed External Crystal Oscillator(HSE)
//          <i> Default HSE = DISABLE
//
//    <q3> Enable Low Speed External Crystal Oscillator (LSE)
//          <i> Default LSE = DISABLE
//          <i> Not apply to HT32F52220/30
//
//    <e4> Enable PLL
//                <i> Default PLL = DISABLE
//        <o5>  PLL Clock Source
//                <0=> CK_HSE
//                <1=> CK_HSI
//                <i> Default PLL clock source = CK_HSI
//                <i> PLL source clock must be in the range of 4 MHz to 16 MHz
//        <o6>  PLL Feedback Clock Divider (NF2): 1 ~ 16
//                <1-16:1>
//                <i> PLL feedback clock = PLL clock source x NF2
//                <i> PLL feedback clock must be in the range of 16 MHz to 48 MHz
//        <o7>  PLL Output Clock Divider (NO2)
//                <0=> 1
//                <1=> 2
//                <2=> 4
//                <3=> 8
//                <i> PLL output clock = PLL feedback clock / NO2
//                <i> PLL output clock must be in the range of 2 MHz to 48 MHz
//      </e>
//
//    <h> SystemCoreClock Configuration (CK_AHB)
//      <o8>    SystemCoreClock Source
//                <1=> CK_PLL
//                <2=> CK_HSE
//                <3=> CK_HSI
//                <6=> CK_LSE (Not apply to HT32F52220/30)
//                <7=> CK_LSI
//                <i> Default SystemCoreClock source = CK_HSI
//      <o9>    SystemCoreClock Source Divider
//                <0=> 1
//                <1=> 2
//                <2=> 4
//                <3=> 8
//                <4=> 16
//                <5=> 32
//                <i> Default SystemCoreClock source divider = 1
//    </h>
//
//    <h> FLASH Configuration
//      <o10>   Wait state
//                <0=> 0 WS
//                <1=> 1 WS
//                <4=> AUTO
//                <i> HT32F52331/41/42/52
//                <i>   0 WS:  1 KHz <= CK_AHB <= 24 MHz
//                <i>   1 WS: 24 MHz <  CK_AHB <= 48 MHz
//                <i> HT32F52220/30/31/41
//                <i>   0 WS:  1 KHz <= CK_AHB <= 20 MHz
//                <i>   1 WS: 20 MHz <  CK_AHB <= 40 MHz
//      <q11>   Pre-fetch Buffer Enable
//                <i> Default pre-fetch buffer = ENABLE
//      <q12>   Branch cache Enable (HT3F52342/52 Only)
//                <i> Default branch cache = ENABLE
//    </h>
//
//      <o13>   Isolation State
//                <0=> ISOLATED
//                <1=> NON-ISOLATED
//                <i> Default Isolation state = ISOLATED
*/
#define HSI_ENABLE        (1)     /*!< 0: DISABLE,  1: ENABLE                                               */
#define HSE_ENABLE        (1)     /*!< 0: DISABLE,  1: ENABLE                                               */
#define LSI_ENABLE        (1)     /*!< 0: DISABLE,  1: ENABLE                                               */
#define LSE_ENABLE        (0)     /*!< 0: DISABLE,  1: ENABLE                                               */
#define PLL_ENABLE        (1)     /*!< 0: DISABLE,  1: ENABLE                                               */
#define PLL_CLK_SRC       (0)     /*!< 0: HSE,      1: HSI                                                  */
#define PLL_NF2_DIV       (6)     /*!< 1~16: DIV1~DIV16                                                     */
#define PLL_NO2_DIV       (0)     /*!< 0: DIV1,     1: DIV2,   2: DIV4,   3: DIV8                           */
#define HCLK_SRC          (1)     /*!< 0: PLL,      1: PLL,    2: HSE,    3: HSI     6: LSE,    7: LSI      */
#define HCLK_DIV          (0)     /*!< 0: DIV1,     1: DIV2,   2: DIV4,   3: DIV8,   4: DIV16,  5: DIV32    */
#define WAIT_STATE        (4)     /*!< 0: WS = 0,   1: WS = 1,                       4: WS = AUTO           */
#define PRE_FETCH_ENABLE  (1)     /*!< 0: DISABLE,  1: ENABLE                                               */
#define BCACHE_ENABLE     (1)     /*!< 0: DISABLE,  1: ENABLE                                               */
#define ISO_CTRL          (1)     /*!< 0: ISOLATED, 1: NON-ISOLATED                                         */

/*--------------------- WDT Configuration ----------------------------------
//
//    <e0> Enable WDT Configuration
//      <o1> WDT Prescaler Selection
//           <0=> CK_WDT / 1
//           <1=> CK_WDT / 2
//           <2=> CK_WDT / 4
//           <3=> CK_WDT / 8
//           <4=> CK_WDT / 16
//           <5=> CK_WDT / 32
//           <6=> CK_WDT / 64
//           <7=> CK_WDT / 128
//      <o2> WDT Reload Value <1-4095:1>
//      <q3> Enable WDT Reset function
//      <o4> WDT Sleep Halt mode
//           <0=> No halt
//           <1=> Halt in DeepSleep1
//           <2=> Halt in Sleep & DeepSleep1
//   </e>
*/
#define WDT_ENABLE        (0)     /*!< 0: DISABLE,  1: ENABLE                                               */
#define WDT_PRESCALER     (5)     /*!< 0: 1/1, 1: 1/2, 2: 1/4, 3: 1/8, 4: 1/16, 5: 1/32, 6: 1/64, 7: 1/128  */
#define WDT_RELOAD        (2000)  /*!< 0 ~ 4095, 12 bit                                                     */
#define WDT_RESET_ENABLE  (1)     /*!< 0: No Reset, 1: Reset when WDT over flow                             */
#define WDT_SLEEP_HALT    (2)     /*!< 0: No halt,  1: Halt in DeepSleep1, 2: Halt in Sleep & DeepSleep1    */

/**
 * @brief Check HSI frequency
 */
#if (HSI_VALUE != 8000000UL)
  #error "CK_HSI clock issue: must be 8 MHz!"
#endif

/**
 * @brief Check HSE frequency
 */
#if ((HSE_VALUE < 4000000UL) || (HSE_VALUE > 16000000UL))
  #error "CK_HSE clock issue: must be in the range of 4 MHz to 16 MHz!"
#endif

/**
 * @brief Check LSI frequency
 */
#if (LSI_VALUE != 32000UL)
  #error "CK_LSI clock issue: must be 32 KHz!"
#endif

/**
 * @brief Check LSE frequency
 */
#if defined(USE_HT32F52231_41) || defined(USE_HT32F52331_41) || defined(USE_HT32F52342_52)
#if (LSE_VALUE != 32768UL)
  #error "CK_LSE clock issue: must be 32.768 KHz!"
#endif
#endif

/**
 * @brief CK_PLL definition
 */
#if (PLL_ENABLE == 1)
  /* Get CK_VCO frequency                                                                                   */
  #if (PLL_CLK_SRC == 1)
    #if (HSI_ENABLE == 0)
      #error "CK_PLL clock source issue: HSI has not been enabled"
    #else
      #define __CK_VCO    (HSI_VALUE * PLL_NF2_DIV)             /*!< Select HSI as PLL source               */
    #endif
  #else
    #if (HSE_ENABLE == 0)
      #error "CK_PLL clock source issue: HSE has not been enabled!"
    #else
      #define __CK_VCO    (HSE_VALUE * PLL_NF2_DIV)             /*!< Select HSE as PLL source               */
    #endif
  #endif

  #define VCO_MIN 16000000UL
  #define VCO_MAX 48000000UL
  #define PLL_MIN  2000000UL
  #define PLL_MAX 48000000UL

  /* Check CK_VCO frequency                                                                                 */
  #if ((__CK_VCO < VCO_MIN) || (__CK_VCO > VCO_MAX))
    #error "CK_VCO clock issue: must be in the range!"
  #endif

  #define __CK_PLL    (__CK_VCO >> PLL_NO2_DIV)                 /*!< Get CK_PLL frequency                   */

  /* Check CK_PLL frequency                                                                                 */
  #if ((__CK_PLL < PLL_MIN) || (__CK_PLL > PLL_MAX))
    #error "CK_PLL clock issue: must be in the range!"
  #endif
#endif

/**
 * @brief CK_SYS definition
 */
#if (HCLK_SRC == 1)
  #if (PLL_ENABLE == 1)
    #define __CK_SYS    __CK_PLL              /*!< Select PLL as CK_SYS source                              */
  #else
    #error "CK_SYS clock source issue: PLL is not enable!"
  #endif
#elif (HCLK_SRC == 2)
  #if (HSE_ENABLE == 1)
    #define __CK_SYS    HSE_VALUE             /*!< Select HSE as CK_SYS source                              */
  #else
    #error "CK_SYS clock source issue: HSE is not enable!"
  #endif
#elif (HCLK_SRC == 3)
  #if (HSI_ENABLE == 1)
    #define __CK_SYS    HSI_VALUE             /*!< Select HSI as CK_SYS source                              */
  #else
    #error "CK_SYS clock source issue: HSI is not enable!"
  #endif
#elif (HCLK_SRC == 6)
  #ifdef USE_HT32F52220_30
    #error "CK_LSE can not be the source of SystemCoreClock for HT32F52220/30"
  #else
  #if (LSE_ENABLE == 1)
    #define __CK_SYS    LSE_VALUE             /*!< Select LSE as CK_SYS source                              */
  #else
    #error "CK_SYS clock source issue: LSE is not enable!"
  #endif
  #endif
#elif (HCLK_SRC == 7)
  #if (LSI_ENABLE == 1)
    #define __CK_SYS    LSI_VALUE             /*!< Select LSI as CK_SYS source                              */
  #else
    #error "CK_SYS clock source issue: LSI is not enable!"
  #endif
#else
  #error "CK_SYS clock source issue: No clock source is selected!"
#endif

/**
 * @brief CK_AHB definition
 */
#define __CK_AHB    (__CK_SYS >> HCLK_DIV)    /*!< Get CK_AHB frequency                                     */

#if defined(USE_HT32F52331_41) || defined(USE_HT32F52342_52)
  #define CKAHB_MIN 1000UL
  #define CKAHB_MAX 48000000UL
  #define WS0_CLK   24000000UL
#endif

#if defined(USE_HT32F52220_30) || defined(USE_HT32F52231_41)
  #define CKAHB_MIN 1000UL
  #define CKAHB_MAX 40000000UL
  #define WS0_CLK   20000000UL
#endif

/* Check CK_AHB frequency                                                                                   */
#if ((__CK_AHB < CKAHB_MIN) || (__CK_AHB > CKAHB_MAX))
  #error "CK_AHB clock issue: must be in the range!"
#endif

/* Check FLASH wait-state setting                                                                           */
#if ((__CK_AHB > WS0_CLK) && (WAIT_STATE < 1))
  #error "FLASH wait state configuration issue!"
#endif
/**
  * @}
  */

/** @addtogroup HT32F520xx_System_Private_Variables
  * @{
  */
__IO uint32_t SystemCoreClock = __CK_AHB;   /*!< SystemCoreClock = CK_AHB                                   */
/**
  * @}
  */

/** @addtogroup HT32F520xx_System_Private_Functions
  * @{
  */

/**
  * @brief  Setup the microcontroller system.
  *         Initializes the system clocks and the embedded Flash.
  * @note   This function should be used after reset.
  * @retval None
  */
void SystemInit(void)
{
  if ((HT_FLASH->CID[0] == 0) && (HT_FLASH->CID[1] == 0) && (HT_FLASH->CID[2] == 0) && (HT_FLASH->CID[3] == 0)) {
    __DSB(); ww(0xE000ED0C, 0x05FA0004); __DSB(); while (1);
  }

#if (WDT_ENABLE == 1)
  HT_CKCU->APBCCR1 |= (0x1 << 4);
  HT_WDT->MR1 = ((HT_WDT->MR1 & 0xFFF) | (WDT_PRESCALER << 12));
  HT_WDT->MR0 = WDT_RELOAD | (WDT_RESET_ENABLE << 13) | (WDT_SLEEP_HALT << 14) | (0x1 << 16);
  HT_WDT->CR = 0x5FA00001;
#endif

#if (ISO_CTRL == 1)
  HT_CKCU->LPCR = 1;                                            /* configure Backup domain isolation        */
#endif

#if (WDT_ENABLE == 1) && defined(USE_HT32F52342_52)
  HT_CKCU->LPCR = 1;
  HT_CKCU->APBCCR1 |= (0x1 << 6);
  while (HT_PWRCU->TEST != 0x27);
  HT_RTC->CR &= ~(1UL << 2);
  while (HT_CKCU->GCSR & 0x20);
  HT_RTC->CR |= (1UL << 2);
  HT_CKCU->APBCCR1 &= ~(0x1 << 6);
  #if (ISO_CTRL == 0)
  HT_CKCU->LPCR = 0;
  #endif
#endif

  /* HSE initiation                                                                                         */
#if (HSE_ENABLE == 1)
  SetBit_BB((u32)(&HT_CKCU->GCCR), 10);                         /* enable HSE                               */
  while (!GetBit_BB((u32)(&HT_CKCU->GCSR), 2));                 /* wait for HSE ready                       */
#endif

  /* LSE initiation                                                                                         */
#if defined(USE_HT32F52231_41) || defined(USE_HT32F52331_41) || defined(USE_HT32F52342_52)
#if (LSE_ENABLE == 1)
  SetBit_BB((u32)(&HT_CKCU->APBCCR1), 6);                       /* enable power domain register clock       */
  while (HT_PWRCU->TEST != 0x27);                               /* wait for power domain register ready     */
  do {
    SetBit_BB((u32)(&HT_RTC->CR), 3);                           /* enable LSE                               */
  } while (!GetBit_BB((u32)(&HT_RTC->CR), 3));
  while (!GetBit_BB((u32)(&HT_CKCU->GCSR), 4));                 /* wait for LSE ready                       */
#endif
#endif
  /* LSI initiation                                                                                         */
#if (HCLK_SRC == 7)
  while (!GetBit_BB((u32)(&HT_CKCU->GCSR), 5));                 /* wait for LSI ready                       */
#endif

  /* PLL initiation                                                                                         */
#if (PLL_ENABLE == 1)
  #if (PLL_CLK_SRC == 0)
    ResetBit_BB((u32)(&HT_CKCU->GCFGR), 8);                     /* select PLL source as HSE                 */
  #endif

  HT_CKCU->PLLCFGR = ((PLL_NF2_DIV & 0x0F) << 23) | (PLL_NO2_DIV << 21);  /* set PLL divider                */
  SetBit_BB((u32)(&HT_CKCU->GCCR), 9);                                    /* enable PLL                     */
  while (!GetBit_BB((u32)(&HT_CKCU->GCSR), 1));                           /* wait for PLL ready             */
#endif

  /* CK_AHB initiation                                                                                      */
#if (WAIT_STATE == 4)
  #if (__CK_AHB > WS0_CLK)
    HT_FLASH->CFCR = (((HT_FLASH->CFCR) & ~7UL) | 2UL);         /* auto-select wait state                   */
  #endif
#else
  HT_FLASH->CFCR = (((HT_FLASH->CFCR) & ~7UL) | (WAIT_STATE + 1));        /* manual wait state              */
#endif

#if (HCLK_DIV != 0)
  HT_CKCU->AHBCFGR = HCLK_DIV;                                  /* set CK_AHB prescaler                     */
#endif

#if (HCLK_SRC != 3)
  HT_CKCU->GCCR = ((HT_CKCU->GCCR & ~7UL) | HCLK_SRC);          /* select CK_SYS source                     */
  while ((HT_CKCU->CKST & 7UL) != HCLK_SRC);                    /* wait for clock switch complete           */
#endif

  /* Pre-fetch buffer configuration                                                                         */
#if (PRE_FETCH_ENABLE == 0)
  ResetBit_BB((u32)(&HT_FLASH->CFCR), 4);               /* 0: pre-fetch disable, 1: pre-fetch enable        */
#endif

  /* Branch cache configuration                                                                             */
#ifdef USE_HT32F52342_52
#if (BCACHE_ENABLE == 0)
  ResetBit_BB((u32)(&HT_FLASH->CFCR), 12);              /* 0: branch cache disable, 1: branch cache enable  */
#endif
#endif

  /* HSI power down                                                                                         */
#if ((HSI_ENABLE == 0) && (HCLK_SRC != 3) && ((PLL_ENABLE == 0) || (PLL_CLK_SRC == 0)))
  ResetBit_BB((u32)(&HT_CKCU->GCCR), 11);
#endif
}

/**
  * @brief  Update SystemCoreClock
  * @retval None
  */
void SystemCoreClockUpdate(void)
{
  u32 SystemCoreClockDiv = HT_CKCU->AHBCFGR & 7UL;
  u32 PllFeedbackClockDiv = ((HT_CKCU->PLLCFGR >> 23) == 0) ? (16) : (HT_CKCU->PLLCFGR >> 23);
  u32 PllOutputClockDiv = (HT_CKCU->PLLCFGR >> 21) & 3UL;
  u32 SystemCoreClockSrc = HT_CKCU->CKST & 7UL;

  /* Get system core clock according to global clock control & configuration registers                      */
  if (SystemCoreClockSrc == 1)
  {
    if (GetBit_BB((u32)(&HT_CKCU->PLLCR), 31))
    {
      PllFeedbackClockDiv = 1;
      PllOutputClockDiv = 0;
    }

    if (GetBit_BB((u32)(&HT_CKCU->GCFGR), 8))
    {
      SystemCoreClock = ((HSI_VALUE * PllFeedbackClockDiv) >> PllOutputClockDiv) >> SystemCoreClockDiv;
    }
    else
    {
      SystemCoreClock = ((HSE_VALUE * PllFeedbackClockDiv) >> PllOutputClockDiv) >> SystemCoreClockDiv;
    }
  }
  else if (SystemCoreClockSrc == 2)
  {
    SystemCoreClock = HSE_VALUE >> SystemCoreClockDiv;
  }
  else if (SystemCoreClockSrc == 3)
  {
    SystemCoreClock = HSI_VALUE >> SystemCoreClockDiv;
  }
  #ifndef USE_HT32F52220_30
  else if (SystemCoreClockSrc == 6)
  {
    SystemCoreClock = LSE_VALUE >> SystemCoreClockDiv;
  }
  #endif
  else if (SystemCoreClockSrc == 7)
  {
    SystemCoreClock = LSI_VALUE >> SystemCoreClockDiv;
  }
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

/******************* (C) COPYRIGHT Holtek Semiconductor Inc. *****END OF FILE***                            */
