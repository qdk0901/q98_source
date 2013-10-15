/* Android Parameter */
static int ap_mdm = 0;
module_param(ap_mdm, int, 0644);
static int ap_has_alsa = 0;
module_param(ap_has_alsa, int, 0644);
static int ap_data_only = 1;
module_param(ap_data_only, int, 0644);
static int ap_has_earphone = 0;
module_param(ap_has_earphone, int, 0644);
static int lcd_density = 160;
module_param(lcd_density, int, 0644);
static int hwrotation = 270;
module_param(hwrotation, int, 0644);
//static int support_3g_dongle = true;
//module_param(support_3g_dongle, int ,0644);
static char *board_id = "unkown";
module_param(board_id, charp, 0644);

enum
{
	FORCE_USE_NONE,
	FORCE_USE_CODEC_RK616,
	FORCE_USE_CODEC_RT3261,
};

static int board_type = BOARD_Q97S_IPAD3;
static int panel_type = PANEL_LG97X03;
static int force_use_codec = FORCE_USE_NONE;

int get_board_type()
{
	return board_type;
}

int get_panel_type()
{
	return panel_type;
}

static int __init board_type_setup(char *str)
{
	if (!strcmp(str, "q98_ipad3"))
		board_type = BOARD_Q98_IPAD3;
	else if (!strcmp(str, "q98_ipad2"))
		board_type = BOARD_Q98_IPAD2;
	else if (!strcmp(str, "q97s_ipad3"))
		board_type = BOARD_Q97S_IPAD3;
	else if (!strcmp(str, "q97s_ipad2"))
		board_type = BOARD_Q97S_IPAD2;
	else if (!strcmp(str, "q910_101"))
			board_type = BOARD_Q910_101;
	else if (!strcmp(str, "q910_ipad2"))
		board_type = BOARD_Q910_IPAD2;
	else if (!strcmp(str, "q910_ipad3"))
		board_type = BOARD_Q910_IPAD3;
	else if (!strcmp(str, "q98_v2_ipad3"))
		board_type = BOARD_Q98_V2_IPAD3;
	else if (!strcmp(str, "q98_v2_ipad2"))
		board_type = BOARD_Q98_V2_IPAD2;
	else if (!strcmp(str, "find9"))
      board_type = BOARD_FINE9;
		
	printk("###############################[board_type = %s]##############################\n", str);
	return 0;
}

early_param("board_type", board_type_setup);

static int __init panel_type_setup(char *str)
{
	if (!strcmp(str, "panel_lg97x02"))
		panel_type = PANEL_LG97X02;
	else if (!strcmp(str, "panel_hsd97x02"))
		panel_type = PANEL_HSD97X02;
	else if (!strcmp(str, "panel_lg97x03"))
		panel_type = PANEL_LG97X03;
	else if (!strcmp(str, "panel_hsd101"))
		panel_type = PANEL_HSD101;
	else if (!strcmp(str, "panel_ltl089cl02w"))
		panel_type = PANEL_LTL089CL02W;
	else if (!strcmp(str, "panel_ltl090cl01w"))
		panel_type = PANEL_LTL090CL01W;
		
	printk("###############################[panel_type = %s]##############################\n", str);
	return 0;
}

early_param("panel_type", panel_type_setup);

static void remove_i2c_info(struct i2c_board_info* info, int size, char* item_to_erase)
{
		int i;
		for (i = 0; i < size; i++) {
			if (!strcmp(info[i].type, item_to_erase))
				strcpy(info[i].type, "removed");
		}
}

// q97s ipad2 specifics
static int rk29_backlight_io_init_q97s_q98_ipad2(void)
{
	int ret = 0;

	iomux_set(PWM_MODE);
	ret = gpio_request(BL_EN_PIN, "bl_en");
	if (ret == 0) {
		gpio_direction_output(BL_EN_PIN, BL_EN_VALUE);
	}
	return ret;
}

static int rk29_backlight_io_deinit_q97s_q98_ipad2(void)
{
	int ret = 0, pwm_gpio;
	gpio_free(BL_EN_PIN);
	pwm_gpio = iomux_mode_to_gpio(PWM_MODE);
	gpio_request(pwm_gpio, "bl_pwm");
	gpio_direction_output(pwm_gpio, GPIO_HIGH);
	return ret;		
}

