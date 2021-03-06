/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 * Realtek Corporation, No. 2, Innovation Road II, Hsinchu Science Park,
 * Hsinchu 300, Taiwan.
 *
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 *****************************************************************************/

/*Image2HeaderVersion: R3 1.0*/
#if (RTL8198F_SUPPORT == 1)
#ifndef __INC_MP_RF_HW_IMG_8198F_H
#define __INC_MP_RF_HW_IMG_8198F_H


/******************************************************************************
*                           radioa.TXT
******************************************************************************/

void
odm_read_and_config_mp_8198f_radioa(/* tc: Test Chip, mp: mp Chip*/
	struct	dm_struct *dm
);
u32	odm_get_version_mp_8198f_radioa(void);

/******************************************************************************
*                           radiob.TXT
******************************************************************************/

void
odm_read_and_config_mp_8198f_radiob(/* tc: Test Chip, mp: mp Chip*/
	struct	dm_struct *dm
);
u32	odm_get_version_mp_8198f_radiob(void);

/******************************************************************************
*                           radioc.TXT
******************************************************************************/

void
odm_read_and_config_mp_8198f_radioc(/* tc: Test Chip, mp: mp Chip*/
	struct	dm_struct *dm
);
u32	odm_get_version_mp_8198f_radioc(void);

/******************************************************************************
*                           radiod.TXT
******************************************************************************/

void
odm_read_and_config_mp_8198f_radiod(/* tc: Test Chip, mp: mp Chip*/
	struct	dm_struct *dm
);
u32	odm_get_version_mp_8198f_radiod(void);

/******************************************************************************
*                           txpowertrack.TXT
******************************************************************************/

void
odm_read_and_config_mp_8198f_txpowertrack(/* tc: Test Chip, mp: mp Chip*/
	struct	dm_struct *dm
);
u32	odm_get_version_mp_8198f_txpowertrack(void);

/******************************************************************************
*                           txpwr_lmt.TXT
******************************************************************************/

void
odm_read_and_config_mp_8198f_txpwr_lmt(/* tc: Test Chip, mp: mp Chip*/
	struct	dm_struct *dm
);
u32	odm_get_version_mp_8198f_txpwr_lmt(void);

#endif
#endif /* end of HWIMG_SUPPORT*/

