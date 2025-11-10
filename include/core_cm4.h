#ifndef __CORE_CM4_H_GENERIC
#define __CORE_CM4_H_GENERIC

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* IO definitions */
#ifdef __cplusplus
#define __I volatile /*!< Defines 'read only' permissions */
#else
#define __I volatile const /*!< Defines 'read only' permissions */
#endif
#define __O volatile /*!< Defines 'write only' permissions */
#define __IO volatile /*!< Defines 'read / write' permissions */

/* Memory mapping of Core Hardware */
#define SCS_BASE (0xE000E000UL) /*!< System Control Space Base Address */
#define SysTick_BASE (SCS_BASE + 0x0010UL) /*!< SysTick Base Address */
#define SCB_BASE (SCS_BASE + 0x0D00UL) /*!< System Control Block Base Address */

typedef struct {
	__IO uint32_t
		CTRL; /*!< Offset: 0x000 (R/W)  SysTick Control and Status Register */
	__IO uint32_t
		LOAD; /*!< Offset: 0x004 (R/W)  SysTick Reload Value Register */
	__IO uint32_t
		VAL; /*!< Offset: 0x008 (R/W)  SysTick Current Value Register */
	__I uint32_t
		CALIB; /*!< Offset: 0x00C (R/ )  SysTick Calibration Register */
} SysTick_Type;

typedef struct {
	__I uint32_t CPUID; /*!< Offset: 0x000 (R/ )  CPUID Base Register */
	__IO uint32_t
		ICSR; /*!< Offset: 0x004 (R/W)  Interrupt Control and State Register */
	__IO uint32_t
		VTOR; /*!< Offset: 0x008 (R/W)  Vector Table Offset Register */
	// ... other registers ...
} SCB_Type;

#define SysTick \
	((SysTick_Type *)SysTick_BASE) /*!< SysTick configuration struct */
#define SCB ((SCB_Type *)SCB_BASE) /*!< SCB configuration struct */

/* SCB Interrupt Control State Register Definitions */
#define SCB_ICSR_PENDSVSET_Pos 28U /*!< SCB ICSR: PENDSVSET Position */
#define SCB_ICSR_PENDSVSET_Msk \
	(1UL << SCB_ICSR_PENDSVSET_Pos) /*!< SCB ICSR: PENDSVSET Mask */

/* Intrinsic Functions */
__attribute__((always_inline)) static inline void __enable_irq(void)
{
	__asm volatile("cpsie i" : : : "memory");
}

__attribute__((always_inline)) static inline void __disable_irq(void)
{
	__asm volatile("cpsid i" : : : "memory");
}

#ifdef __cplusplus
}
#endif

#endif /* __CORE_CM4_H_GENERIC */
