/*
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */


#include <printf.h>
#include <platform/mt_typedefs.h>
#include <platform/mtk_key.h>
#include <platform/boot_mode.h>
#include <platform/mt_pmic.h>
#include <platform/mt_gpio.h>
#include <platform/mt_pmic_wrap_init.h>
#include <platform/sync_write.h>
#include <target/cust_key.h>

extern int pmic_detect_powerkey(void);
extern int pmic_detect_homekey(void);

void set_kpd_pmic_mode(void)
{
		unsigned int a,c;
		a = pwrap_read(0x0040,&c);
		if(a != 0)
		printf("kpd write fail, addr: 0x0502\n");

		printf("kpd read addr: 0x0502: data:0x%x\n", c);
		c = c&0xFFFE;//0x4000;
		a = pwrap_write(0x0040,c);
		if(a != 0)
		printf("kpd write fail, addr: 0x0502\n");

#ifdef MT65XX_PMIC_RST_KEY
	pmic_config_interface(STRUP_CON3, 0x01, PMIC_RG_FCHR_PU_EN_MASK, PMIC_RG_FCHR_PU_EN_SHIFT);//pull up homekey pin of PMIC for 82 project
	//mdelay(100);
#endif

	return;
}

void disable_PMIC_kpd_clock(void)
{
#if 0
	int rel = 0;
	rel = pmic_config_interface(WRP_CKPDN,0x1, PMIC_RG_WRP_32K_PDN_MASK, PMIC_RG_WRP_32K_PDN_SHIFT);
	if(rel !=  0){
		printf("kpd disable_PMIC_kpd_clock register fail!\n");
	}
#endif
}

void enable_PMIC_kpd_clock(void)
{
#if 0
	int rel = 0;
	rel = pmic_config_interface(WRP_CKPDN,0x0, PMIC_RG_WRP_32K_PDN_MASK, PMIC_RG_WRP_32K_PDN_SHIFT);
	if(rel !=  0){
		printf("kpd enable_PMIC_kpd_clock register fail!\n");
	}
#endif
}

BOOL mtk_detect_key(unsigned short key)	/* key: HW keycode */
{
/* Because 6572 FPGA doesn't include keypad HW,
 * add FPGA macro to avoid misjudgment 
 */
#ifdef MACH_FPGA
	return FALSE;
#else

printf("mtk detect key function key = %d\n", key);

	unsigned short idx, bit, din;

	if (key >= KPD_NUM_KEYS)
		return FALSE;

	if (key % 9 == 8)
		key = 8;

	if (key == 8)
    	{ /* Power key */
		if (1 == pmic_detect_powerkey())
		{
			//dbg_print ("power key is pressed\n");
			return TRUE;
		}
		return FALSE;
	}

#ifdef MT65XX_PMIC_RST_KEY
	if(key == MT65XX_PMIC_RST_KEY)
	{
	printf("mtk detect key function pmic_detect_homekey MT65XX_PMIC_RST_KEY = %d\n",MT65XX_PMIC_RST_KEY);
		if (1 == pmic_detect_homekey())
		{
			printf("mtk detect key function pmic_detect_homekey pressed\n");
			return TRUE;
		}
		return FALSE;
	}
#endif

	idx = key / 16;
	bit = key % 16;

	din = DRV_Reg16(KP_MEM1 + (idx << 2)) & (1U << bit);
	if (!din) {
		printf("key %d is pressed\n", key);
		return TRUE;
	}
	return FALSE;
#endif
}

BOOL mtk_detect_pmic_just_rst(void)
{
#if 1
	kal_uint32 just_rst=0;

	printf("detecting pmic just reset\n");

		pmic_read_interface(0x04A, &just_rst, 0x01, 14);
		if(just_rst)
		{
			printf("Just recover form a reset\n");
			pmic_config_interface(0x04A, 0x01, 0x01, 4);
			return TRUE;
		}
#endif
	return FALSE;
}
BOOL mt65XX_detect_special_key(void)
{
	unsigned short idx1, idx2, bit1, bit2, din, din1, din2;

    #ifdef MT6582_FCHR_ENB
        if (1 == pmic_detect_homekey())
	    din1 = 0;
        else
	    din1 = 1;

        printf("mt6577_detect_key FCHR_ENB din1=%x\n",din1);
    #else
//Key1
	idx1 = MT65XX_FACTORY_KEY / 16;
	bit1 = MT65XX_FACTORY_KEY % 16;
	din1 = DRV_Reg16(KP_MEM1 + (idx1 << 2)) & (1U << bit1);
	printf("mt6577_detect_key din1=%x\n",din1);
    #endif
#ifdef MT65XX_FACTORY_KEY2
	if (din1)
	{
	    idx1 = MT65XX_FACTORY_KEY2 / 16;
	    bit1 = MT65XX_FACTORY_KEY2 % 16;
	    din1 = DRV_Reg16(KP_MEM1 + (idx1 << 2)) & (1U << bit1);	    
	}
#endif
/*
	din1 = mt6329_detect_homekey();

	if (din1 == 1)
	    din1 = 0;
	else
	    din1 = 1;
*/
	idx2 = MT65XX_RECOVERY_KEY / 16;
	bit2 = MT65XX_RECOVERY_KEY % 16;
	din2 = DRV_Reg16(KP_MEM1 + (idx2 << 2)) & (1U << bit2);
#ifdef MT65XX_RECOVERY_KEY2
	if (din2)
	{
	    idx2 = MT65XX_RECOVERY_KEY2 / 16;
	    bit2 = MT65XX_RECOVERY_KEY2 % 16;
	    din2 = DRV_Reg16(KP_MEM1 + (idx2 << 2)) & (1U << bit2);	    
	}
#endif
	
	
	printf("mt6577_detect_key din2=%x\n",din2);

	din = din1 + din2;
	printf("mt6577_detect_key din=%x\n",din);

	if (!din) {
		//printf("key %d is pressed\n", key);
		return TRUE;
	}
	return FALSE;
}

unsigned short mt65XX_get_key(void)
{
	unsigned short idx, bit, din;

	din = DRV_Reg16(KP_MEM1);

	printf("mt6577_detect_key din=%x\n",din);

	return din;
}

