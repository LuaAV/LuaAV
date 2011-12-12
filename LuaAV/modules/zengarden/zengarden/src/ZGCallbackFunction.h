/*
 *  Copyright 2010 Reality Jockey, Ltd.
 *                 info@rjdj.me
 *                 http://rjdj.me/
 * 
 *  This file is part of ZenGarden.
 *
 *  ZenGarden is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ZenGarden is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *  
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with ZenGarden.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _ZG_CALLBACK_FUNCTION_H_
#define _ZG_CALLBACK_FUNCTION_H_

/** An enumeration of the different operations available from a graph's callback function. */
typedef enum {
  /** Print to standard out. Argument is char* to message buffer. */
  ZG_PRINT_STD,
  
  /** Print to standard error. Argument is char* to message buffer. */
  ZG_PRINT_ERR,
  
  /**
   * Suggestion to turn on or off context signal processing. Argument is int* to 1 or 0.
   * Note that message and signal processing are dependent in ZenGarden. If PdContext::process()
   * is no longer called, then messages will also stop being processed.
   */
  ZG_PD_DSP
} ZGCallbackFunction;

#endif // _ZG_CALLBACK_FUNCTION_H_
