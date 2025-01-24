#ifndef SHAKE_H
#define SHAKE_H

// Screen / View effects

// screen shake
extern int gmsgShake;


extern void V_ApplyShake( float* origin, float* angles, float factor );
extern void V_CalcShake( void );



#endif // SHAKE_H