static int rk29_backlight_pwm_suspend_q97s_q98_ipad2(void)
{
	int ret, pwm_gpio = iomux_mode_to_gpio(PWM_MODE);

	ret = gpio_request(pwm_gpio, "bl_pwm");
	if (ret) {
		printk("func %s, line %d: request gpio fail\n", __FUNCTION__, __LINE__);
		return ret;
	}
	gpio_direction_output(pwm_gpio, GPIO_HIGH);
	gpio_direction_output(BL_EN_PIN, !BL_EN_VALUE);
	return ret;
}

static int rk29_backlight_pwm_resume_q97s_q98_ipad2(void)
{
	int pwm_gpio = iomux_mode_to_gpio(PWM_MODE);

	gpio_free(pwm_gpio);
	iomux_set(PWM_MODE);
	msleep(150);
	gpio_direction_output(BL_EN_PIN, BL_EN_VALUE);
	return 0;
}

static void q97s_ipad2_override()
{
	lcd_density = 160;
	hwrotation = 270;
	
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "anx6345");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), GTP_I2C_NAME);
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "ft5506_q910");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "ft5506_q910_ipad3");
	remove_i2c_info(i2c4_info, ARRAY_SIZE(i2c4_info), "rk616");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "rk616");
	
	rk29_bl_info.io_init = rk29_backlight_io_init_q97s_q98_ipad2;
	rk29_bl_info.io_deinit = rk29_backlight_io_deinit_q97s_q98_ipad2;
	rk29_bl_info.pwm_suspend = rk29_backlight_pwm_suspend_q97s_q98_ipad2;
	rk29_bl_info.pwm_resume = rk29_backlight_pwm_resume_q97s_q98_ipad2;
	
	rk_headset_info.Headset_gpio = RK30_PIN0_PA1;
	rk_headset_info.headset_in_type = HEADSET_IN_LOW;
	rk_headset_info.Hook_gpio = RK30_PIN3_PD7;
	rk_headset_info.Hook_down_type = HOOK_DOWN_HIGH;
	
}

static void q98_ipad2_override()
{
	lcd_density = 160;
	hwrotation = 90;
	
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "anx6345");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "laibao_touch");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "vtl_ts");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "ft5506_q910");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "ft5506_q910_ipad3");
	remove_i2c_info(i2c4_info, ARRAY_SIZE(i2c4_info), "rk616");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "rk616");
	
	rk29_bl_info.io_init = rk29_backlight_io_init_q97s_q98_ipad2;
	rk29_bl_info.io_deinit = rk29_backlight_io_deinit_q97s_q98_ipad2;
	rk29_bl_info.pwm_suspend = rk29_backlight_pwm_suspend_q97s_q98_ipad2;
	rk29_bl_info.pwm_resume = rk29_backlight_pwm_resume_q97s_q98_ipad2;
	
	rk_headset_info.Headset_gpio = RK30_PIN0_PA7;
	rk_headset_info.headset_in_type = HEADSET_IN_HIGH;
	rk_headset_info.Hook_gpio = RK30_PIN3_PD7;
	rk_headset_info.Hook_down_type = HOOK_DOWN_HIGH;		
}


// q97s ipad3 specifics

static int rk_fb_io_init_q97s_q98_ipad3(struct rk29_fb_setting_info *fb_setting)
{
	int ret = 0;
	return 0;
}
static int rk_fb_io_disable_q97s_q98_ipad3(void)
{
	return 0;
}
static int rk_fb_io_enable_q97s_q98_ipad3(void)
{
	return 0;
}

static int rk29_backlight_io_init_q97s_q98_ipad3(void)
{
	int ret = 0;

	iomux_set(PWM_MODE);
	return ret;
}

static int rk29_backlight_io_deinit_q97s_q98_ipad3(void)
{
	int ret = 0, pwm_gpio;
	pwm_gpio = iomux_mode_to_gpio(PWM_MODE);
	gpio_request(pwm_gpio, "bl_pwm");
	gpio_direction_output(pwm_gpio, GPIO_HIGH);
	return ret;		
}

