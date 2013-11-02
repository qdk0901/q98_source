/*
 * es8323.c  --  ES8323 ALSA SoC audio codec driver
 *
 * Copyright 2011 Realtek Semiconductor Corp.
 * Author: Johnny Hsu <johnnyhsu@realtek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/initval.h>
#include <sound/tlv.h>

#include "es8323_pcm.h"

#define ES8323_PROC
#ifdef ES8323_PROC
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/vmalloc.h>
#endif

#define MODEM_ON 1
#define MODEM_OFF 0

static struct i2c_client *i2c_client;
static int status;

static int codec_write(struct i2c_client *client, unsigned int reg,
			      unsigned int value)
{
	u8 data[2];

	data[0] = reg;
	data[1] = value & 0xff;

	//printk("%s: reg=0x%x value=0x%x\n",__func__,reg,value);
	if (i2c_master_send(client, data, 2) == 2)
		return 0;
	else
		return -EIO;
}

static unsigned int codec_read(struct i2c_client *client,
					  unsigned int r)
{
	struct i2c_msg xfer[2];
	u8 reg = r;
	u16 data;
	int ret;

	/* Write register */
	xfer[0].addr = client->addr;
	xfer[0].flags = 0;
	xfer[0].len = 1;
	xfer[0].buf = &reg;
	xfer[0].scl_rate = 100 * 1000;

	/* Read data */
	xfer[1].addr = client->addr;
	xfer[1].flags = I2C_M_RD;
	xfer[1].len = 1;
	xfer[1].buf = (u8 *)&data;
	xfer[1].scl_rate = 100 * 1000;

	ret = i2c_transfer(client->adapter, xfer, 2);
	if (ret != 2) {
		dev_err(&client->dev, "i2c_transfer() returned %d\n", ret);
		return 0;
	}
	//printk("%s: reg=0x%x value=0x%x\n",__func__,reg,(data >> 8) | ((data & 0xff) << 8));

	return data & 0xff;
}

struct es8323_reg {
	u8 reg_index;
	u8 reg_value;
};

static struct es8323_reg init_data[] = {
	{0x02, 0xF3},
	{0x00, 0x05}, //Set chip to play&record mode
	{0x01, 0x40}, //Power up analog and lbias
	{0x03, 0x00}, //Power up ADC /Analog input
	{0x04, 0x0C}, //Enable LOUT/ROUT
	{0x0a, 0xf0},  //ADC INPUT=LIN2/RIN2
	{0x0b, 0x82},  //ADC INPUT=LIN2/RIN2 //82
	{0x09, 0x72}, //Select analog input PGA gain for ADC, 24bB
	{0x0c, 0x1F}, //PCM- 16bit
	{0x0d, 0x02}, //MCLK/RCLK ratio for ADC, 11.2M 256FS
	{0x10, 0x10}, //ADC digital volume 0db
	{0x11, 0x10}, //ADC digital volume 0db
	
	//ALC control 
	/*
	{0x12, 0xea}, // ALC stereo MAXGAIN: 35.5dB,  MINGAIN: +6dB (Record Volume increased!)
	{0x13, 0xc0},
	{0x14, 0x05},
	{0x15, 0x06},
	{0x16, 0x53},
	*/
	
	{0x17, 0x1F}, // SFI for DAC ,  PCM -16bit
	{0x18, 0x02}, // MCLK/RCLK ratio for DAC, 11.2M 256FS
	{0x1A, 0x30}, // DAC digital volume
	{0x1B, 0x30}, // DAC digital volume
	{0x1D, 0x46}, //right channel output zero
	
	//DAC mixer
	{0x26, 0x00}, // select RIN1/LIN1 for DAC
	{0x27, 0x90}, // LDAC to left mixer
	{0x28, 0x38},
	{0x29, 0x38},
	{0x2A, 0x90}, // RDAC to right mixer
	
	// LOUT/ROUT volume
	{0x2E, 0x1E}, //0dB
	{0x2F, 0x1E} , //0dB
	{0x30, 0x1E}, //0dB
	{0x31, 0x1E}, //0dB
	
	//Power up DEM and STM
	{0x02, 0x00},
	{0x08, 0x00}, //ES8388 slave
	{0x2B, 0x80}, 


{0x1E,0x1F},
{0x1F,0x00},
{0x20,0xED},
{0x21,0xAF},
{0x22,0x20},
{0x23,0x6C},
{0x24,0xE9},
{0x25,0xBE},

	
};
#define ES8323_INIT_REG_NUM ARRAY_SIZE(init_data)

