/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell 
 * or otherwise commercially exploit the source or things you created based on the 
 * source.
 *
*/



#ifndef _RADAR_H
#define _RADAR_H

#include "radar/radarsetup.h"

extern int Radar_static_looping;

class object;
struct blip;
struct color;

extern void radar_init_std();
extern void radar_plot_object_std( object *objp );
extern void radar_frame_init_std();
extern void radar_mission_init_std();
extern void radar_frame_render_std(float frametime);

// observer hud rendering code uses this function
void radar_draw_blips_sorted_std(int distort);
void radar_draw_range_std();
void radar_blit_gauge_std();
void radar_stuff_blip_info_std(object *objp, int is_bright, color **blip_color, int *blip_type);
void radar_null_nblips_std();
void radar_draw_circle_std( int x, int y, int rad );
void radar_blip_draw_distorted_std(blip *b);
void radar_blip_draw_flicker_std(blip *b);
void radar_draw_image_std( int x, int y, int rad, int idx, int size);

class HudGaugeRadarStd: public HudGaugeRadar
{
	hud_frames Radar_gauge;

	int current_target_x, current_target_y;
	color radar_crosshairs;

	// formerly parts of Current_radar_global
	float Radar_center_offsets[2];

	float max_radius;	// Maximum radius of the plot area, default is to X-axis size

	// Ngon plot area
	float arc_length;	// Arc length of a sector (angle)
	float beta;			// Interrior angle of the radial vectors and the n-gon edges
	float gamma;		// Counterclockwise offset of the n-gon
	float r_min;		// Apothem of the polygon, which is the smallest radius
	int min_segments;
protected:
	/**
	 * @brief Clamps the incoming coordinates to be within the unit circle (or ngon)
	 *
	 * @param[in,out] x coord
	 * @param[in,out] y coord
	 * @details If the plotted coordinates is outside the plot area, it is radially projected onto the edge
	 */
	void clampBlip(float* x, float* y);

public:
	HudGaugeRadarStd();

	void initCenterOffsets(float x, float y);
	void initBitmaps(char *fname);

	void blipDrawDistorted(blip *b, int x, int y);
	void blipDrawFlicker(blip *b, int x, int y);
	void blitGauge();
	void drawBlips(int blip_type, int bright, int distort);
	void drawBlipsSorted(int distort);
	void drawContactCircle( int x, int y, int rad );
	void drawContactImage( int x, int y, int rad, int idx, int clr_idx, int size );
	void drawCrosshairs(int x, int y);
	void render(float frametime);
	void pageIn();
	void plotBlip(blip *b, int *x, int *y);
	void plotObject( object *objp );
};

#endif