static int rk29_backlight_pwm_suspend_q97s_q98_ipad3(void)
{
	int ret, pwm_gpio = iomux_mode_to_gpio(PWM_MODE);

	ret = gpio_request(pwm_gpio, "bl_pwm");
	if (ret) {
		printk("func %s, line %d: request gpio fail\n", __FUNCTION__, __LINE__);
		return ret;
	}
	gpio_direction_output(pwm_gpio, GPIO_HIGH);
	return ret;
}

static int rk29_backlight_pwm_resume_q97s_q98_ipad3(void)
{
	int pwm_gpio = iomux_mode_to_gpio(PWM_MODE);

	gpio_free(pwm_gpio);
	iomux_set(PWM_MODE);
	return 0;
}

static void q97s_ipad3_override()
{
	lcd_density = 320;
	hwrotation = 270;
	
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), GTP_I2C_NAME);
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "ft5506_q910");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "ft5506_q910_ipad3");
	remove_i2c_info(i2c4_info, ARRAY_SIZE(i2c4_info), "rk616");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "rk616");
	
	lcdc0_screen_info.io_init = rk_fb_io_init_q97s_q98_ipad3;
	lcdc0_screen_info.io_disable = rk_fb_io_disable_q97s_q98_ipad3;
	lcdc0_screen_info.io_enable = rk_fb_io_enable_q97s_q98_ipad3;
#if defined(CONFIG_LCDC1_RK3188)
	lcdc1_screen_info.io_init = rk_fb_io_init_q97s_q98_ipad3;
	lcdc1_screen_info.io_disable = rk_fb_io_disable_q97s_q98_ipad3;
	lcdc1_screen_info.io_enable = rk_fb_io_enable_q97s_q98_ipad3;
#endif
		
	rk29_bl_info.io_init = rk29_backlight_io_init_q97s_q98_ipad3;
	rk29_bl_info.io_deinit = rk29_backlight_io_deinit_q97s_q98_ipad3;
	rk29_bl_info.pwm_suspend = rk29_backlight_pwm_suspend_q97s_q98_ipad3;
	rk29_bl_info.pwm_resume = rk29_backlight_pwm_resume_q97s_q98_ipad3;
	
	
	
	rk_headset_info.Headset_gpio = RK30_PIN0_PA1;
	rk_headset_info.headset_in_type = HEADSET_IN_LOW;
	rk_headset_info.Hook_gpio = RK30_PIN3_PD7;
	rk_headset_info.Hook_down_type = HOOK_DOWN_HIGH;
}

static void q98_ipad3_override()
{
	lcd_density = 320;
	hwrotation = 90;
	
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "laibao_touch");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "vtl_ts");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "ft5506_q910");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "ft5506_q910_ipad3");
	remove_i2c_info(i2c4_info, ARRAY_SIZE(i2c4_info), "rk616");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "rk616");

	lcdc0_screen_info.io_init = rk_fb_io_init_q97s_q98_ipad3;
	lcdc0_screen_info.io_disable = rk_fb_io_disable_q97s_q98_ipad3;
	lcdc0_screen_info.io_enable = rk_fb_io_enable_q97s_q98_ipad3;
	
#if defined(CONFIG_LCDC1_RK3188)
	lcdc1_screen_info.io_init = rk_fb_io_init_q97s_q98_ipad3;
	lcdc1_screen_info.io_disable = rk_fb_io_disable_q97s_q98_ipad3;
	lcdc1_screen_info.io_enable = rk_fb_io_enable_q97s_q98_ipad3;
#endif
		
	rk29_bl_info.io_init = rk29_backlight_io_init_q97s_q98_ipad3;
	rk29_bl_info.io_deinit = rk29_backlight_io_deinit_q97s_q98_ipad3;
	rk29_bl_info.pwm_suspend = rk29_backlight_pwm_suspend_q97s_q98_ipad3;
	rk29_bl_info.pwm_resume = rk29_backlight_pwm_resume_q97s_q98_ipad3;
	
	rk_headset_info.Headset_gpio = RK30_PIN0_PA7;
	rk_headset_info.headset_in_type = HEADSET_IN_HIGH;
	rk_headset_info.Hook_gpio = RK30_PIN3_PD7;
	rk_headset_info.Hook_down_type = HOOK_DOWN_HIGH;		
}

