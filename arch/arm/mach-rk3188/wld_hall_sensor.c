/*
 *  wld_hall_sensor.c
 *
 * Copyright (C) 2013 WLD, Inc.
 * Author: qdk0901 <qdk0901@wld.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
*/

#include <linux/module.h>
#include <linux/sysdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/irq.h>
#include <linux/delay.h>
#include <linux/types.h>
#include <linux/input.h>
#include <linux/platform_device.h>
#include <linux/mutex.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/hrtimer.h>
#include <linux/switch.h>
#include <linux/input.h>
#include <linux/debugfs.h>
#include <linux/wakelock.h>
#include <asm/gpio.h>
#include <asm/atomic.h>
#include <asm/mach-types.h>
#include <linux/earlysuspend.h>
#include <linux/gpio.h>
#include <mach/board.h>
#include <linux/slab.h>
#include "wld_hall_sensor.h"

#if 1
#define DBG(x...) printk(x)
#else
#define DBG(x...) do { } while (0)
#endif

#define DETECT_PEROID 500

enum
{
	HALL_SENSOR_UNKOWN,
	HALL_SENSOR_ACTIVE,
	HALL_SENSOR_DEACTIVE,	
};

struct hall_sensor_priv {
	struct input_dev *input_dev;
	struct hall_sensor_pdata *pdata;
	struct work_struct work;
	struct timer_list timer;
	struct switch_dev sdev;
	struct mutex mutex_lock;
	int last_status;
	int is_suspend;
};

static struct hall_sensor_priv *g_hall_sensor;

static void hall_sensor_timer_func(void)
{
	schedule_work(&g_hall_sensor->work);
}

static void hall_sensor_work(struct work_struct *work)
{
	mutex_lock(&g_hall_sensor->mutex_lock);
	struct hall_sensor_pdata *pdata = g_hall_sensor->pdata;
	int level = gpio_get_value(pdata->hall_sensor_gpio);
	if (level == pdata->hall_sensor_active_type) {
		// hall sensor active
		
		if (g_hall_sensor->last_status == HALL_SENSOR_DEACTIVE) {
				DBG("hall sensor status change from deactive to active\n");
				pdata->action_for_hall_sensor(1);
		}
		
		g_hall_sensor->last_status = HALL_SENSOR_ACTIVE;
		
	} else {
		// hall sensor not active
		if (g_hall_sensor->last_status == HALL_SENSOR_ACTIVE) {
				DBG("hall sensor status change from active to deactive\n");
				if (!g_hall_sensor->is_suspend)
					pdata->action_for_hall_sensor(0);
		}
		
		g_hall_sensor->last_status = HALL_SENSOR_DEACTIVE;
	}
	
	mod_timer(&g_hall_sensor->timer, jiffies + msecs_to_jiffies(DETECT_PEROID));
	mutex_unlock(&g_hall_sensor->mutex_lock);	
}

static ssize_t hall_print_name(struct switch_dev *sdev, char *buf)
{
	return sprintf(buf, "Hall\n");
}

static int hall_sensor_open(struct input_dev *dev)
{
	return 0;
}

static void hall_sensor_close(struct input_dev *dev)
{
	
}

#ifdef CONFIG_HAS_EARLYSUSPEND
static void hall_sensor_early_suspend(struct early_suspend *h)
{
	g_hall_sensor->is_suspend = 1;
}

static void hall_sensor_early_resume(struct early_suspend *h)
{
	g_hall_sensor->is_suspend = 0;
}

static struct early_suspend hs_early_suspend;
#endif

static int hall_sensor_probe(struct platform_device *pdev)
{
	int ret;
	struct hall_sensor_priv *hall_sensor;
	struct hall_sensor_pdata *pdata;
	pdata = pdev->dev.platform_data;
	
	if (!pdata->hall_sensor_gpio)
		return -ENODEV;
		
	hall_sensor = kzalloc(sizeof(struct hall_sensor_priv), GFP_KERNEL);
	if (hall_sensor == NULL) {
		dev_err(&pdev->dev, "failed to allocate driver data\n");
		return -ENOMEM;
	}	
	hall_sensor->pdata = pdata;
	hall_sensor->sdev.name = "hall";
	hall_sensor->sdev.print_name = hall_print_name;
	ret = switch_dev_register(&hall_sensor->sdev);
	if (ret < 0)
		goto failed_free;
	
	mutex_init(&hall_sensor->mutex_lock);
	
	hall_sensor->input_dev = input_allocate_device();
	if (!hall_sensor->input_dev) {
		dev_err(&pdev->dev, "failed to allocate input device\n");
		ret = -ENOMEM;
		goto failed_free;
	}	
	hall_sensor->input_dev->name = pdev->name;
	hall_sensor->input_dev->open = hall_sensor_open;
	hall_sensor->input_dev->close = hall_sensor_close;
	hall_sensor->input_dev->dev.parent = &pdev->dev;
	hall_sensor->input_dev->id.vendor = 0x0001;
	hall_sensor->input_dev->id.product = 0x0001;
	hall_sensor->input_dev->id.version = 0x0200;
	// Register the input device 
	ret = input_register_device(hall_sensor->input_dev);
	if (ret) {
		dev_err(&pdev->dev, "failed to register input device\n");
		goto failed_free_dev;
	}
	input_set_capability(hall_sensor->input_dev, EV_KEY, pdata->hall_sensor_code);

#ifdef CONFIG_HAS_EARLYSUSPEND
	hs_early_suspend.suspend = hall_sensor_early_suspend;
	hs_early_suspend.resume = hall_sensor_early_resume;
	hs_early_suspend.level = ~0x0;
	register_early_suspend(&hs_early_suspend);
#endif
	//------------------------------------------------------------------
	
	hall_sensor->last_status = HALL_SENSOR_UNKOWN;
	
	INIT_WORK(&hall_sensor->work, hall_sensor_work);
	setup_timer(&hall_sensor->timer, hall_sensor_timer_func, 0);
	
	g_hall_sensor = hall_sensor;
	
	//perform the first detection
	hall_sensor_work(&hall_sensor->work);
	return 0;	
	
failed_free_dev:
	platform_set_drvdata(pdev, NULL);
	input_free_device(hall_sensor->input_dev);
failed_free:
	dev_err(&pdev->dev, "failed to hall sensor probe\n");
	kfree(hall_sensor);
	return ret;
}

static int hall_sensor_suspend(struct platform_device *pdev, pm_message_t state)
{
	g_hall_sensor->is_suspend = 1;
	return 0;
}

static int hall_sensor_resume(struct platform_device *pdev)
{
	g_hall_sensor->is_suspend = 0;
	return 0;
}

static struct platform_driver hall_sensor_driver = {
	.probe	= hall_sensor_probe,
//	.resume = 	hall_sensor_resume,	
//	.suspend = 	hall_sensor_suspend,	
	.driver	= {
		.name	= "hall_sensor",
		.owner	= THIS_MODULE,
	},
};

static int __init hall_sensor_init(void)
{
	platform_driver_register(&hall_sensor_driver);
	return 0;
}
late_initcall(hall_sensor_init);

MODULE_AUTHOR("qdk0901 <qdk0901@wld.com>");
MODULE_DESCRIPTION("Hall Sensor driver");
MODULE_LICENSE("GPL");
