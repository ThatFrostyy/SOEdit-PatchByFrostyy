/**** EulerAngles.h - Support for 24 angle schemes ****/
//https://github.com/erich666/GraphicsGems
/* Ken Shoemake, 1993 */
#ifndef _H_EulerAngles
#define _H_EulerAngles
#include "QuatTypes.h"
/*** Order type constants, constructors, extractors ***/
    /* There are 24 possible conventions, designated by:    */
    /*	  o EulAxI = axis used initially		    */
    /*	  o EulPar = parity of axis permutation		    */
    /*	  o EulRep = repetition of initial axis as last	    */
    /*	  o EulFrm = frame from which axes are taken	    */
    /* Axes I,J,K will be a permutation of X,Y,Z.	    */
    /* Axis H will be either I or K, depending on EulRep.   */
    /* Frame S takes axes from initial static frame.	    */
    /* If ord = (AxI=X, Par=Even, Rep=No, Frm=S), then	    */
    /* {a,b,c,ord} means Rz(c)Ry(b)Rx(a), where Rz(c)v	    */
    /* rotates v around Z by c radians.			    */
#define EulFrmS	     0
#define EulFrmR	     1
#define EulFrm(ord)  ((unsigned)(ord)&1)
#define EulRepNo     0
#define EulRepYes    1
#define EulRep(ord)  (((unsigned)(ord)>>1)&1)
#define EulParEven   0
#define EulParOdd    1
#define EulPar(ord)  (((unsigned)(ord)>>2)&1)
/* this code is merely a quick (and legal!) way to set arrays, EulSafe being 0,1,2,0 */
#define EulSafe	     "\000\001\002\000"
#define EulNext	     "\001\002\000\001"
#define EulAxI(ord)  ((int)(EulSafe[(((unsigned)(ord)>>3)&3)]))
#define EulAxJ(ord)  ((int)(EulNext[EulAxI(ord)+(EulPar(ord)==EulParOdd)]))
#define EulAxK(ord)  ((int)(EulNext[EulAxI(ord)+(EulPar(ord)!=EulParOdd)]))
#define EulAxH(ord)  ((EulRep(ord)==EulRepNo)?EulAxK(ord):EulAxI(ord))
    /* EulGetOrd unpacks all useful information about order simultaneously. */
#define EulGetOrd(ord,i,j,k,h,n,s,f) {unsigned o=(unsigned)ord;f=o&1;o>>=1;s=o&1;o>>=1;\
    n=o&1;o>>=1;i=EulSafe[o&3];j=EulNext[i+n];k=EulNext[i+1-n];h=s?k:i;}
    /* EulOrd creates an order value between 0 and 23 from 4-tuple choices. */
#define EulOrd(i,p,r,f)	   (((((((i)<<1)+(p))<<1)+(r))<<1)+(f))
    /* Static axes */
#define EulOrdXYZs    EulOrd(XEul,EulParEven,EulRepNo,EulFrmS)
#define EulOrdXYXs    EulOrd(XEul,EulParEven,EulRepYes,EulFrmS)
#define EulOrdXZYs    EulOrd(XEul,EulParOdd,EulRepNo,EulFrmS)
#define EulOrdXZXs    EulOrd(XEul,EulParOdd,EulRepYes,EulFrmS)
#define EulOrdYZXs    EulOrd(YEul,EulParEven,EulRepNo,EulFrmS)
#define EulOrdYZYs    EulOrd(YEul,EulParEven,EulRepYes,EulFrmS)
#define EulOrdYXZs    EulOrd(YEul,EulParOdd,EulRepNo,EulFrmS)
#define EulOrdYXYs    EulOrd(YEul,EulParOdd,EulRepYes,EulFrmS)
#define EulOrdZXYs    EulOrd(ZEul,EulParEven,EulRepNo,EulFrmS)
#define EulOrdZXZs    EulOrd(ZEul,EulParEven,EulRepYes,EulFrmS)
#define EulOrdZYXs    EulOrd(ZEul,EulParOdd,EulRepNo,EulFrmS)
#define EulOrdZYZs    EulOrd(ZEul,EulParOdd,EulRepYes,EulFrmS)
    /* Rotating axes */
#define EulOrdZYXr    EulOrd(XEul,EulParEven,EulRepNo,EulFrmR)
#define EulOrdXYXr    EulOrd(XEul,EulParEven,EulRepYes,EulFrmR)
#define EulOrdYZXr    EulOrd(XEul,EulParOdd,EulRepNo,EulFrmR)
#define EulOrdXZXr    EulOrd(XEul,EulParOdd,EulRepYes,EulFrmR)
#define EulOrdXZYr    EulOrd(YEul,EulParEven,EulRepNo,EulFrmR)
#define EulOrdYZYr    EulOrd(YEul,EulParEven,EulRepYes,EulFrmR)
#define EulOrdZXYr    EulOrd(YEul,EulParOdd,EulRepNo,EulFrmR)
#define EulOrdYXYr    EulOrd(YEul,EulParOdd,EulRepYes,EulFrmR)
#define EulOrdYXZr    EulOrd(ZEul,EulParEven,EulRepNo,EulFrmR)
#define EulOrdZXZr    EulOrd(ZEul,EulParEven,EulRepYes,EulFrmR)
#define EulOrdXYZr    EulOrd(ZEul,EulParOdd,EulRepNo,EulFrmR)
#define EulOrdZYZr    EulOrd(ZEul,EulParOdd,EulRepYes,EulFrmR)
//EulerAngles Eul_FromHMatrix(HMatrix M, int order);
#endif
