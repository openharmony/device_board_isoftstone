/*
 * Misc system wide definitions
 *
 * Copyright (C) 1999-2017, Broadcom Corporation
 *
 *      Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed to you
 * under the terms of the GNU General Public License version 2 (the "GPL"),
 * available at http://www.broadcom.com/licenses/GPLv2.php, with the
 * following added to such license:
 *
 *      As a special exception, the copyright holders of this software give you
 * permission to link this software with independent modules, and to copy and
 * distribute the resulting executable under terms of your choice, provided that
 * you also meet, for each linked independent module, the terms and conditions of
 * the license of that module.  An independent module is a module which is not
 * derived from this software.  The special exception does not apply to any
 * modifications of the software.
 *
 *      Notwithstanding the above, under no circumstances may you combine this
 * software in any way with any other Broadcom software provided under a license
 * other than the GPL, without Broadcom's express prior written consent.
 *
 *
 * <<Broadcom-WL-IPTag/Open:>>
 *
 * $Id: bcmdefs.h 657791 2016-09-02 15:14:42Z $
 */

#ifndef    _bcmdefs_h_
#define    _bcmdefs_h_

/*
 * One doesn't need to include this file explicitly, gets included automatically if
 * typedefs.h is included.
 */

/* Use BCM_REFERENCE to suppress warnings about intentionally-unused function
 * arguments or local variables.
 */
#define BCM_REFERENCE(data)    ((void)(data))

/* Allow for suppressing unused variable warnings. */
#ifdef __GNUC__
#define UNUSED_VAR     __attribute__ ((unused))
#else
#define UNUSED_VAR
#endif

/* Compile-time assert can be used in place of ASSERT if the expression evaluates
 * to a constant at compile time.
 */
#define STATIC_ASSERT(expr) { \
    /* Make sure the expression is constant. */ \
    typedef enum { _STATIC_ASSERT_NOT_CONSTANT = (expr) } _static_assert_e UNUSED_VAR; \
    /* Make sure the expression is true. */ \
    typedef char STATIC_ASSERT_FAIL[(expr) ? 1 : -1] UNUSED_VAR; \
}

/* Reclaiming text and data :
 * The following macros specify special linker sections that can be reclaimed
 * after a system is considered 'up'.
 * BCMATTACHFN is also used for detach functions (it's not worth having a BCMDETACHFN,
 * as in most cases, the attach function calls the detach function to clean up on error).
 */
#if defined(BCM_RECLAIM)

extern bool bcm_reclaimed;
extern bool bcm_attach_part_reclaimed;
extern bool bcm_preattach_part_reclaimed;

#if defined(BCM_RECLAIM_ATTACH_FN_DATA)
#define _data    __attribute__ ((__section__ (".dataini2." #_data))) _data
#define _fn    __attribute__ ((__section__ (".textini2." #_fn), noinline)) _fn

/* Relocate attach symbols to save-restore region to increase pre-reclaim heap size. */
#define BCM_SRM_ATTACH_DATA(_data)    __attribute__ ((__section__ (".datasrm." #_data))) _data
#define BCM_SRM_ATTACH_FN(_fn)        __attribute__ ((__section__ (".textsrm." #_fn), noinline)) _fn

#ifndef PREATTACH_NORECLAIM
#define BCMPREATTACHDATA(_data)    __attribute__ ((__section__ (".dataini3." #_data))) _data
#define BCMPREATTACHFN(_fn)    __attribute__ ((__section__ (".textini3." #_fn), noinline)) _fn
#else
#define BCMPREATTACHDATA(_data)    __attribute__ ((__section__ (".dataini2." #_data))) _data
#define BCMPREATTACHFN(_fn)    __attribute__ ((__section__ (".textini2." #_fn), noinline)) _fn
#endif /* PREATTACH_NORECLAIM  */
#else  /* BCM_RECLAIM_ATTACH_FN_DATA  */
#define _data    _data
#define _fn    _fn
#define BCMPREATTACHDATA(_data)    _data
#define BCMPREATTACHFN(_fn)    _fn
#endif /* BCM_RECLAIM_ATTACH_FN_DATA  */

#if defined(BCM_RECLAIM_INIT_FN_DATA)
#define _data    __attribute__ ((__section__ (".dataini1." #_data))) _data
#define _fn        __attribute__ ((__section__ (".textini1." #_fn), noinline)) _fn
#define CONST
#else /* BCM_RECLAIM_INIT_FN_DATA  */
#define _data    _data
#define _fn        _fn
#ifndef CONST
#define CONST    const
#endif
#endif /* BCM_RECLAIM_INIT_FN_DATA  */

/* Non-manufacture or internal attach function/dat */
#define    BCMNMIATTACHFN(_fn)    _fn
#define    BCMNMIATTACHDATA(_data)    _data

