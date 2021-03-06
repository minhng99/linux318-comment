/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * 
 * 
 * $Revision: 39101 $
 * $Date: 2013-05-03 17:35:27 +0800 (Fri, 03 May 2013) $
 *
 * Purpose : SEC Driver test case
 *
 * Feature : test SEC API
 *
 */

#ifndef __DAL_SEC_TEST_CASE_H__
#define __DAL_SEC_TEST_CASE_H__

int32 dal_sec_portAttackPreventState_test(uint32 testcase);
int32 dal_sec_attackPrevent_test(uint32 testcase);
int32 dal_sec_attackFloodThresh_test(uint32 testcase);

#endif /* __DAL_SEC_TEST_CASE_H__ */

