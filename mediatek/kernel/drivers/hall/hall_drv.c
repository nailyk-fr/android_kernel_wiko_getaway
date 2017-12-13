#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/vmalloc.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/wait.h>
#include <linux/spinlock.h>
#include <linux/ctype.h>

#include <linux/semaphore.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/workqueue.h>
#include <linux/switch.h>
#include <linux/delay.h>

#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/kthread.h>
#include <linux/input.h>
#include <linux/wakelock.h>
#include <linux/time.h>

#include <linux/string.h>

#include <linux/hwmsensor.h>
#include <linux/hwmsen_dev.h>
#include <linux/sensors_io.h>

#include <mach/mt_typedefs.h>
#include <mach/mt_reg_base.h>
#include <mach/irqs.h>
#include <mach/reg_accdet.h>

#include <cust_eint.h>
#include <cust_gpio_usage.h>
#include <mach/mt_gpio.h>
#include <mach/eint.h>

#include <mach/mt_boot.h>
#include "hall_drv.h"

/*----------------------------------------------------------------------
IOCTL
----------------------------------------------------------------------*/
#define HALL_DEVNAME "hall"
#define TINNO_PS_STATUS_POLL

/*----------------------------------------------------------------------
static variable defination
----------------------------------------------------------------------*/
#ifdef TINNO_PS_STATUS_POLL
struct work_struct	eint_work;
#endif
static int debug_enable_drv = 1;
#define HALL_DEBUG(format, args...) do{ \
	if(debug_enable_drv) \
	{\
		printk(KERN_ERR format,##args);\
	}\
}while(0)
#define EINT_PIN_HALL_OFF        (0)
#define EINT_PIN_HALL_ON       (1)
int cur_hall_state = EINT_PIN_HALL_OFF;


static struct platform_driver hall_driver;
static struct switch_dev hall_data;	
static dev_t hall_devno;
static struct cdev *hall_cdev;
static struct class *hall_class = NULL;
static struct device *hall_nor_device = NULL;

static int g_hall_first = 1;
static void hall_eint_set_hw_debounce(void)
{
	mt_eint_set_hw_debounce(CUST_EINT_HALL_1_NUM, CUST_EINT_HALL_1_DEBOUNCE_CN);
}

static void hall_eint_set(void)
{
	mt_set_gpio_mode(GPIO_HALL_1_PIN, GPIO_HALL_1_PIN_M_EINT);
    mt_set_gpio_dir(GPIO_HALL_1_PIN, GPIO_DIR_IN);
    mt_set_gpio_pull_enable(GPIO_HALL_1_PIN, GPIO_PULL_DISABLE); //To disable GPIO PULL.
    //hall_eint_set_sens();
	hall_eint_set_hw_debounce();
}

static void hall_eint_set_polarity_pre(void)
{

	mt_eint_set_polarity(CUST_EINT_HALL_1_NUM, 1);

}

static void hall_eint_set_polarity_next(void)
{

	mt_eint_set_polarity(CUST_EINT_HALL_1_NUM, 0);

}

void hall_eint_polarity_reverse_on(void)
{	
	if (CUST_EINT_HALL_1_TYPE == CUST_EINTF_TRIGGER_HIGH){
		hall_eint_set_polarity_pre();
	}else{
		hall_eint_set_polarity_next();
	}
	hall_eint_set_hw_debounce();
}

void hall_eint_polarity_reverse_off(void)
{
	if (CUST_EINT_HALL_1_TYPE == CUST_EINTF_TRIGGER_HIGH){
		hall_eint_set_polarity_next();
	}else{
		hall_eint_set_polarity_pre();
	}
	/* update the eint status */
    hall_eint_set_hw_debounce();
}

void hall_eint_unmask(void)
{
	mt_eint_unmask(CUST_EINT_HALL_1_NUM);  
}

void hall_eint_mask(void)
{
	mt_eint_mask(CUST_EINT_HALL_1_NUM); 
}


void hall_eint_func(void)
{
	int ret=0;
    HALL_DEBUG("[Hall] hall_eint_func:cur_hall_state=%d \n",cur_hall_state);
    hall_eint_mask();
	if(cur_hall_state ==  EINT_PIN_HALL_OFF ) 
	{
		hall_eint_polarity_reverse_on();
		/* update the eint status */
		cur_hall_state = EINT_PIN_HALL_ON;
        switch_set_state((struct switch_dev *)&hall_data, cur_hall_state);
	} 
	else 
	{
	#ifdef TINNO_PS_STATUS_POLL
	extern int get_ap3220_ps_status(void);
		 
	if(get_ap3220_ps_status())
		schedule_work(&eint_work);
		/*{
		 		sensor_data.values[0] = 1;
				sensor_data.value_divide = 1;
				sensor_data.status = SENSOR_STATUS_ACCURACY_MEDIUM;
				hwmsen_get_interrupt_data(ID_PROXIMITY, &sensor_data);
		}*/
	#endif
		hall_eint_polarity_reverse_off();
		cur_hall_state = EINT_PIN_HALL_OFF;
        switch_set_state((struct switch_dev *)&hall_data, cur_hall_state);
	}
	
    hall_eint_unmask();
}

static void hall_eint_registration(void)
{
	mt_eint_registration(CUST_EINT_HALL_1_NUM, CUST_EINT_HALL_1_TYPE, hall_eint_func, 0);
	HALL_DEBUG("[Hall]hall set EINT finished, hall_eint_num=%d, hall_eint_debounce_en=%d, hall_eint_polarity=%d\n", CUST_EINT_HALL_1_NUM, CUST_EINT_HALL_1_DEBOUNCE_EN, CUST_EINT_HALL_1_TYPE);
}