#ifdef BCMNODOWN
#define _fn    _fn
#else
#define _fn    _fn
#endif

#else /* BCM_RECLAIM */

#define bcm_reclaimed               0
#define _data        _data
#define _fn        _fn
#define BCM_SRM_ATTACH_DATA(_data)    _data
#define BCM_SRM_ATTACH_FN(_fn)        _fn
#define BCMPREATTACHDATA(_data)        _data
#define BCMPREATTACHFN(_fn)        _fn
#define _data        _data
#define _fn            _fn
#define _fn        _fn
#define    BCMNMIATTACHFN(_fn)        _fn
#define    BCMNMIATTACHDATA(_data)        _data
#define CONST                const

#endif /* BCM_RECLAIM */

#if !defined STB
#undef BCM47XX_CA9
#endif /* STB */

/* BCMFASTPATH Related Macro defines
*/
#ifndef BCMFASTPATH
#if defined(STB)
#define BCMFASTPATH        __attribute__ ((__section__ (".text.fastpath")))
#define BCMFASTPATH_HOST    __attribute__ ((__section__ (".text.fastpath_host")))
#else
#define BCMFASTPATH
#define BCMFASTPATH_HOST
#endif
#endif /* BCMFASTPATH */


/* Use the BCMRAMFN() macro to tag functions in source that must be included in RAM (excluded from
 * ROM). This should eliminate the need to manually specify these functions in the ROM config file.
 * It should only be used in special cases where the function must be in RAM for *all* ROM-based
 * chips.
 */
    #define BCMRAMFN(_fn)    _fn

#define STATIC    static

/* Bus types */
#define    SI_BUS            0    /* SOC Interconnect */
#define    PCI_BUS            1    /* PCI target */
#define    PCMCIA_BUS        2    /* PCMCIA target */
#define SDIO_BUS        3    /* SDIO target */
#define JTAG_BUS        4    /* JTAG */
#define USB_BUS            5    /* USB (does not support R/W REG) */
#define SPI_BUS            6    /* gSPI target */
#define RPC_BUS            7    /* RPC target */

/* Allows size optimization for single-bus image */
#ifdef BCMBUSTYPE
#define BUSTYPE(bus)    (BCMBUSTYPE)
#else
#define BUSTYPE(bus)    (bus)
#endif

#ifdef BCMBUSCORETYPE
#define BUSCORETYPE(ct)        (BCMBUSCORETYPE)
#else
#define BUSCORETYPE(ct)        (ct)
#endif

/* Allows size optimization for single-backplane image */
#ifdef BCMCHIPTYPE
#define CHIPTYPE(bus)    (BCMCHIPTYPE)
#else
#define CHIPTYPE(bus)    (bus)
#endif


/* Allows size optimization for SPROM support */
#if defined(BCMSPROMBUS)
#define SPROMBUS    (BCMSPROMBUS)
#elif defined(SI_PCMCIA_SROM)
#define SPROMBUS    (PCMCIA_BUS)
#else
#define SPROMBUS    (PCI_BUS)
#endif

/* Allows size optimization for single-chip image */
#ifdef BCMCHIPID
#define CHIPID(chip)    (BCMCHIPID)
#else
#define CHIPID(chip)    (chip)
#endif

#ifdef BCMCHIPREV
#define CHIPREV(rev)    (BCMCHIPREV)
#else
#define CHIPREV(rev)    (rev)
#endif

#ifdef BCMPCIEREV
#define PCIECOREREV(rev)    (BCMPCIEREV)
#else
#define PCIECOREREV(rev)    (rev)
#endif

#ifdef BCMPMUREV
#define PMUREV(rev)    (BCMPMUREV)
#else
#define PMUREV(rev)    (rev)
#endif

#ifdef BCMCCREV
#define CCREV(rev)    (BCMCCREV)
#else
#define CCREV(rev)    (rev)
#endif

#ifdef BCMGCIREV
#define GCIREV(rev)    (BCMGCIREV)
#else
#define GCIREV(rev)    (rev)
#endif

/* Defines for DMA Address Width - Shared between OSL and HNDDMA */
#define DMADDR_MASK_32 0x0        /* Address mask for 32-bits */
#define DMADDR_MASK_30 0xc0000000    /* Address mask for 30-bits */
#define DMADDR_MASK_26 0xFC000000    /* Address maks for 26-bits */
#define DMADDR_MASK_0  0xffffffff    /* Address mask for 0-bits (hi-part) */

#define    DMADDRWIDTH_26  26 /* 26-bit addressing capability */
#define    DMADDRWIDTH_30  30 /* 30-bit addressing capability */
#define    DMADDRWIDTH_32  32 /* 32-bit addressing capability */
#define    DMADDRWIDTH_63  63 /* 64-bit addressing capability */
#define    DMADDRWIDTH_64  64 /* 64-bit addressing capability */