// q910 lvds

static int rk29_backlight_io_init_q910_lvds(void)
{
	int ret = 0;

	iomux_set(PWM_MODE);
	ret = gpio_request(BL_EN_PIN, "bl_en");
	if (ret != 0) {
		printk("%s request bl_en fail.");
		goto ret0;
  }
	gpio_direction_output(BL_EN_PIN, BL_EN_VALUE);
	
  ret = gpio_request(LED_EN_PIN, "led_en");
  if (ret != 0) {
    printk("%s request led_en fail.");
    goto ret0;
  }
		gpio_direction_output(LED_EN_PIN, LED_EN_VALUE);
ret0:
	return ret;
}

static int rk29_backlight_io_deinit_q910_lvds(void)
{
	int ret = 0, pwm_gpio;
	gpio_free(BL_EN_PIN);
  gpio_free(LED_EN_PIN);
	pwm_gpio = iomux_mode_to_gpio(PWM_MODE);
	gpio_request(pwm_gpio, "bl_pwm");
  gpio_direction_output(pwm_gpio, !BL_EN_VALUE);
	return ret;
}

static int rk29_backlight_pwm_suspend_q910_lvds(void)
{
	int ret, pwm_gpio = iomux_mode_to_gpio(PWM_MODE);

	ret = gpio_request(pwm_gpio, "bl_pwm");
	if (ret) {
		printk("func %s, line %d: request gpio fail\n", __FUNCTION__, __LINE__);
		return ret;
	}
  gpio_direction_output(pwm_gpio, GPIO_LOW);
	gpio_direction_output(BL_EN_PIN, !BL_EN_VALUE);
  gpio_direction_output(LED_EN_PIN, !LED_EN_VALUE);
	return ret;
}

static int rk29_backlight_pwm_resume_q910_lvds(void)
{
	int pwm_gpio = iomux_mode_to_gpio(PWM_MODE);

	gpio_free(pwm_gpio);
	iomux_set(PWM_MODE);
	msleep(30);
	gpio_direction_output(BL_EN_PIN, BL_EN_VALUE);
  gpio_direction_output(LED_EN_PIN, LED_EN_VALUE);
	return 0;
}

static void q910_101_override()
{
	lcd_density = 160;
	hwrotation = 270;
	
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "laibao_touch");
	//remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "vtl_ts");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), GTP_I2C_NAME);
	remove_i2c_info(i2c4_info, ARRAY_SIZE(i2c4_info), "rt3261");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "cat66121_hdmi");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "anx6345");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "ft5506_q910_ipad3");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "rk616");
	
	rk29_bl_info.io_init = rk29_backlight_io_init_q910_lvds;
	rk29_bl_info.io_deinit = rk29_backlight_io_deinit_q910_lvds;
	rk29_bl_info.pwm_suspend = rk29_backlight_pwm_suspend_q910_lvds;
	rk29_bl_info.pwm_resume = rk29_backlight_pwm_resume_q910_lvds;
	rk29_bl_info.bl_ref = 1;
	
	rk_headset_info.Headset_gpio = RK30_PIN0_PA1;
	rk_headset_info.headset_in_type = HEADSET_IN_LOW;
	rk_headset_info.Hook_gpio = RK30_PIN3_PD7;
	rk_headset_info.Hook_down_type = HOOK_DOWN_LOW;
	
	gpio_request(RK30_PIN0_PA1, NULL);
	gpio_direction_input(RK30_PIN0_PA1);
	gpio_pull_updown(RK30_PIN0_PA1, PullDisable);
	
	strcpy(rk_device_headset.name, "removed"); //FIXME
	
	signed char orientation[9] = {0, -1, 0, -1, 0, 0, 0, 0, -1};
	memcpy(mma8452_info.orientation, orientation, sizeof(signed char) * 9);
	
	if (panel_type == PANEL_LG97X02 || panel_type == PANEL_HSD97X02) {
		//vtl_ts_config_info.screen_max_x = 1024;
		vtl_ts_config_info.screen_max_y = 1185;
		vtl_ts_config_info.revert_x_flag = 0;
		//vtl_ts_config_info.revert_y_flag = 0;
		//vtl_ts_config_info.exchange_x_y_flag = 0;
		hwrotation = 90;
	}
}

