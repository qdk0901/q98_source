#ifdef CONFIG_VIDEO_RK29
#include <plat/rk_camera.h>
/* Notes:

Simple camera device registration:

       new_camera_device(sensor_name,\       // sensor name, it is equal to CONFIG_SENSOR_X
                          face,\              // sensor face information, it can be back or front
                          pwdn_io,\           // power down gpio configuration, it is equal to CONFIG_SENSOR_POWERDN_PIN_XX
                          flash_attach,\      // sensor is attach flash or not
                          mir,\               // sensor image mirror and flip control information
                          i2c_chl,\           // i2c channel which the sensor attached in hardware, it is equal to CONFIG_SENSOR_IIC_ADAPTER_ID_X
                          cif_chl)  \         // cif channel which the sensor attached in hardware, it is equal to CONFIG_SENSOR_CIF_INDEX_X

Comprehensive camera device registration:

      new_camera_device_ex(sensor_name,\
                             face,\
                             ori,\            // sensor orientation, it is equal to CONFIG_SENSOR_ORIENTATION_X
                             pwr_io,\         // sensor power gpio configuration, it is equal to CONFIG_SENSOR_POWER_PIN_XX
                             pwr_active,\     // sensor power active level, is equal to CONFIG_SENSOR_RESETACTIVE_LEVEL_X
                             rst_io,\         // sensor reset gpio configuration, it is equal to CONFIG_SENSOR_RESET_PIN_XX
                             rst_active,\     // sensor reset active level, is equal to CONFIG_SENSOR_RESETACTIVE_LEVEL_X
                             pwdn_io,\
                             pwdn_active,\    // sensor power down active level, is equal to CONFIG_SENSOR_POWERDNACTIVE_LEVEL_X
                             flash_attach,\
                             res,\            // sensor resolution, this is real resolution or resoltuion after interpolate
                             mir,\
                             i2c_chl,\
                             i2c_spd,\        // i2c speed , 100000 = 100KHz
                             i2c_addr,\       // the i2c slave device address for sensor
                             cif_chl,\
                             mclk)\           // sensor input clock rate, 24 or 48
                          
*/
#define SUPPORT_MORE_CAMERA 1
static struct rkcamera_platform_data new_camera[] = { 
#if SUPPORT_MORE_CAMERA
   new_camera_device_ex(RK29_CAM_SENSOR_OV5640,
                        back,
                        INVALID_VALUE,
                        INVALID_VALUE,
                        INVALID_VALUE,
                        INVALID_VALUE,
                        INVALID_VALUE,
                        RK30_PIN3_PB5,
                        1, //active high
                        0, //flash attach
                        0x500000, //resolution 5MP
                        0x00, //mir
                        3,
                        100000,
                        0x78,
                        0,
                        24),
		new_camera_device_ex(RK29_CAM_SENSOR_GC2035,
	                front,
	                INVALID_VALUE,
	                INVALID_VALUE,
	                INVALID_VALUE,
	                INVALID_VALUE,
	                INVALID_VALUE,
	                RK30_PIN3_PB4,
	                1, //active high
	                0, //flash attach
	                0x200000, //resolution 2MP
	                0x00,
	                3,
	                100000,
	                0x78,
	                0,
	                24),
	new_camera_device_ex(RK29_CAM_SENSOR_GC2035,
                      back,
                      INVALID_VALUE,
                      INVALID_VALUE,
                      INVALID_VALUE,
                      INVALID_VALUE,
                      INVALID_VALUE,
                      RK30_PIN3_PB5,
                      1, //active high
                      0, //flash attach
                      0x200000, //resolution 2MP
                      0x00,
                      3,
                      100000,
                      0x78,
                      0,
                      24), 
	new_camera_device_ex(RK29_CAM_SENSOR_GC0308,
	                front,
	                INVALID_VALUE,
	                INVALID_VALUE,
	                INVALID_VALUE,
	                INVALID_VALUE,
	                INVALID_VALUE,
	                RK30_PIN3_PB4,
	                1, //active high
	                0, //flash attach
	                0x30000, //resolution 0.3MP
	                0x00,
	                3,
	                100000,
	                0x42,
	                0,
	                24),
#else
   new_camera_device_ex(RK29_CAM_SENSOR_OV5640,
                        back,
                        INVALID_VALUE,
                        INVALID_VALUE,
                        INVALID_VALUE,
                        INVALID_VALUE,
                        INVALID_VALUE,
                        RK30_PIN3_PB5,
                        1, //active high
                        0, //flash attach
                        0x500000, //resolution 5MP
                        0x00, //mir
                        3,
                        100000,
                        0x78,
                        0,
                        24),
	new_camera_device_ex(RK29_CAM_SENSOR_GC2035,
                      front,
                      INVALID_VALUE,
                      INVALID_VALUE,
                      INVALID_VALUE,
                      INVALID_VALUE,
                      INVALID_VALUE,
                      RK30_PIN3_PB4,
                      1, //active high
                      0, //flash attach
                      0x200000, //resolution 2MP
                      0x00,
                      3,
                      100000,
                      0x78,
                      0,
                      24), 
#endif     
    new_camera_device_end  
};

