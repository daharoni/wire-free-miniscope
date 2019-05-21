/**
 * \file
 *
 * \brief User board configuration template
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef CONF_BOARD_H
#define CONF_BOARD_H

#define CONF_BOARD_SD_MMC_HSMCI

#    define SD_MMC_HSMCI_MEM_CNT          1
// Optional card detect pin and write protection pin
//#    define SD_MMC_0_CD_GPIO              IOPORT_CREATE_PIN(PIOA,6) //(PIO_PA6_IDX)
//#    define SD_MMC_0_CD_PIO_ID            ID_PIOA
//#    define SD_MMC_0_CD_FLAGS             (PIO_INPUT | PIO_PULLUP)
//#    define SD_MMC_0_CD_DETECT_VALUE      0

#    define SD_MMC_HSMCI_SLOT_0_SIZE      4 // 4-bits connector pin
#    define PIN_HSMCI_MCCDA_GPIO          (PIO_PA28_IDX)
#    define PIN_HSMCI_MCCDA_FLAGS         (IOPORT_MODE_MUX_C) //(PIO_PERIPH_C | PIO_DEFAULT)
#    define PIN_HSMCI_MCCK_GPIO           (PIO_PA25_IDX) //(PIO_PA29_IDX)
#    define PIN_HSMCI_MCCK_FLAGS          (IOPORT_MODE_MUX_D) //(PIO_PERIPH_D | PIO_DEFAULT)
#    define PIN_HSMCI_MCDA0_GPIO          (PIO_PA30_IDX)
#    define PIN_HSMCI_MCDA0_FLAGS         (IOPORT_MODE_MUX_C) //(PIO_PERIPH_C | PIO_DEFAULT)
#    define PIN_HSMCI_MCDA1_GPIO          (PIO_PA31_IDX)
#    define PIN_HSMCI_MCDA1_FLAGS         (IOPORT_MODE_MUX_C) //(PIO_PERIPH_C | PIO_DEFAULT)
#    define PIN_HSMCI_MCDA2_GPIO          (PIO_PA26_IDX)
#    define PIN_HSMCI_MCDA2_FLAGS         (IOPORT_MODE_MUX_C) //(PIO_PERIPH_C | PIO_DEFAULT)
#    define PIN_HSMCI_MCDA3_GPIO          (PIO_PA27_IDX)
#    define PIN_HSMCI_MCDA3_FLAGS         (IOPORT_MODE_MUX_C) //(PIO_PERIPH_C | PIO_DEFAULT)


#endif // CONF_BOARD_H