static int es8323_reg_init(struct i2c_client *client)
{
	int i;

	for (i = 0; i < ES8323_INIT_REG_NUM; i++)
		codec_write(client, init_data[i].reg_index,
				init_data[i].reg_value);

	for (i = 0; i < ES8323_INIT_REG_NUM; i++) {
		//printk("codec_read[0x%02x] = %02x\n", init_data[i].reg_index, codec_read(client, init_data[i].reg_index));	
	}
	return 0;
}

static int es8323_on_init(struct i2c_client *client)
{
codec_write(client, 0x02,0xf3);
codec_write(client, 0x2B,0x80);
codec_write(client, 0x08,0x00);   //ES8388 salve  
codec_write(client, 0x00,0x32);   //
codec_write(client, 0x01,0x72);   //PLAYBACK & RECORD Mode,EnRefr=1
codec_write(client, 0x03,0x59);   //pdn_ana=0,ibiasgen_pdn=0
codec_write(client, 0x05,0x00);   //pdn_ana=0,ibiasgen_pdn=0
codec_write(client, 0x06,0xc3);   //pdn_ana=0,ibiasgen_pdn=0 
codec_write(client, 0x09,0x88);  //ADC L/R PGA =  +24dB
//----------------------------------------------------------------------------------------------------------------
codec_write(client, 0x0a,0xf0);  //ADC INPUT=LIN2/RIN2
codec_write(client, 0x0b,0x82);  //ADC INPUT=LIN2/RIN2 //82
//-----------------------------------------------------------------------------------------------------------------
codec_write(client, 0x0C,0x1F);  //I2S-24BIT
codec_write(client, 0x0d,0x02);  //MCLK/LRCK=256 
codec_write(client, 0x10,0x00);  //ADC Left Volume=0db
codec_write(client, 0x11,0x00);  //ADC Right Volume=0db
codec_write(client, 0x12,0xea); // ALC stereo MAXGAIN: 35.5dB,  MINGAIN: +6dB (Record Volume increased!)
codec_write(client, 0x13,0xc0);
codec_write(client, 0x14,0x05);
codec_write(client, 0x15,0x06);
codec_write(client, 0x16,0x53);  
codec_write(client, 0x17,0x1F);  //I2S-16BIT
codec_write(client, 0x18,0x02);
codec_write(client, 0x1A,0x00);  //DAC VOLUME=0DB
codec_write(client, 0x1B,0x00);
                /*
                codec_write(client, 0x1E,0x01);    //for 47uF capacitors ,15db Bass@90Hz,Fs=44100
                codec_write(client, 0x1F,0x84);
                codec_write(client, 0x20,0xED);
                codec_write(client, 0x21,0xAF);
                codec_write(client, 0x22,0x20);
                codec_write(client, 0x23,0x6C);
                codec_write(client, 0x24,0xE9);
                codec_write(client, 0x25,0xBE);
                */
codec_write(client, 0x26,0x12);  //Left DAC TO Left IXER
codec_write(client, 0x27,0x90);  //Left DAC TO Left MIXER
codec_write(client, 0x28,0x38);
codec_write(client, 0x29,0x38);
codec_write(client, 0x2A,0xb8);
codec_write(client, 0x02,0x00); //aa //START DLL and state-machine,START DSM 
codec_write(client, 0x08,0x00);   //ES8388 salve
codec_write(client, 0x19,0x02);  //SOFT RAMP RATE=32LRCKS/STEP,Enable ZERO-CROSS CHECK,DAC MUTE
codec_write(client, 0x04,0x0c);   //pdn_ana=0,ibiasgen_pdn=0  
msleep(100);
codec_write(client, 0x2e,0x00); 
codec_write(client, 0x2f,0x00);
codec_write(client, 0x30,0x08); 
codec_write(client, 0x31,0x08);
msleep(200);
codec_write(client, 0x30,0x0f); 
codec_write(client, 0x31,0x0f);
msleep(200);
codec_write(client, 0x30,0x18); 
codec_write(client, 0x31,0x18);
msleep(100);
codec_write(client, 0x04,0x2c);   //pdn_ana=0,ibiasgen_pdn=0 		
}

static int es8323_reset(struct i2c_client *client)
{
	return codec_write(client, 0, 0x80);
}

void es8323_on(void)
{
	if(status == MODEM_OFF)	
	{
		printk("enter %s\n",__func__);
		es8323_reset(i2c_client);
		es8323_reg_init(i2c_client);
		//es8323_on_init(i2c_client);
		status = MODEM_ON;
	}
}
EXPORT_SYMBOL(es8323_on);

