/*----------------------
   BIGJOB.H header file
  ----------------------*/

#define ID_RESOURCE 1

#define IDM_REPS    1
#define IDM_ACTION  2

#define IDM_10      10
#define IDM_100     100
#define IDM_1000    1000
#define IDM_10000   10000

#define IDM_START   20
#define IDM_ABORT   21

     /*-----------------------------------------------------
        Definitions, functions, and variables for BIGJOBx.C
       -----------------------------------------------------*/

#ifndef RC_INVOKED       /* This stuff not needed for .RC file */

#define STATUS_READY     0
#define STATUS_WORKING   1
#define STATUS_DONE      2

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

HAB  hab ;

double Savage (double A)
     {
     return tan (atan (exp (log (sqrt (A * A))))) + 1.0 ;
     }

VOID CheckMenuItem (HWND hwnd, SHORT iMenuItem, BOOL bCheck)
     {
     HWND hwndParent = WinQueryWindow (hwnd, QW_PARENT, FALSE) ;
     HWND hwndMenu   = WinWindowFromID (hwndParent, FID_MENU) ;

     WinSendMsg (hwndMenu, MM_SETITEMATTR, MPFROM2SHORT(iMenuItem, TRUE),
                 MPFROM2SHORT(MIA_CHECKED, bCheck ? MIA_CHECKED : 0)) ;
     }

VOID EnableMenuItem (HWND hwnd, SHORT iMenuItem, BOOL bEnable)
     {
     HWND hwndParent = WinQueryWindow (hwnd, QW_PARENT, FALSE) ;
     HWND hwndMenu   = WinWindowFromID (hwndParent, FID_MENU) ;

     WinSendMsg (hwndMenu, MM_SETITEMATTR, MPFROM2SHORT(iMenuItem, TRUE),
                 MPFROM2SHORT(MIA_DISABLED, bEnable ? 0 : MIA_DISABLED)) ;
     }

VOID PaintWindow (HWND hwnd, SHORT iStatus, SHORT iRep, LONG lTime)
     {
     static CHAR *szMessage [3] = { "Ready", "Working ...",
                                    "%d repetitions in %ld msec." } ;
     CHAR        szBuffer [60] ;
     HPS         hps ;
     RECTL        rc ;

     hps = WinBeginPaint (hwnd, NULL, NULL) ;
     GpiErase (hps) ;

     WinQueryWindowRect (hwnd, &rc) ;

     sprintf (szBuffer, szMessage [iStatus], iRep, lTime) ;

     WinDrawText (hps, -1, szBuffer, &rc, 0L, 0L, DT_TEXTATTRS | DT_CENTER | 
		 DT_VCENTER) ;
     WinEndPaint (hps) ;
     }

#endif

