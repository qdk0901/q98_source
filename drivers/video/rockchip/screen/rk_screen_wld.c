#include <linux/rk_fb.h>

#if defined(CONFIG_RK_HDMI)
#include "../hdmi/rk_hdmi.h"
#endif
#if defined(CONFIG_MACH_RK_FAC)
#include <mach/config.h>
extern uint lcd_param[LCD_PARAM_MAX];
#endif

#include <plat/board.h>
#include "lcd_def.h"

static void* set_scaler_info_thunk;
/////
#include "lcd_undef.h"
#include "lcd_LG_LP097X02.c"
ADD_SCREEN(lcd_LG_LP097X02)

/////
#include "lcd_undef.h"
#include "lcd_hsd100pxn.c"
ADD_SCREEN(lcd_hsd100pxn)

/////
#include "lcd_undef.h"
#include "lcd_mq0801d.c"
ADD_SCREEN(lcd_mq0801d)

/////
#include "lcd_undef.h"
#include "lcd_LP097QX1.c"
ADD_SCREEN(lcd_LP097QX1)

#include "lcd_undef.h"
#include "lcd_LTL089CL02W_mipi.c"
ADD_SCREEN(lcd_LTL089CL02W_mipi)

#include "lcd_undef.h"
#include "lcd_LTL090CL01W_mipi.c"
ADD_SCREEN(lcd_LTL090CL01W_mipi)

/////
#include "lcd_undef.h"
#include "lcd_hsd_1280x800_rk3188_rk616.c"
ADD_SCREEN(lcd_hsd_1280x800_rk3188_rk616)

#include "lcd_undef.h"
#include "lcd_hsd_1024x768_rk3188_rk616.c"
ADD_SCREEN(lcd_hsd_1024x768_rk3188_rk616)

#include "lcd_undef.h"
#include "lcd_lg_1024x768_rk3188_rk616.c"
ADD_SCREEN(lcd_lg_1024x768_rk3188_rk616)

