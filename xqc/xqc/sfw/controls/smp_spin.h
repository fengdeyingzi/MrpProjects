#ifndef _SMP_SPIN_H
#define _SMP_SPIN_H

#include "window.h"

/**
  \defgroup smp_spin Simple Spin box

  To work with the simple spin box:
	- Create the spin window
	- Set the spin range
	- Add to the parent window
	- Response the spin notify messages
  
  \code
	//create a spin box
	hControl = SGL_CreateWindow(SMP_Spin_WndProc, ...);
	SMP_Spin_SetRange(hControl,  ...);
	SGL_AddChildWindow(hWnd, hControl);

	//response to spin notify messages
	case WM_COMMAND:
		WID id = LOWORD(wParam);
		WORD code = HIWORD(wParam);

		if(id == "the spin id")
		{
			switch(code)
			{
			case SMP_SPINN_VALUECHANGED:
				Sint32 value = (Sint32)lParam; // the spin current value
				//handle the notify event.
				break;
			}
		}	
  \endcode
  
  @ingroup controls
  @{
*/
	/**
	* \name Window Styles
	* @{
	*/

/**
 * \brief Indicates that left arrow pressed
 */
#define SMP_SPINS_LEFTARROW			0x0001L

/**
 * \brief Indicates that right arrow pressed
 */
#define SMP_SPINS_RIGHTARROW		0x0002L

	/** @} */

	/**
	 * \name Window Notify Messages
	 * @{
	 */

/**
 * \brief Sent when the spin window value changed.
 *
 * \code
 *	case WM_COMMAND:
 *		WID id = LOWORD(wParam);
 *		WORD code = HIWORD(wParam);
 *
 *		if(id == "the spin id" && code == SMP_SPINN_VALUECHANGED)
 *		{
			Sint32 value = (Sint32)lParam; // the spin current value
 * 			//handle the spin notify message
 *		}
 * \endcode
 *
 * \param hControl the spin box handle send this message
 */
#define SMP_SPINN_VALUECHANGED		0x0001

	/** @} */

	/**
	 * \name Window Member Functions
	 * @{
	 */

/**
 * \brief Set the range of the spin window
 *
 * \param hWnd the spin window handle
 * \param min the min value
 * \param max the max value
 * \param step the step movement
 */
VOID SMP_Spin_SetRange(HWND hWnd, Sint32 min, Sint32 max, Sint32 step);

/**
 * \brief Set the current value of the 
 *
 * \param hWnd the spin window handle
 * \param value the current value
 * \param notify if send the notify event
 * \param update if to update the window
 */
VOID SMP_Spin_SetValue(HWND hWnd, Sint32 value, BOOL notify, BOOL update);

/**
 * \brief Get the current value of the spin window
 *
 * \param hWnd the spin window handle
 * \return the current value
 */
Sint32 SMP_Spin_GetValue(HWND hWnd);

	/** @} */

	/**
	 * \name Window Procedure
	 * @{
	 */

/**
 * \brief Simple spin window procedure
 *
 * \param hWnd the window handle
 * \param Msg the window message 
 * \param wParam the first parameter
 * \param lParam the second parameter
 * \return the result of message process 
 */
LRESULT SMP_Spin_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	
	/** @} */

/** @} end of smp_spin */


#endif
