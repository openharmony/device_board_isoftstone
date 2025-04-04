/*
 * (C) Copyright 2007-2013
 * Reuuimlla Technology Co., Ltd. <www.reuuimllatech.com>
 * Aaron.Maoye <leafy.myeh@reuuimllatech.com>
 *
 * Some macro and struct of Allwinner UART controller.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * 2013.6.6 Mintow <duanmintao@allwinnertech.com>
 *    Adapt to support sun8i/sun9i of Allwinner.
 */

#ifndef _SUNXI_UART_H_
#define _SUNXI_UART_H_

#include <linux/regulator/consumer.h>
#include <linux/dmaengine.h>
#include <linux/reset.h>
//include <linux/serial_core.h>

/* SUNXI UART PORT definition*/
#define PORT_MAX_USED	PORT_LINFLEXUART  /* see include/uapi/linux/serial_core.h */
#define PORT_SUNXI	(PORT_MAX_USED + 1)

struct sw_uart_pdata {
	unsigned int used;
	unsigned int io_num;
	unsigned int port_no;
	char regulator_id[16];
	struct regulator *regulator;
};

#if IS_ENABLED(CONFIG_SERIAL_SUNXI_DMA)
struct sw_uart_dma {
	u32 use_dma; /* 1:used */

	/* receive and transfer buffer */
	char *rx_buffer; /* visual memory */
	char *tx_buffer;
	dma_addr_t rx_phy_addr; /* physical memory */
	dma_addr_t tx_phy_addr;
	u32 rb_size; /* buffer size */
	u32 tb_size;

	/* regard the rx buffer as a circular buffer */
	u32 rb_head;
	u32 rb_tail;
	u32 rx_size;

	dma_cookie_t rx_cookie;

	char tx_dma_inited; /* 1:dma tx channel has been init */
	char rx_dma_inited; /* 1:dma rx channel has been init */
	char tx_dma_used;   /* 1:dma tx is working */
	char rx_dma_used;   /* 1:dma rx is working */

	/* timer to poll activity on rx dma */
	char use_timer;
	int rx_timeout;

	struct dma_chan *dma_chan_rx, *dma_chan_tx;
	struct scatterlist rx_sgl, tx_sgl;
	unsigned int		rx_bytes, tx_bytes;
};
#endif

struct sw_uart_port {
	struct uart_port port;
	char   name[16];
	struct clk *mclk;
	struct clk *sclk;
	struct clk *pclk;
	struct reset_control *reset;
	unsigned char id;
	unsigned char ier;
	unsigned char lcr;
	unsigned char mcr;
	unsigned char fcr;
	unsigned char dll;
	unsigned char dlh;
	unsigned char rs485;
	unsigned char msr_saved_flags;
	unsigned int lsr_break_flag;
	struct sw_uart_pdata *pdata;
#if IS_ENABLED(CONFIG_SERIAL_SUNXI_DMA)
	struct sw_uart_dma *dma;
	struct hrtimer rx_hrtimer;
	u32 rx_last_pos;
#define SUNXI_UART_DRQ_RX(ch)		(DRQSRC_UART0_RX + ch)
#define SUNXI_UART_DRQ_TX(ch)		(DRQDST_UART0_TX + ch)
#endif

	/* for debug */
#define MAX_DUMP_SIZE	1024
	unsigned int dump_len;
	char *dump_buff;
	struct proc_dir_entry *proc_root;
	struct proc_dir_entry *proc_info;

	struct pinctrl *pctrl;
	struct serial_rs485 rs485conf;
	bool card_print;
	bool throttled;
};