static inline int hall_setup_eint(void)
{
	
	/*configure to GPIO function, external interrupt*/
    HALL_DEBUG("[Hall]hall_setup_eint\n");
	hall_eint_set();
	
	hall_eint_registration();
	
	hall_eint_unmask();  

	
    return 0;
	
}
static int hall_open(struct inode *inode, struct file *file)
{ 
   	return 0;
}

static int hall_release(struct inode *inode, struct file *file)
{
    return 0;
}

static long hall_unlocked_ioctl(struct file *file, unsigned int cmd,unsigned long arg)
{
	return 0;
}

static struct file_operations hall_fops = {
	.owner		= THIS_MODULE,
	.unlocked_ioctl		= hall_unlocked_ioctl,
	.open		= hall_open,
	.release	= hall_release,	
};
#ifdef TINNO_PS_STATUS_POLL
static void hall_eint_work(struct work_struct *work)
{
	hwm_sensor_data sensor_data;
	sensor_data.values[0] = 1;
	sensor_data.value_divide = 1;
	sensor_data.status = SENSOR_STATUS_ACCURACY_MEDIUM;
	hwmsen_get_interrupt_data(ID_PROXIMITY, &sensor_data);
}
#endif
static int hall_probe(struct platform_device *dev)	
{
	int ret = 0;

	HALL_DEBUG("[Hall]hall_probe begin!\n");

	
	//------------------------------------------------------------------
	// 							below register hall as switch class
	//------------------------------------------------------------------	
	hall_data.name = "hall";
	hall_data.index = 0;
	hall_data.state = EINT_PIN_HALL_OFF;

	
	ret = switch_dev_register(&hall_data);
	if(ret)
	{
		HALL_DEBUG("[Hall]switch_dev_register returned:%d!\n", ret);
		return 1;
	}
		
	//------------------------------------------------------------------
	// 							Create normal device 
	//------------------------------------------------------------------
	ret = alloc_chrdev_region(&hall_devno, 0, 1, HALL_DEVNAME);
	if (ret)
	{
		HALL_DEBUG("[Hall]alloc_chrdev_region: Get Major number error!\n");			
	} 
		
	hall_cdev = cdev_alloc();
    hall_cdev->owner = THIS_MODULE;
    hall_cdev->ops = &hall_fops;
    ret = cdev_add(hall_cdev, hall_devno, 1);
	if(ret)
	{
		HALL_DEBUG("[Hall]hall error: cdev_add\n");
	}
#ifdef TINNO_PS_STATUS_POLL
	INIT_WORK(&eint_work, hall_eint_work);
#endif
	hall_class = class_create(THIS_MODULE, HALL_DEVNAME);

    // if we want auto creat device node, we must call this
	hall_nor_device = device_create(hall_class, NULL, hall_devno, NULL, HALL_DEVNAME);  

	HALL_DEBUG("[Hall]hall_probe : HALL_INIT\n");  
	if (1 == g_hall_first) 
	{		 
	    hall_setup_eint();
		g_hall_first = 0;
	}
	
    HALL_DEBUG("[Hall]hall_probe done!\n");
		
	return 0;
}

static int hall_remove(struct platform_device *dev)	
{
	HALL_DEBUG("[Accdet]accdet_remove begin!\n");
	
	switch_dev_unregister(&hall_data);
	device_del(hall_nor_device);
	class_destroy(hall_class);
	cdev_del(hall_cdev);
	unregister_chrdev_region(hall_devno,1);	
	HALL_DEBUG("[Accdet]accdet_remove Done!\n");
    
	return 0;
}

static int hall_suspend(struct device *device)  // only one suspend mode
{
	return 0;
}

static int hall_resume(struct device *device) // wake up
{
	return 0;
}

static struct platform_driver hall_driver = {
	.probe		= hall_probe,	
	.remove   	= hall_remove,
	.suspend 	= hall_suspend,
    .resume 	= hall_resume,
	.driver     = {
	.name       = "Hall_Driver",
	//.pm         = &hall_pm_ops,
	},
};

static int hall_mod_init(void)
{
	int ret = 0;

	HALL_DEBUG("[Hall]accdet_mod_init begin!\n");

	//------------------------------------------------------------------
	// 							Hall PM
	//------------------------------------------------------------------
	
	ret = platform_driver_register(&hall_driver);
	if (ret) {
		HALL_DEBUG("[Hall]platform_driver_register error:(%d)\n", ret);
		return ret;
	}
	else
	{
		HALL_DEBUG("[Hall]platform_driver_register done!\n");
	}

    HALL_DEBUG("[Hall]hall_mod_init done!\n");
    return 0;

}

static void  hall_mod_exit(void)
{
	HALL_DEBUG("[Hall]hall_mod_exit\n");
	platform_driver_unregister(&hall_driver);

	HALL_DEBUG("[Hall]hall_mod_exit Done!\n");
}

module_init(hall_mod_init);
module_exit(hall_mod_exit);


module_param(debug_enable_drv,int,0644);

MODULE_DESCRIPTION("Tinno S8321A hall driver");
MODULE_AUTHOR("xuhong.yi <xuhong.yi@tinno.com>");
MODULE_LICENSE("GPL");