#endif  //#ifdef CONFIG_VIDEO_RK29
/*---------------- Camera Sensor Configuration Macro End------------------------*/
#include "../../../drivers/media/video/rk30_camera.c"
/*---------------- Camera Sensor Macro Define End  ---------*/

#define PMEM_CAM_SIZE PMEM_CAM_NECESSARY
/*****************************************************************************************
 * camera  devices
 * author: ddl@rock-chips.com
 *****************************************************************************************/
#ifdef CONFIG_VIDEO_RK29
#define CONFIG_SENSOR_POWER_IOCTL_USR	   1 //define this refer to your board layout
#define CONFIG_SENSOR_RESET_IOCTL_USR	   0
#define CONFIG_SENSOR_POWERDOWN_IOCTL_USR	   0
#define CONFIG_SENSOR_FLASH_IOCTL_USR	   0

static void rk_cif_power(struct rk29camera_gpio_res *res,int on)
{
	struct regulator *ldo_18,*ldo_28;
	int camera_power = res->gpio_power;
	  int camera_ioflag = res->gpio_flag;
	  int camera_io_init = res->gpio_init;
	  
	ldo_28 = regulator_get(NULL, "act_ldo8");	// vcc28_cif
	ldo_18 = regulator_get(NULL, "act_ldo3");	// vcc18_cif
	if (ldo_28 == NULL || IS_ERR(ldo_28) || ldo_18 == NULL || IS_ERR(ldo_18)){
		printk("get cif ldo failed!\n");
		return;
		}
	if(on == 0){
		while(regulator_is_enabled(ldo_28)>0)	
			regulator_disable(ldo_28);
		regulator_put(ldo_28);
		while(regulator_is_enabled(ldo_18)>0)
			regulator_disable(ldo_18);
		regulator_put(ldo_18);
		mdelay(10);
	if (camera_power != INVALID_GPIO)  {
		  if (camera_io_init & RK29_CAM_POWERACTIVE_MASK) {
			  gpio_set_value(camera_power, (((~camera_ioflag)&RK29_CAM_POWERACTIVE_MASK)>>RK29_CAM_POWERACTIVE_BITPOS));
			//	dprintk("%s..%s..PowerPin=%d ..PinLevel = %x	 \n",__FUNCTION__,res->dev_name, camera_power, (((~camera_ioflag)&RK29_CAM_POWERACTIVE_MASK)>>RK29_CAM_POWERACTIVE_BITPOS));
			}
		}
		}
	else{
		regulator_set_voltage(ldo_28, 2800000, 2800000);
		regulator_enable(ldo_28);
   //	printk("%s set ldo7 vcc28_cif=%dmV end\n", __func__, regulator_get_voltage(ldo_28));
		regulator_put(ldo_28);

		regulator_set_voltage(ldo_18, 1800000, 1800000);
	//	regulator_set_suspend_voltage(ldo, 1800000);
		regulator_enable(ldo_18);
	//	printk("%s set ldo1 vcc18_cif=%dmV end\n", __func__, regulator_get_voltage(ldo_18));
		regulator_put(ldo_18);
	if (camera_power != INVALID_GPIO)  {
		  if (camera_io_init & RK29_CAM_POWERACTIVE_MASK) {
			gpio_set_value(camera_power, ((camera_ioflag&RK29_CAM_POWERACTIVE_MASK)>>RK29_CAM_POWERACTIVE_BITPOS));
			//dprintk("%s..%s..PowerPin=%d ..PinLevel = %x	 \n",__FUNCTION__,res->dev_name, camera_power, ((camera_ioflag&RK29_CAM_POWERACTIVE_MASK)>>RK29_CAM_POWERACTIVE_BITPOS));
			mdelay(10);
			}
	}

	}
}

