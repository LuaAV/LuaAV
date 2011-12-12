/*
 *  Copyright 2009,2010 Reality Jockey, Ltd.
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

#ifndef _DSP_MESSAGE_PRESEDENCE_H_
#define _DSP_MESSAGE_PRESEDENCE_H_

enum DspMessagePresedence {
  /** left-inlet: DSP, right-inline: DSP */
  DSP_DSP = 3,
  
  /** left-inlet: DSP, right-inline: MESSAGE */
  DSP_MESSAGE = 1,
  
  /** left-inlet: MESSAGE, right-inline: DSP */
  MESSAGE_DSP = 2,
  
  /** left-inlet: MESSAGE, right-inline: MESSAGE */
  MESSAGE_MESSAGE = 0
};

#endif // _DSP_MESSAGE_PRESEDENCE_H_