//q910 ipad3

static int rk_fb_io_init_q910_ipad3(struct rk29_fb_setting_info *fb_setting)
{
	int ret = 0;
	return 0;
}

static int rk_fb_io_disable_q910_ipad3(void)
{
	return 0;
}

static int rk_fb_io_enable_q910_ipad3(void)
{
	return 0;
}

static int rk29_backlight_io_init_q910_ipad3(void)
{
	int ret = 0;

	iomux_set(PWM_MODE);
	return ret;
}

static int rk29_backlight_io_deinit_q910_ipad3(void)
{
	int ret = 0, pwm_gpio;
	pwm_gpio = iomux_mode_to_gpio(PWM_MODE);
	gpio_request(pwm_gpio, "bl_pwm");
	gpio_direction_output(pwm_gpio, GPIO_HIGH);
	return ret;		
}

static int rk29_backlight_pwm_suspend_q910_ipad3(void)
{
	int ret, pwm_gpio = iomux_mode_to_gpio(PWM_MODE);

	ret = gpio_request(pwm_gpio, "bl_pwm");
	if (ret) {
		printk("func %s, line %d: request gpio fail\n", __FUNCTION__, __LINE__);
		return ret;
	}
	gpio_direction_output(pwm_gpio, GPIO_HIGH);
	return ret;
}

static int rk29_backlight_pwm_resume_q910_ipad3(void)
{
	int pwm_gpio = iomux_mode_to_gpio(PWM_MODE);

	gpio_free(pwm_gpio);
	iomux_set(PWM_MODE);
	return 0;
}

static void q910_ipad3_override()
{
	lcd_density = 320;
	hwrotation = 270;
	
	//remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "laibao_touch");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "vtl_ts");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), GTP_I2C_NAME);
	remove_i2c_info(i2c4_info, ARRAY_SIZE(i2c4_info), "rt3261");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "cat66121_hdmi");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "ft5506_q910");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "rk616");
	
	rk29_bl_info.io_init = rk29_backlight_io_init_q910_ipad3;
	rk29_bl_info.io_deinit = rk29_backlight_io_deinit_q910_ipad3;
	rk29_bl_info.pwm_suspend = rk29_backlight_pwm_suspend_q910_ipad3;
	rk29_bl_info.pwm_resume = rk29_backlight_pwm_resume_q910_ipad3;
	rk29_bl_info.bl_ref = 0;
	
	lcdc0_screen_info.io_init = rk_fb_io_init_q910_ipad3;
	lcdc0_screen_info.io_disable = rk_fb_io_disable_q910_ipad3;
	lcdc0_screen_info.io_enable = rk_fb_io_enable_q910_ipad3;
	
#if defined(CONFIG_LCDC1_RK3188)
	lcdc1_screen_info.io_init = rk_fb_io_init_q910_ipad3;
	lcdc1_screen_info.io_disable = rk_fb_io_disable_q910_ipad3;
	lcdc1_screen_info.io_enable = rk_fb_io_enable_q910_ipad3;
#endif
	
	rk_headset_info.Headset_gpio = RK30_PIN0_PA1;
	rk_headset_info.headset_in_type = HEADSET_IN_LOW;
	rk_headset_info.Hook_gpio = RK30_PIN3_PD7;
	rk_headset_info.Hook_down_type = HOOK_DOWN_LOW;
	
	gpio_request(RK30_PIN0_PA1, NULL);
	gpio_direction_input(RK30_PIN0_PA1);
	gpio_pull_updown(RK30_PIN0_PA1, PullDisable);
	
	strcpy(rk_device_headset.name, "removed"); //FIXME
	signed char orientation[9] = {0, -1, 0, -1, 0, 0, 0, 0, -1};
	memcpy(mma8452_info.orientation, orientation, sizeof(signed char) * 9);
}

