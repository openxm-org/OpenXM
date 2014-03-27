/* Wind data from this11/h-mle/FB2/s1_wind.c 
   The data average s1 and s2 is defined by 
   (s1,s2) = (sum(s1_i)/N,sum(s2_i)/N)
   where (s1_i,s2_i), i=1,2, ..., N are data on the S^1 (circle).
   s11 is defined as the average of s1_i*s1_i; s11=sum(s1_i*s1_i)/N.
   s12 is defined as the average of s1_i*s2_i; s11=sum(s1_i*s2_i)/N.
   s22 is defined as the average of s2_i*s2_i; s11=sum(s2_i*s2_i)/N.

*/
g_s11 = 0.662125; g_s12 = 0.274563; g_s22 = 0.337875; g_s1 = 0.317564; g_s2 = -0.020188;
/* Initial parameter for the HGD. */
g_x11 = 0.5; g_x12 = 0.5; g_x22 = 0.15; g_y1 = 1; g_y2 = 0.15; g_r = 1.0;
