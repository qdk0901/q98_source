#define ADD_SCREEN(fn)\
void set_lcd_info_##fn(struct rk29fb_screen *screen, struct rk29lcd_info *lcd_info )\
{\
	int board = get_board_type();\
	screen->type = SCREEN_TYPE;\
	screen->face = OUT_FACE;\
	screen->lvds_format = LVDS_FORMAT;\
\
	\
	screen->x_res = H_VD;\
	screen->y_res = V_VD;\
\
	screen->width = LCD_WIDTH;\
	screen->height = LCD_HEIGHT;\
\
    \
	screen->lcdc_aclk = LCDC_ACLK;\
	screen->pixclock = DCLK;\
	screen->left_margin = H_BP;\
	screen->right_margin = H_FP;\
	screen->hsync_len = H_PW;\
	screen->upper_margin = V_BP;\
	screen->lower_margin = V_FP;\
	screen->vsync_len = V_PW;\
\
	\
	screen->pin_hsync = HSYNC_POL;\
	screen->pin_vsync = VSYNC_POL;\
	screen->pin_den = DEN_POL;\
	screen->pin_dclk = DCLK_POL;\
\
	\
	screen->swap_rb = SWAP_RB;\
	screen->swap_rg = SWAP_RG;\
	screen->swap_gb = SWAP_GB;\
	screen->swap_delta = 0;\
	screen->swap_dumy = 0;\
	if (board_has_rk616()) {\
		screen->sscreen_get = set_scaler_info_thunk;\
	}\
}\
size_t get_fb_size_##fn(void)\
{\
	size_t size = 0;\
	size = ((H_VD)*(V_VD)<<2)* 3;\
	return ALIGN(size,SZ_1M);\
}

#define SET_SCREEN_INFO(fn) set_lcd_info_##fn(screen, lcd_info)
#define GET_SCREEN_INFO(fn) get_fb_size_##fn() 