//q98 v2

static void q98_v2_ipad3_override()
{
	lcd_density = 320;
	hwrotation = 270;
	force_use_codec = FORCE_USE_CODEC_RT3261;
	
	//remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "vtl_ts");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), GTP_I2C_NAME);
	
	if (force_use_codec != FORCE_USE_CODEC_RT3261)
		remove_i2c_info(i2c4_info, ARRAY_SIZE(i2c4_info), "rt3261");
		
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "cat66121_hdmi");
	remove_i2c_info(i2c4_info, ARRAY_SIZE(i2c4_info), "rk616");
	//remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "rk616");
	
	rk29_bl_info.io_init = rk29_backlight_io_init_q910_ipad3;
	rk29_bl_info.io_deinit = rk29_backlight_io_deinit_q910_ipad3;
	rk29_bl_info.pwm_suspend = rk29_backlight_pwm_suspend_q910_ipad3;
	rk29_bl_info.pwm_resume = rk29_backlight_pwm_resume_q910_ipad3;
	rk29_bl_info.bl_ref = 0;
	
	lcdc0_screen_info.io_init = rk_fb_io_init_q910_ipad3;
	lcdc0_screen_info.io_disable = rk_fb_io_disable_q910_ipad3;
	lcdc0_screen_info.io_enable = rk_fb_io_enable_q910_ipad3;
	
#if defined(CONFIG_LCDC1_RK3188)
	lcdc1_screen_info.io_init = rk_fb_io_init_q910_ipad3;
	lcdc1_screen_info.io_disable = rk_fb_io_disable_q910_ipad3;
	lcdc1_screen_info.io_enable = rk_fb_io_enable_q910_ipad3;
#endif
	
	rk_headset_info.Headset_gpio = RK30_PIN0_PA1;
	rk_headset_info.headset_in_type = HEADSET_IN_LOW;
	rk_headset_info.Hook_gpio = RK30_PIN3_PD7;
	rk_headset_info.Hook_down_type = HOOK_DOWN_LOW;
	
	gpio_request(RK30_PIN0_PA1, NULL);
	gpio_direction_input(RK30_PIN0_PA1);
	gpio_pull_updown(RK30_PIN0_PA1, PullDisable);
	
	strcpy(rk_device_headset.name, "removed"); //FIXME
	
	vtl_ts_config_info.revert_x_flag = 1;
}

//fine9

static void fine9_override()
{
	lcd_density = 240;
	hwrotation = 270;
	
	//remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "vtl_ts");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "laibao_touch");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "ft5506_q910");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "ft5506_q910_ipad3");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), GTP_I2C_NAME);
	
	if (force_use_codec != FORCE_USE_CODEC_RT3261)
		remove_i2c_info(i2c4_info, ARRAY_SIZE(i2c4_info), "rt3261");
	
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "anx6345");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "cat66121_hdmi");
	remove_i2c_info(i2c2_info, ARRAY_SIZE(i2c2_info), "rk616");
	
	rk29_bl_info.io_init = rk29_backlight_io_init_q910_lvds;
	rk29_bl_info.io_deinit = rk29_backlight_io_deinit_q910_lvds;
	rk29_bl_info.pwm_suspend = rk29_backlight_pwm_suspend_q910_lvds;
	rk29_bl_info.pwm_resume = rk29_backlight_pwm_resume_q910_lvds;
	rk29_bl_info.bl_ref = 0;
	
	rk_headset_info.Headset_gpio = RK30_PIN0_PA1;
	rk_headset_info.headset_in_type = HEADSET_IN_HIGH;
	rk_headset_info.Hook_gpio = RK30_PIN3_PD7;
	rk_headset_info.Hook_down_type = HOOK_DOWN_HIGH;
	
	vtl_ts_config_info.screen_max_x = 1920;
	vtl_ts_config_info.screen_max_y = 1280;
	vtl_ts_config_info.revert_x_flag = 0;
	
	signed char orientation[9] = {0, -1, 0, -1, 0, 0, 0, 0, -1};
	memcpy(mma8452_info.orientation, orientation, sizeof(signed char) * 9);