// if we use one lcdc with jetta for dual display,we need these configration
#if defined(CONFIG_ONE_LCDC_DUAL_OUTPUT_INF) && defined(CONFIG_RK_HDMI)
static int set_scaler_info(struct rk29fb_screen *screen, u8 hdmi_resolution)
{
	#if defined(CONFIG_RK610_LVDS)
	screen->s_clk_inv = S_DCLK_POL;
	screen->s_den_inv = 0;
	screen->s_hv_sync_inv = 0;
	#endif
	
	switch(hdmi_resolution)
	{
	case HDMI_1920x1080p_60Hz:
                /* Scaler Timing    */
	#if defined(CONFIG_RK610_LVDS)
		screen->hdmi_resolution = hdmi_resolution;
		screen->s_pixclock = S_OUT_CLK;
		screen->s_hsync_len = S_H_PW;
		screen->s_left_margin = S_H_BP;
		screen->s_right_margin = S_H_FP;
		screen->s_hsync_len = S_H_PW;
		screen->s_upper_margin = S_V_BP;
		screen->s_lower_margin = S_V_FP;
		screen->s_vsync_len = S_V_PW;
		screen->s_hsync_st = S_H_ST;
		screen->s_vsync_st = S_V_ST;
	#endif

		//bellow are for JettaB
	#if defined(CONFIG_RK616_LVDS)
		screen->pll_cfg_val = S_PLL_CFG_VAL;
		screen->frac	    = S_FRAC;
		screen->scl_vst	    = S_SCL_VST;
		screen->scl_hst     = S_SCL_HST;
		screen->vif_vst     = S_VIF_VST;
		screen->vif_hst     = S_VIF_HST;
	#endif
		break;
	case HDMI_1920x1080p_50Hz:
                /* Scaler Timing    */
	#if defined(CONFIG_RK610_LVDS)
		screen->hdmi_resolution = hdmi_resolution;
		screen->s_pixclock = S1_OUT_CLK;
		screen->s_hsync_len = S1_H_PW;
		screen->s_left_margin = S1_H_BP;
		screen->s_right_margin = S1_H_FP;
		screen->s_hsync_len = S1_H_PW;
		screen->s_upper_margin = S1_V_BP;
		screen->s_lower_margin = S1_V_FP;
		screen->s_vsync_len = S1_V_PW;
		screen->s_hsync_st = S1_H_ST;
		screen->s_vsync_st = S1_V_ST;
	#endif

	#if defined(CONFIG_RK616_LVDS)
		screen->pll_cfg_val = S1_PLL_CFG_VAL;
		screen->frac	    = S1_FRAC;
		screen->scl_vst	    = S1_SCL_VST;
		screen->scl_hst     = S1_SCL_HST;
		screen->vif_vst     = S1_VIF_VST;
		screen->vif_hst     = S1_VIF_HST;
	#endif
		break;
	case HDMI_1280x720p_60Hz:
                /* Scaler Timing    */
	#if defined(CONFIG_RK610_LVDS)
		screen->hdmi_resolution = hdmi_resolution;
		screen->s_pixclock = S2_OUT_CLK;
		screen->s_hsync_len = S2_H_PW;
		screen->s_left_margin = S2_H_BP;
		screen->s_right_margin = S2_H_FP;
		screen->s_hsync_len = S2_H_PW;
		screen->s_upper_margin = S2_V_BP;
		screen->s_lower_margin = S2_V_FP;
		screen->s_vsync_len = S2_V_PW;
		screen->s_hsync_st = S2_H_ST;
		screen->s_vsync_st = S2_V_ST;
	#endif
	
	#if defined(CONFIG_RK616_LVDS)
		screen->pll_cfg_val = S2_PLL_CFG_VAL;
		screen->frac	    = S2_FRAC;
		screen->scl_vst	    = S2_SCL_VST;
		screen->scl_hst     = S2_SCL_HST;
		screen->vif_vst     = S2_VIF_VST;
		screen->vif_hst     = S2_VIF_HST;
	#endif
		break;
	case HDMI_1280x720p_50Hz:
                /* Scaler Timing    */
	#if defined(CONFIG_RK610_LVDS)
		screen->hdmi_resolution = hdmi_resolution;
		screen->s_pixclock = S3_OUT_CLK;
		screen->s_hsync_len = S3_H_PW;
		screen->s_left_margin = S3_H_BP;
		screen->s_right_margin = S3_H_FP;
		screen->s_hsync_len = S3_H_PW;
		screen->s_upper_margin = S3_V_BP;
		screen->s_lower_margin = S3_V_FP;
		screen->s_vsync_len = S3_V_PW;
		screen->s_hsync_st = S3_H_ST;
		screen->s_vsync_st = S3_V_ST;
	#endif
	
	#if defined(CONFIG_RK616_LVDS)
		screen->pll_cfg_val = S3_PLL_CFG_VAL;
		screen->frac	    = S3_FRAC;
		screen->scl_vst	    = S3_SCL_VST;
		screen->scl_hst     = S3_SCL_HST;
		screen->vif_vst     = S3_VIF_VST;
		screen->vif_hst     = S3_VIF_HST;
	#endif
		break;
	case HDMI_720x576p_50Hz_4_3:
	case HDMI_720x576p_50Hz_16_9:
                /* Scaler Timing    */
	#if defined(CONFIG_RK610_LVDS)
		screen->hdmi_resolution = hdmi_resolution;
		screen->s_pixclock = S4_OUT_CLK;
		screen->s_hsync_len = S4_H_PW;
		screen->s_left_margin = S4_H_BP;
		screen->s_right_margin = S4_H_FP;
		screen->s_hsync_len = S4_H_PW;
		screen->s_upper_margin = S4_V_BP;
		screen->s_lower_margin = S4_V_FP;
		screen->s_vsync_len = S4_V_PW;
		screen->s_hsync_st = S4_H_ST;
		screen->s_vsync_st = S4_V_ST;
	#endif
	
	#if defined(CONFIG_RK616_LVDS)
		screen->pll_cfg_val = S4_PLL_CFG_VAL;
		screen->frac	    = S4_FRAC;
		screen->scl_vst	    = S4_SCL_VST;
		screen->scl_hst     = S4_SCL_HST;
		screen->vif_vst     = S4_VIF_VST;
		screen->vif_hst     = S4_VIF_HST;
	#endif
		break;
		
	case HDMI_720x480p_60Hz_16_9:
	case HDMI_720x480p_60Hz_4_3:
                /* Scaler Timing    */
	#if defined(CONFIG_RK610_LVDS)
		screen->hdmi_resolution = hdmi_resolution;
		screen->s_pixclock = S5_OUT_CLK;
		screen->s_hsync_len = S5_H_PW;
		screen->s_left_margin = S5_H_BP;
		screen->s_right_margin = S5_H_FP;
		screen->s_hsync_len = S5_H_PW;
		screen->s_upper_margin = S5_V_BP;
		screen->s_lower_margin = S5_V_FP;
		screen->s_vsync_len = S5_V_PW;
		screen->s_hsync_st = S5_H_ST;
		screen->s_vsync_st = S5_V_ST;
	#endif
	
	#if defined(CONFIG_RK616_LVDS)
		screen->pll_cfg_val = S5_PLL_CFG_VAL;
		screen->frac	    = S5_FRAC;
		screen->scl_vst	    = S5_SCL_VST;
		screen->scl_hst     = S5_SCL_HST;
		screen->vif_vst     = S5_VIF_VST;
		screen->vif_hst     = S5_VIF_HST;
	#endif
		break;
	default :
            	printk("%s lcd not support dual display at this hdmi resolution %d \n",__func__,hdmi_resolution);
            	return -1;
	        break;
	}
	
	return 0;
}
#else
#define set_scaler_info  NULL
#endif