#if CONFIG_SENSOR_POWER_IOCTL_USR
static int sensor_power_usr_cb (struct rk29camera_gpio_res *res,int on)
{
	//#error "CONFIG_SENSOR_POWER_IOCTL_USR is 1, sensor_power_usr_cb function must be writed!!";
	rk_cif_power(res,on);
	return 0;
}
#endif

#if CONFIG_SENSOR_FLASH_IOCTL_USR
static int sensor_flash_usr_cb (struct rk29camera_gpio_res *res,int on)
{
	#error "CONFIG_SENSOR_FLASH_IOCTL_USR is 1, sensor_flash_usr_cb function must be writed!!";
}
#endif

static struct rk29camera_platform_ioctl_cb	sensor_ioctl_cb = {
	#if CONFIG_SENSOR_POWER_IOCTL_USR
	.sensor_power_cb = sensor_power_usr_cb,
	#else
	.sensor_power_cb = NULL,
	#endif

	#if CONFIG_SENSOR_RESET_IOCTL_USR
	.sensor_reset_cb = sensor_reset_usr_cb,
	#else
	.sensor_reset_cb = NULL,
	#endif

	#if CONFIG_SENSOR_POWERDOWN_IOCTL_USR
	.sensor_powerdown_cb = sensor_powerdown_usr_cb,
	#else
	.sensor_powerdown_cb = NULL,
	#endif

	#if CONFIG_SENSOR_FLASH_IOCTL_USR
	.sensor_flash_cb = sensor_flash_usr_cb,
	#else
	.sensor_flash_cb = NULL,
	#endif
};

static rk_sensor_user_init_data_s rk_init_data_sensor[RK_CAM_NUM] = 
{
};
#include "../../../drivers/media/video/rk30_camera.c"

void camera_dynamic_init()
{
	int board_type = get_board_type();
	if (board_type == BOARD_Q98_IPAD2 || board_type == BOARD_Q98_IPAD3 || board_type == BOARD_FINE9) {
#if SUPPORT_MORE_CAMERA
		new_camera[0].io.gpio_powerdown = RK30_PIN3_PB4;
		new_camera[1].io.gpio_powerdown = RK30_PIN3_PB5;
		new_camera[2].io.gpio_powerdown = RK30_PIN3_PB4;
		new_camera[3].io.gpio_powerdown = RK30_PIN3_PB5;
#else
		new_camera[0].io.gpio_powerdown = RK30_PIN3_PB4;
		new_camera[1].io.gpio_powerdown = RK30_PIN3_PB5;
#endif
		
		if (board_type == BOARD_FINE9)
			new_camera[1].mirror = 0x2; // upsize down
	}
}
#endif /* CONFIG_VIDEO_RK29 */