typedef struct {
    uint32 loaddr;
    uint32 hiaddr;
} dma64addr_t;

#define PHYSADDR64HI(_pa) ((_pa).hiaddr)
#define PHYSADDR64HISET(_pa, _val) \
    do { \
        (_pa).hiaddr = (_val);        \
    } while (0)
#define PHYSADDR64LO(_pa) ((_pa).loaddr)
#define PHYSADDR64LOSET(_pa, _val) \
    do { \
        (_pa).loaddr = (_val);        \
    } while (0)

#ifdef BCMDMA64OSL
typedef dma64addr_t dmaaddr_t;
#define PHYSADDRHI(_pa) PHYSADDR64HI(_pa)
#define PHYSADDRHISET(_pa, _val) PHYSADDR64HISET(_pa, _val)
#define PHYSADDRLO(_pa)  PHYSADDR64LO(_pa)
#define PHYSADDRLOSET(_pa, _val) PHYSADDR64LOSET(_pa, _val)
#define PHYSADDRTOULONG(_pa, _ulong) \
    do { \
        _ulong = ((unsigned long long)(_pa).hiaddr << 32) | ((_pa).loaddr); \
    } while (0)

#else
typedef unsigned long dmaaddr_t;
#define PHYSADDRHI(_pa) (0)
#define PHYSADDRHISET(_pa, _val)
#define PHYSADDRLO(_pa) ((_pa))
#define PHYSADDRLOSET(_pa, _val) \
    do { \
        (_pa) = (_val);            \
    } while (0)
#endif /* BCMDMA64OSL */
#define PHYSADDRISZERO(_pa) (PHYSADDRLO(_pa) == 0 && PHYSADDRHI(_pa) == 0)

/* One physical DMA segment */
typedef struct  {
    dmaaddr_t addr;
    uint32      length;
} hnddma_seg_t;

#define MAX_DMA_SEGS 8


typedef struct {
    void *oshdmah; /* Opaque handle for OSL to store its information */
    uint origsize; /* Size of the virtual packet */
    uint nsegs;
    hnddma_seg_t segs[MAX_DMA_SEGS];
} hnddma_seg_map_t;


/* packet headroom necessary to accommodate the largest header in the system, (i.e TXOFF).
 * By doing, we avoid the need  to allocate an extra buffer for the header when bridging to WL.
 * There is a compile time check in wlc.c which ensure that this value is at least as big
 * as TXOFF. This value is used in dma_rxfill (hnddma.c).
 */

#if defined(BCM_RPC_NOCOPY) || defined(BCM_RCP_TXNOCOPY)
/* add 40 bytes to allow for extra RPC header and info  */
#define BCMEXTRAHDROOM 260
#else /* BCM_RPC_NOCOPY || BCM_RPC_TXNOCOPY */
#if defined(STB)
#if defined(BCM_GMAC3)
#define BCMEXTRAHDROOM 32 /* For FullDongle, no D11 headroom space required. */
#else
#define BCMEXTRAHDROOM 224
#endif /* ! BCM_GMAC3 */
#else
#define BCMEXTRAHDROOM 204
#endif
#endif /* BCM_RPC_NOCOPY || BCM_RPC_TXNOCOPY */

/* Packet alignment for most efficient SDIO (can change based on platform) */
#ifndef SDALIGN
#define SDALIGN    32
#endif

/* Headroom required for dongle-to-host communication.  Packets allocated
 * locally in the dongle (e.g. for CDC ioctls or RNDIS messages) should
 * leave this much room in front for low-level message headers which may
 * be needed to get across the dongle bus to the host.  (These messages
 * don't go over the network, so room for the full WL header above would
 * be a waste.).
*/
#define BCMDONGLEHDRSZ 12
#define BCMDONGLEPADSZ 16

#define BCMDONGLEOVERHEAD    (BCMDONGLEHDRSZ + BCMDONGLEPADSZ)


#if defined(NO_BCMDBG_ASSERT)
# undef BCMDBG_ASSERT
# undef BCMASSERT_LOG
#endif

#if defined(BCMASSERT_LOG)
#define BCMASSERT_SUPPORT
#endif

/* Macros for doing definition and get/set of bitfields
 * Usage example, e.g. a three-bit field (bits 4-6):
 *    #define <NAME>_M    BITFIELD_MASK(3)
 *    #define <NAME>_S    4
 * ...
 *    regval = R_REG(osh, &regs->regfoo);
 *    field = GFIELD(regval, <NAME>);
 *    regval = SFIELD(regval, <NAME>, 1);
 *    W_REG(osh, &regs->regfoo, regval);
 */