void set_lcd_info(struct rk29fb_screen *screen, struct rk29lcd_info *lcd_info )
{
	set_scaler_info_thunk = set_scaler_info;
	
	int panel_type = get_panel_type();
	int board_type = get_board_type();
	
	if (panel_type == PANEL_LG97X02) {
		if (board_type == BOARD_Q910_101)
			SET_SCREEN_INFO(lcd_lg_1024x768_rk3188_rk616);
		else
			SET_SCREEN_INFO(lcd_LG_LP097X02);
	}
	else if (panel_type == PANEL_HSD97X02) {
		if (board_type == BOARD_Q910_101)
			SET_SCREEN_INFO(lcd_hsd_1024x768_rk3188_rk616);
		else
			SET_SCREEN_INFO(lcd_hsd100pxn);
	}
	else if (panel_type == PANEL_LG97X03)
		SET_SCREEN_INFO(lcd_LP097QX1);
	else if (panel_type == PANEL_HSD101)
		SET_SCREEN_INFO(lcd_hsd_1280x800_rk3188_rk616);
	else if (panel_type == PANEL_LTL089CL02W)
		SET_SCREEN_INFO(lcd_LTL089CL02W_mipi);
	else if (panel_type == PANEL_LTL090CL01W)
		SET_SCREEN_INFO(lcd_LTL090CL01W_mipi);
	else if (panel_type == PANEL_MQ0801D)
		SET_SCREEN_INFO(lcd_mq0801d);
}

size_t get_fb_size(void)
{
	int panel_type = get_panel_type();
	int board_type = get_board_type();
	
	if (panel_type == PANEL_LG97X02) {
		if (board_type == BOARD_Q910_101)
			return GET_SCREEN_INFO(lcd_lg_1024x768_rk3188_rk616);
		else
			return GET_SCREEN_INFO(lcd_LG_LP097X02);
	}
	else if (panel_type == PANEL_HSD97X02) {
		if (board_type == BOARD_Q910_101)
			return GET_SCREEN_INFO(lcd_hsd_1024x768_rk3188_rk616);
		else
			return GET_SCREEN_INFO(lcd_hsd100pxn);
	}
	else if (panel_type == PANEL_LG97X03)
		return GET_SCREEN_INFO(lcd_LP097QX1);
	else if (panel_type == PANEL_HSD101)
		return GET_SCREEN_INFO(lcd_hsd_1280x800_rk3188_rk616);
	else if (panel_type == PANEL_LTL089CL02W)
		return GET_SCREEN_INFO(lcd_LTL089CL02W_mipi);
	else if (panel_type == PANEL_LTL090CL01W)
		return GET_SCREEN_INFO(lcd_LTL090CL01W_mipi);
	else if (panel_type == PANEL_MQ0801D)
		return GET_SCREEN_INFO(lcd_mq0801d);
	return 0;	
}