#if defined (CONFIG_MFD_RK616)	
	rk616_pdata.spk_ctl_gpio = RK30_PIN0_PD4;
	rk616_pdata.mic_sel_gpio = RK30_PIN2_PD7;
	rk616_pdata.hp_ctl_gpio = RK30_PIN1_PB3;
#endif

	//ldo6
  act8846_ldo_info[5].min_uv = 1800000;
  act8846_ldo_info[5].max_uv = 1800000;
  
  rk30_adc_battery_platdata.set_charge_current = RK30_PIN1_PA5;
  if(gpio_request(RK30_PIN1_PA5, "charge_ctl") != 0) {
		printk("charge_ctl request fail!\n");
  } else {
		iomux_set(GPIO1_A5);
		gpio_direction_output(RK30_PIN1_PA5, GPIO_LOW);
  }
  
  rk29_bl_info.max_brightness = 255;
}

int board_use_rk616_codec()
{
	if (force_use_codec == FORCE_USE_CODEC_RT3261)
		return 0;
	// if MT815 is used, return 0 here
	if (board_type == BOARD_Q910_101 || board_type == BOARD_Q910_IPAD2 || board_type == BOARD_Q910_IPAD3 || board_type == BOARD_Q910_89 ||
		board_type == BOARD_Q98_V2_IPAD3 || board_type == BOARD_Q98_V2_IPAD2 || board_type == BOARD_FINE9)
		return 1;
	
	return 0;
}

int board_has_rk616()
{
	if (board_type == BOARD_Q910_101 || board_type == BOARD_Q910_IPAD2 || board_type == BOARD_Q910_IPAD3 || board_type == BOARD_Q910_89 ||
		board_type == BOARD_Q98_V2_IPAD3 || board_type == BOARD_Q98_V2_IPAD2 || board_type == BOARD_FINE9)
		return 1;
		
	return 0;	
}

int board_rk616_i2c_channel()
{
	if (board_type == BOARD_Q98_V2_IPAD3 || board_type == BOARD_Q98_V2_IPAD2)
		return 2;
	
	return 4;
}

int board_audio_path_fix()
{
	if (board_type == BOARD_Q910_IPAD2 || board_type == BOARD_Q910_IPAD3)
		return 1;
	
	return 0;
}

int get_host_drv_pin()
{
	if (board_type == BOARD_FINE9)
		return RK30_PIN0_PC5;
	
	return RK30_PIN0_PC0;
}

int get_otg_drv_pin()
{
	return RK30_PIN3_PD5;
}

int board_rotate_screen()
{
	if (board_type == BOARD_Q98_IPAD2 || board_type == BOARD_Q98_IPAD3)
		return 1;
		
	if (board_type == BOARD_Q910_101) {
		if (panel_type == PANEL_LG97X02 || panel_type == PANEL_HSD97X02)
			return 1;	
	}

	return 0;		
}

int board_custom_boot_logo()
{
	return 0;	
}

extern void camera_dynamic_init();

static void usb_detect_init()
{
	if (board_type == BOARD_Q98_IPAD3 || BOARD_Q98_IPAD2)
		board_usb_detect_init(RK30_PIN3_PD5);
	else
		board_usb_detect_init(RK30_PIN0_PA7);
}

static void boards_override()
{
	if (board_type == BOARD_Q97S_IPAD2)
		q97s_ipad2_override();
	else if (board_type == BOARD_Q97S_IPAD3)
		q97s_ipad3_override();
	else if (board_type == BOARD_Q98_IPAD2)
		q98_ipad2_override();
	else if (board_type == BOARD_Q98_IPAD3)
		q98_ipad3_override();
	else if (board_type == BOARD_Q910_101)
		q910_101_override();
	else if (board_type == BOARD_Q910_IPAD3)
		q910_ipad3_override();
	else if (board_type == BOARD_Q98_V2_IPAD3)
		q98_v2_ipad3_override();
	else if (board_type == BOARD_FINE9)
		fine9_override();
		
	camera_dynamic_init();
}