void es8323_off(void)
{
	if(status == MODEM_ON)	
	{
		printk("enter %s\n",__func__);
		es8323_reset(i2c_client);
		status = MODEM_OFF;
	}
}
EXPORT_SYMBOL(es8323_off);

static const struct i2c_device_id es8323_i2c_id[] = {
	{ "es8323_pcm", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, es8323_i2c_id);

static int es8323_proc_init(void);

static int __devinit es8323_i2c_probe(struct i2c_client *i2c,
		    const struct i2c_device_id *id)
{
	pr_info("%s(%d)\n", __func__, __LINE__);

	#ifdef ES8323_PROC	
	es8323_proc_init();
	#endif

	i2c_client = i2c;
	es8323_reset(i2c);
	status = MODEM_ON;
	es8323_off( );
	return 0;
}

static int __devexit es8323_i2c_remove(struct i2c_client *i2c)
{
	return 0;
}

struct i2c_driver es8323_i2c_driver = {
	.driver = {
		.name = "es8323_pcm",
		.owner = THIS_MODULE,
	},
	.probe = es8323_i2c_probe,
	.remove   = __devexit_p(es8323_i2c_remove),
	.id_table = es8323_i2c_id,
};

static int __init es8323_modinit(void)
{
	return i2c_add_driver(&es8323_i2c_driver);
}
late_initcall(es8323_modinit);

static void __exit es8323_modexit(void)
{
	i2c_del_driver(&es8323_i2c_driver);
}
module_exit(es8323_modexit);

MODULE_DESCRIPTION("ASoC ES8323 driver");
MODULE_AUTHOR("Johnny Hsu <johnnyhsu@realtek.com>");
MODULE_LICENSE("GPL");


#ifdef ES8323_PROC

static ssize_t es8323_proc_write(struct file *file, const char __user *buffer,
		unsigned long len, void *data)
{
	char *cookie_pot; 
	char *p;
	int reg;
	int value;

	cookie_pot = (char *)vmalloc( len );
	if (!cookie_pot) 
	{
		return -ENOMEM;
	} 
	else 
	{
		if (copy_from_user( cookie_pot, buffer, len )) 
			return -EFAULT;
	}

	switch(cookie_pot[0])
	{
		case 'r':
		case 'R':
			printk("Read reg debug\n");		
			if(cookie_pot[1] ==':')
			{
				strsep(&cookie_pot,":");
				while((p=strsep(&cookie_pot,",")))
				{
					reg = simple_strtol(p,NULL,16);
					value = codec_read(i2c_client,reg);
					printk("codec_read:0x%04x = 0x%04x\n",reg,value);
				}
				printk("\n");
			}
			else
			{
				printk("Error Read reg debug.\n");
				printk("For example: echo r:22,23,24,25>es8323_ts\n");
			}
			break;
		case 'w':
		case 'W':
			printk("Write reg debug\n");		
			if(cookie_pot[1] ==':')
			{
				strsep(&cookie_pot,":");
				while((p=strsep(&cookie_pot,"=")))
				{
					reg = simple_strtol(p,NULL,16);
					p=strsep(&cookie_pot,",");
					value = simple_strtol(p,NULL,16);
					codec_write(i2c_client,reg,value);
					printk("codec_write:0x%04x = 0x%04x\n",reg,value);
				}
				printk("\n");
			}
			else
			{
				printk("Error Write reg debug.\n");
				printk("For example: w:22=0,23=0,24=0,25=0>es8323_ts\n");
			}
			break;
		case 'a':
			printk("Dump reg \n");		

			for(reg = 0; reg < 0x6e; reg+=2)
			{
				value = codec_read(i2c_client,reg);
				printk("codec_read:0x%04x = 0x%04x\n",reg,value);
			}

			break;		
		default:
			printk("Help for es8323_ts .\n-->The Cmd list: \n");
			printk("-->'d&&D' Open or Off the debug\n");
			printk("-->'r&&R' Read reg debug,Example: echo 'r:22,23,24,25'>es8323_ts\n");
			printk("-->'w&&W' Write reg debug,Example: echo 'w:22=0,23=0,24=0,25=0'>es8323_ts\n");
			break;
	}

	return len;
}

static const struct file_operations es8323_proc_fops = {
	.owner		= THIS_MODULE,
};

static int es8323_proc_init(void)
{
	struct proc_dir_entry *es8323_proc_entry;
	es8323_proc_entry = create_proc_entry("driver/es8323_ts", 0777, NULL);
	if(es8323_proc_entry != NULL)
	{
		es8323_proc_entry->write_proc = es8323_proc_write;
		return 0;
	}
	else
	{
		printk("create proc error !\n");
		return -1;
	}
}
#endif





