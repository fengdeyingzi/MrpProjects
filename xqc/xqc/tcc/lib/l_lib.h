#ifndef L_LIB_H
#define L_LIB_H

int32 l_base_init(TCCState *s);
int32 l_sound_init(TCCState *s);
int32 l_bmp_init(TCCState *s);
int32 l_exb_init(TCCState *s);
int32 l_network_init(TCCState *s);
//int32 l_jgraphics_init(TCCState *s);
int32 l_image_init(TCCState *s);
int32 l_camera_init(TCCState *s);
int32 l_base_i_init(TCCState *s);
int32 l_math_init(TCCState *s);
int32 l_tcc_init(TCCState *s);

#endif