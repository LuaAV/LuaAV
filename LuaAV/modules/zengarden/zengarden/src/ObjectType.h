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

#ifndef _OBJECT_TYPE_H_
#define _OBJECT_TYPE_H_

/** An enumeration all supported objects. */
enum ObjectType {
  DSP_ADC,
  DSP_ADD,
  DSP_BANDPASS_FILTER,
  DSP_CATCH,
  DSP_CLIP,
  DSP_COSINE,
  DSP_DAC,
  DSP_TABLE_PLAY,
  DSP_DELAY_READ,
  DSP_DELAY_WRITE,
  DSP_INLET,
  DSP_OUTLET,
  DSP_RECEIVE,
  DSP_SEND,
  DSP_TABLE_READ,
  DSP_TABLE_READ4,
  DSP_THROW,
  DSP_VARIABLE_DELAY,
  MESSAGE_INLET,
  MESSAGE_NOTEIN,
  MESSAGE_OUTLET,
  MESSAGE_RECEIVE,
  MESSAGE_SEND,
  MESSAGE_TABLE,
  MESSAGE_TABLE_READ,
  MESSAGE_TABLE_WRITE,
  OBJECT_PD,
  OBJECT_UNKNOWN // unknown or unimportant
};

#endif // _OBJECT_TYPE_H_