/* register offset define */
#define SUNXI_UART_RBR (0x00) 		/* receive buffer register */
#define SUNXI_UART_THR (0x00) 		/* transmit holding register */
#define SUNXI_UART_DLL (0x00) 		/* divisor latch low register */
#define SUNXI_UART_DLH (0x04) 		/* diviso latch high register */
#define SUNXI_UART_IER (0x04) 		/* interrupt enable register */
#define SUNXI_UART_IIR (0x08) 		/* interrupt identity register */
#define SUNXI_UART_FCR (0x08) 		/* FIFO control register */
#define SUNXI_UART_LCR (0x0c) 		/* line control register */
#define SUNXI_UART_MCR (0x10) 		/* modem control register */
#define SUNXI_UART_LSR (0x14) 		/* line status register */
#define SUNXI_UART_MSR (0x18) 		/* modem status register */
#define SUNXI_UART_SCH (0x1c) 		/* scratch register */
#define SUNXI_UART_USR (0x7c) 		/* status register */
#define SUNXI_UART_TFL (0x80) 		/* transmit FIFO level */
#define SUNXI_UART_RFL (0x84) 		/* RFL */
#define SUNXI_UART_HALT (0xa4) 		/* halt tx register */
#define SUNXI_UART_RS485 (0xc0)		/* RS485 control and status register */

/* register bit field define */
/* Interrupt Enable Register */
#define SUNXI_UART_IER_PTIME (BIT(7))
#define SUNXI_UART_IER_RS485 (BIT(4))
#define SUNXI_UART_IER_MSI   (BIT(3))
#define SUNXI_UART_IER_RLSI  (BIT(2))
#define SUNXI_UART_IER_THRI  (BIT(1))
#define SUNXI_UART_IER_RDI   (BIT(0))
/* Interrupt ID Register */
#define SUNXI_UART_IIR_FEFLAG_MASK (BIT(6)|BIT(7))
#define SUNXI_UART_IIR_IID_MASK    (BIT(0)|BIT(1)|BIT(2)|BIT(3))
 #define SUNXI_UART_IIR_IID_MSTA    (0)
 #define SUNXI_UART_IIR_IID_NOIRQ   (1)
 #define SUNXI_UART_IIR_IID_THREMP  (2)
 #define SUNXI_UART_IIR_IID_RXDVAL  (4)
 #define SUNXI_UART_IIR_IID_LINESTA (6)
 #define SUNXI_UART_IIR_IID_BUSBSY  (7)
 #define SUNXI_UART_IIR_IID_CHARTO  (12)
/* FIFO Control Register */
#define SUNXI_UART_FCR_RXTRG_MASK  (BIT(6)|BIT(7))
 #define SUNXI_UART_FCR_RXTRG_1CH   (0 << 6)
 #define SUNXI_UART_FCR_RXTRG_1_4   (1 << 6)
 #define SUNXI_UART_FCR_RXTRG_1_2   (2 << 6)
 #define SUNXI_UART_FCR_RXTRG_FULL  (3 << 6)
#define SUNXI_UART_FCR_TXTRG_MASK  (BIT(4)|BIT(5))
 #define SUNXI_UART_FCR_TXTRG_EMP   (0 << 4)
 #define SUNXI_UART_FCR_TXTRG_2CH   (1 << 4)
 #define SUNXI_UART_FCR_TXTRG_1_4   (2 << 4)
 #define SUNXI_UART_FCR_TXTRG_1_2   (3 << 4)
#define SUNXI_UART_FCR_TXFIFO_RST  (BIT(2))
#define SUNXI_UART_FCR_RXFIFO_RST  (BIT(1))
#define SUNXI_UART_FCR_FIFO_EN     (BIT(0))
/* Line Control Register */
#define SUNXI_UART_LCR_DLAB        (BIT(7))
#define SUNXI_UART_LCR_SBC         (BIT(6))
#define SUNXI_UART_LCR_PARITY_MASK (BIT(5)|BIT(4))
 #define SUNXI_UART_LCR_EPAR        (1 << 4)
 #define SUNXI_UART_LCR_OPAR        (0 << 4)
#define SUNXI_UART_LCR_PARITY      (BIT(3))
#define SUNXI_UART_LCR_STOP        (BIT(2))
#define SUNXI_UART_LCR_DLEN_MASK   (BIT(1)|BIT(0))
 #define SUNXI_UART_LCR_WLEN5       (0)
 #define SUNXI_UART_LCR_WLEN6       (1)
 #define SUNXI_UART_LCR_WLEN7       (2)
 #define SUNXI_UART_LCR_WLEN8       (3)