#define BITFIELD_MASK(width) \
        (((unsigned)1 << (width)) - 1)
#define GFIELD(val, field) \
        (((val) >> field ## _S) & field ## _M)
#define SFIELD(val, field, bits) \
        (((val) & (~(field ## _M << field ## _S))) | \
         ((unsigned)(bits) << field ## _S))

/* define BCMSMALL to remove misc features for memory-constrained environments */
#ifdef BCMSMALL
#undef    BCMSPACE
#define bcmspace    FALSE    /* if (bcmspace) code is discarded */
#else
#define    BCMSPACE
#define bcmspace    TRUE    /* if (bcmspace) code is retained */
#endif

/* Max. nvram variable table size */
#ifndef MAXSZ_NVRAM_VARS
#ifdef LARGE_NVRAM_MAXSZ
#define MAXSZ_NVRAM_VARS    LARGE_NVRAM_MAXSZ
#else
/* SROM12 changes */
#define    MAXSZ_NVRAM_VARS    6144
#endif /* LARGE_NVRAM_MAXSZ */
#endif /* !MAXSZ_NVRAM_VARS */



/* WL_ENAB_RUNTIME_CHECK may be set based upon the #define below (for ROM builds). It may also
 * be defined via makefiles (e.g. ROM auto abandon unoptimized compiles).
 */


#ifdef BCMLFRAG /* BCMLFRAG support enab macros  */
    extern bool _bcmlfrag;
    #if defined(WL_ENAB_RUNTIME_CHECK) || !defined(DONGLEBUILD)
        #define BCMLFRAG_ENAB() (_bcmlfrag)
    #elif defined(BCMLFRAG_DISABLED)
        #define BCMLFRAG_ENAB()    (0)
    #else
        #define BCMLFRAG_ENAB()    (1)
    #endif
#else
    #define BCMLFRAG_ENAB()        (0)
#endif /* BCMLFRAG_ENAB */

#ifdef BCMPCIEDEV /* BCMPCIEDEV support enab macros */
extern bool _pciedevenab;
    #if defined(WL_ENAB_RUNTIME_CHECK)
        #define BCMPCIEDEV_ENAB() (_pciedevenab)
    #elif defined(BCMPCIEDEV_ENABLED)
        #define BCMPCIEDEV_ENAB()    1
    #else
        #define BCMPCIEDEV_ENAB()    0
    #endif
#else
    #define BCMPCIEDEV_ENAB()    0
#endif /* BCMPCIEDEV */

    #define BCMSDIODEV_ENAB()    0

/* Max size for reclaimable NVRAM array */
#ifdef DL_NVRAM
#define NVRAM_ARRAY_MAXSIZE    DL_NVRAM
#else
#define NVRAM_ARRAY_MAXSIZE    MAXSZ_NVRAM_VARS
#endif /* DL_NVRAM */

extern uint32 gFWID;

/* Chip related low power flags (lpflags) */
#define LPFLAGS_SI_GLOBAL_DISABLE        (1 << 0)
#define LPFLAGS_SI_MEM_STDBY_DISABLE        (1 << 1)
#define LPFLAGS_SI_SFLASH_DISABLE        (1 << 2)
#define LPFLAGS_SI_BTLDO3P3_DISABLE        (1 << 3)
#define LPFLAGS_SI_GCI_FORCE_REGCLK_DISABLE    (1 << 4)
#define LPFLAGS_SI_FORCE_PWM_WHEN_RADIO_ON (1 << 5)
#define LPFLAGS_PHY_GLOBAL_DISABLE        (1 << 16)
#define LPFLAGS_PHY_LP_DISABLE            (1 << 17)
#define LPFLAGS_PSM_PHY_CTL            (1 << 18)

/* Chip related Cbuck modes */
#define PMU_43012_VREG8_DYNAMIC_CBUCK_MODE0 0x00001c03
#define PMU_43012_VREG9_DYNAMIC_CBUCK_MODE0 0x00492490
#define PMU_43012_VREG8_DYNAMIC_CBUCK_MODE1 0x00001c03
#define PMU_43012_VREG9_DYNAMIC_CBUCK_MODE1 0x00490410

/* Chip related dynamic cbuck mode mask */

#define PMU_43012_VREG8_DYNAMIC_CBUCK_MODE_MASK  0xFFFFFC00
#define PMU_43012_VREG9_DYNAMIC_CBUCK_MODE_MASK  0xFFFFFFFF

#ifndef PAD
#define _PADLINE(line)  pad ## line
#define _XSTR(line)     _PADLINE(line)
#define PAD             _XSTR(__LINE__)
#endif

#endif /* _bcmdefs_h_ */
