void glib_line(float x,float y, float x2,float y2, int color);
//  glicosa.c
void glib3_std_scene0(float xnear, float ynear, float znear);
void glib3_std_scene0_ortho(float xnear, float ynear, float znear,float size);
void glib3_icosahedron(float x,float y,float z,float size);
void glib3_bounding_box(float box);
void glib3_test_depth(void);

// glray3.c
void glib3_ray_change_alpha(float e);
void glib3_ray_change_beta(float e);
void glib3_ray_init();
void glib3_ray();
void glib3_ray_reshape(float w,float h); 
