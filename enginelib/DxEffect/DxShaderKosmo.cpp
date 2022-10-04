#include "stdafx.h"

#include "DxEffectKosmo.h"

//	Note : 정적 변수 생성.
//
DWORD	DxEffectKosmo::m_dwEffect;


//	Note : Anisotropic 
//
char DxEffectKosmo::m_strEffect[] =
{
	
//#include "Constants.h"

// this shader projects a 1D-particle an sets the right texture for it
// I think there is a way to do it faster and with more precision - but I run out of time.

// this shader works in screenspace


//def c[CV_BINARYCONSTANTS], 0.5f, 0.25f, 0.125f, 0.0625f	// c0: 1/2, 1/4, 1/8, 1/16 

//
//
//      _------------_
//     /   \      /   \
//    |     |    |     | 
//   (   X   )  (   X   )
//    |     |    |     |
//     \   /      \   /
//      ~------------~
//
//       P0         P1
//
//    radius 0   radius 1

//
//  vertex 0            vertex 1
//
//      *------------------*
//      |                  |
//      |                  |
//      |   X          X   |
//      |                  |
//      |                  |
//      *------------------*
//
// vertex 3             vertex 2
//

// vertex 0:  v0: world coordinates of P0 (this)
//            v1: world coordinates of P1 (other)
//            v2: ( radius0, -radius0, 0, 0 )
// vertex 1:  v0: world coordinates of P1 (this)
//            v1: world coordinates of P0 (other)
//            v2: ( radius1, -radius1, 0.25, 0 )
// vertex 2:  v0: world coordinates of P1 (this)
//            v1: world coordinates of P0 (other)
//            v2: ( radius1, radius1, 0.25, 0.25 )
// vertex 3:  v0: world coordinates of P0 (this)
//            v1: world coordinates of P1 (other)
//            v2: ( radius0, radius0, 0, 0.25 )

//
// 0                      1
//   *------------------*
//   | \                |
//   |     \            |
//   |         \        |
//   |             \    |
//   |                \ |
//   *------------------*
// 3                      2
//
//
//


"vs.1.0	\n"


// transform this position						// r0
//
"m4x4 r0, v0, c[22]	\n"

// transform other position						// r1
//
"m4x4 r1, v1, c[22]	\n"


// perspective transform
"rcp r4.xy,r0.z			\n"
"mul r2.xy,r0.xy,r4.xy		\n"						// r2.xy: r0.xy/r0.z

"rcp r4.xy,r1.z			\n"
"mul r3.xy,r1.xy,r4.xy		\n"						// r3.xy: r1.xy/r1.z

// vector between P0 and P1 (2D view)
"sub r4.xy,r2.xy,r3.xy	\n"



// 2D distance between positions				// r8.x
//
"slt r4.z,r2.x,r2.x		\n"							// ignore z component 
"dp3 r4.z,r4,r4			\n"
"rsq r4.w,r4.z			\n"							// r4.w: 1/length

// normalize
"mul r4.xy,r4.xy,r4.w	\n"

"mul r8.x,r4.w,r4.z		\n"							// r8.x: length = sqrt( sqr(r2.x-r3.x)+sqr(r2.y-r3.y) )


// move corners with radius0 and radius1
"mov r5.xy,v2.x					\n"
"mad r0.xy,r4.xy,r5.xy,r0.xy	\n"					// extend horizontal

"mov r5.xy,-v3.y				\n"
"mad r1.xy,r4.xy,r5.xy,r1.xy	\n"					// extend horizontal


"rcp r11.xy,r0.z				\n"
"mul r2.xy,r0.xy,r11.xy			\n"					// r2.xy: r0.xy/r0.z

"rcp r11.xy,r1.z				\n"
"mul r3.xy,r1.xy,r11.xy			\n"					// r3.xy: r1.xy/r1.z

// vector between P0 and P1 (2D view)
"sub r11.xy,r2.xy,r3.xy	\n"
"slt r11.z,r2.x,r2.x		\n"							// ignore z component 
"dp3 r11.z,r11,r11			\n"
"rsq r11.w,r11.z			\n"							// r11.w: 1/length

"mul r8.x, r11.w, r8.x	\n"




"mov r5.xy,v2.y			\n"
"mul r6.xy,r4.xy,r5.xy	\n"
"add r0.x,r0.x,r6.y		\n"
"add r0.y,r0.y,-r6.x		\n"							// extend vertical

"mov oPos,r0	\n"




//fac=r8.x/(r8.x+v3.x+v3.y)
//fac=1/(1+v3.x/r8.x+v3.y/r8.x)
//fac=1/(1+v3.x*r4.w+v3.y*r4.w)


"mul r8.x,r8.x,v3.z			\n"


"mul r8.x,c[29].y,r8.x	\n"


// get length in a 4 bit binary form			// r3.x: ( bit3, bit2, bit1, bit0 )
//
"sge r3.x, r8.x, c[30].x			\n"		// r3.x: 0 or 1, bit 3
"mad r8.x, r3.x,-c[30].x, r8.x		\n"// r8.x: length has lost a bit

"sge r3.y, r8.x, c[30].y			\n"	// r3.x: 0 or 1, bit 2
"mad r8.x, r3.y,-c[30].y, r8.x		\n"// r8.x: length has lost a bit

"sge r3.z, r8.x, c[30].z			\n"	// r3.x: 0 or 1, bit 1
"mad r8.x, r3.z,-c[30].z, r8.x		\n"// r8.x: length has lost a bit

"sge r3.w, r8.x, c[30].w			\n"	// r3.x: 0 or 1, bit 0




// recombine bits for adressing in 4x4 array
"mad r3.x,r3.x,c[30].x,v2.w		\n"
"mad oT0.y,r3.y,c[30].y,r3.x	\n"

"mad r3.z,r3.z,c[30].x,v2.z		\n"
"mad oT0.x,r3.w,c[30].y,r3.z	\n"


"mov oD0.xyz, c28.z	\n"
"mov oD0.w, v5.x	\n"

};


DWORD	DxEffectKosmo::m_dwSavedStateBlock(0x00000000);
DWORD	DxEffectKosmo::m_dwEffectStateBlock(0x00000000);