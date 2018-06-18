/*---------------------------------------------------------------
   BIGJOB4.C -- Second thread approach to lengthy processing job
 ----------------------------------------------------------------*/

#define INCL_WIN
#define INCL_DOS

#include <os2.h>
#include <math.h>
#include <stdio.h>
#include "bigjob.h"

#define WM_CALC_DONE     (WM_USER + 0)
#define WM_CALC_ABORTED  (WM_USER + 1) 

VOID FAR SecondThread (VOID) ;

BOOL  bContinueCalc = FALSE ;
HWND  hwndClient ;
SHORT iCalcRep ;
LONG  lSemTrigger ;
TID   idThread ;
UCHAR cThreadStack [4096] ;

INT main (VOID)
     {
     static CHAR szClassName [] = "BigJob4" ;
     HMQ         hmq ;
     HWND        hwndFrame ;
     QMSG        qmsg ;
     ULONG       flCreateFlags = FCF_SIZEBORDER|FCF_TITLEBAR|FCF_SYSMENU|
					 FCF_MINMAX|FCF_MENU|FCF_SHELLPOSITION;

     hab = WinInitialize (NULL) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (hab, szClassName, ClientWndProc,
                            CS_SIZEREDRAW, 0) ;

     hwndFrame = WinCreateStdWindow (HWND_DESKTOP,
                    WS_VISIBLE, &flCreateFlags,
                    szClassName, "BigJob Demo No. 4",
                    0L, NULL, ID_RESOURCE, &hwndClient) ;

     while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
          WinDispatchMsg (hab, &qmsg) ;

     DosSuspendThread (idThread) ;

     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;

     return 0 ;
     }
	
MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1,
                                                     MPARAM mp2)
     {
     static SHORT iCurrentRep = IDM_10 ;
     static SHORT iStatus = STATUS_READY ;
     static ULONG lElapsedTime ;

     switch (msg)
          {
          case WM_CREATE:

               DosSemSet (&lSemTrigger) ;

               if (DosCreateThread (SecondThread, &idThread,
                                    cThreadStack + sizeof cThreadStack))

                    WinAlarm (HWND_DESKTOP, WA_ERROR) ;

               break ;

          case WM_COMMAND:

               switch (LOUSHORT (mp1))
                    {
                    case IDM_10:
                    case IDM_100:
                    case IDM_1000:
                    case IDM_10000:
                         CheckMenuItem (hwnd, iCurrentRep, FALSE) ;
                         iCurrentRep = LOUSHORT (mp1) ;
                         CheckMenuItem (hwnd, iCurrentRep, TRUE) ;
                         break ;

                    case IDM_START:
                         iStatus = STATUS_WORKING ;
                         WinInvalidateRect (hwnd, NULL, FALSE) ;

                         iCalcRep = iCurrentRep ;
                         bContinueCalc = TRUE ;
                         DosSemClear (&lSemTrigger) ;
          
                         EnableMenuItem (hwnd, IDM_START, FALSE) ;
                         EnableMenuItem (hwnd, IDM_ABORT, TRUE) ;
                         break ;

                    case IDM_ABORT:
                         bContinueCalc = FALSE ;

                         EnableMenuItem (hwnd, IDM_ABORT, FALSE) ;
                         break ;
     
                    default:
                         break ;
                    }
               break ;

          case WM_CALC_DONE:

               iStatus = STATUS_DONE ;
               lElapsedTime = mp1 ;
               WinInvalidateRect (hwnd, NULL, FALSE) ;

               EnableMenuItem (hwnd, IDM_START, TRUE) ;
               EnableMenuItem (hwnd, IDM_ABORT, FALSE) ;
               break ;

          case WM_CALC_ABORTED:

               iStatus = STATUS_READY ;
               WinInvalidateRect (hwnd, NULL, FALSE) ;

               EnableMenuItem (hwnd, IDM_START, TRUE) ;
               break ;

          case WM_PAINT:
               PaintWindow (hwnd, iStatus, iCalcRep, lElapsedTime) ;
               break ;

          default:
               return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
          }
     return 0L ;
     }

VOID FAR SecondThread ()
     {
     double A ;
     int    i ;
     LONG   lTime ;
     
     while (1)
          {
          DosSemWait (&lSemTrigger, -1L) ;

          lTime = WinGetCurrentTime (hab) ;

          for (A = 1.0, i = 0 ; i < iCalcRep ; i++)
               {
               if (!bContinueCalc)
                    break ;

               A = Savage (A) ;
               }

          lTime = WinGetCurrentTime (hab) - lTime ;
          
          DosSemSet (&lSemTrigger) ;

          if (bContinueCalc)
               WinPostMsg (hwndClient, WM_CALC_DONE, lTime, 0L) ;
          else
               WinPostMsg (hwndClient, WM_CALC_ABORTED, 0L, 0L) ;     
          }
     }