/* Modem Control Register */
#define SUNXI_UART_MCR_MODE_MASK  (BIT(7)|BIT(6))
 #define SUNXI_UART_MCR_MODE_RS485 (2 << 6)
 #define SUNXI_UART_MCR_MODE_SIRE  (1 << 6)
 #define SUNXI_UART_MCR_MODE_UART  (0 << 6)
#define SUNXI_UART_MCR_AFE        (BIT(5))
#define SUNXI_UART_MCR_LOOP       (BIT(4))
#define SUNXI_UART_MCR_RTS        (BIT(1))
#define SUNXI_UART_MCR_DTR        (BIT(0))
/* Line Status Rigster */
#define SUNXI_UART_LSR_RXFIFOE    (BIT(7))
#define SUNXI_UART_LSR_TEMT       (BIT(6))
#define SUNXI_UART_LSR_THRE       (BIT(5))
#define SUNXI_UART_LSR_BI         (BIT(4))
#define SUNXI_UART_LSR_FE         (BIT(3))
#define SUNXI_UART_LSR_PE         (BIT(2))
#define SUNXI_UART_LSR_OE         (BIT(1))
#define SUNXI_UART_LSR_DR         (BIT(0))
#define SUNXI_UART_LSR_BRK_ERROR_BITS 0x1E /* BI, FE, PE, OE bits */
/* Modem Status Register */
#define SUNXI_UART_MSR_DCD        (BIT(7))
#define SUNXI_UART_MSR_RI         (BIT(6))
#define SUNXI_UART_MSR_DSR        (BIT(5))
#define SUNXI_UART_MSR_CTS        (BIT(4))
#define SUNXI_UART_MSR_DDCD       (BIT(3))
#define SUNXI_UART_MSR_TERI       (BIT(2))
#define SUNXI_UART_MSR_DDSR       (BIT(1))
#define SUNXI_UART_MSR_DCTS       (BIT(0))
#define SUNXI_UART_MSR_ANY_DELTA  0x0F
#define MSR_SAVE_FLAGS SUNXI_UART_MSR_ANY_DELTA
/* Status Register */
#define SUNXI_UART_USR_RFF        (BIT(4))
#define SUNXI_UART_USR_RFNE       (BIT(3))
#define SUNXI_UART_USR_TFE        (BIT(2))
#define SUNXI_UART_USR_TFNF       (BIT(1))
#define SUNXI_UART_USR_BUSY       (BIT(0))
/* Halt Register */
#define SUNXI_UART_HALT_PTE       (BIT(7))
#define SUNXI_UART_HALT_LCRUP     (BIT(2))
#define SUNXI_UART_HALT_FORCECFG  (BIT(1))
#define SUNXI_UART_HALT_HTX       (BIT(0))
/* RS485 Control and Status Register */
#define SUNXI_UART_RS485_RXBFA    (BIT(3))
#define SUNXI_UART_RS485_RXAFA    (BIT(2))

/* The global infor of UART channel. */

#if IS_ENABLED(CONFIG_ARCH_SUN50IW10)
#define SUNXI_UART_NUM			8
#endif

#if IS_ENABLED(CONFIG_ARCH_SUN8IW15)
#define SUNXI_UART_NUM			5
#endif

#if IS_ENABLED(CONFIG_ARCH_SUN8IW20) || IS_ENABLED(CONFIG_ARCH_SUN20IW1) || IS_ENABLED(CONFIG_ARCH_SUN50IW9)
#define SUNXI_UART_NUM			6
#endif

#ifndef SUNXI_UART_NUM
#define SUNXI_UART_NUM			1
#endif

/* In 50/39 FPGA, two UART is available, but they share one IRQ.
   So we define the number of UART port as 1. */
#if !IS_ENABLED(CONFIG_AW_IC_BOARD)
#undef SUNXI_UART_NUM
#define SUNXI_UART_NUM			1
#endif

#define SUNXI_UART_FIFO_SIZE		64

#define SUNXI_UART_DEV_NAME		"uart"

struct platform_device *sw_uart_get_pdev(int uart_id);

#endif /* end of _SUNXI_UART_H_ */
