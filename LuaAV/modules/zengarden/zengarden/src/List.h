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

#ifndef _LIST_H_
#define _LIST_H_

/**
 * Implements an ArrayList.
 */
class List {
  
  public:
    List();
  
    /** The contents of the list are not destroyed. */
    virtual ~List();
  
    /** The number of elements currently in the list. */
    int size();
  
    /** Add the element onto the end of the list */
    List *add(void *element);
  
    /**
     * Adds another List to the end of this List, in place.
     * The original List (i.e., this) is returned.
     */
    List *add(List *list);

    /** Returns the indexed element in the list. */
    void *get(int index);
  
    /**
     * Returns the index element from the backing array, regardless of how many elements there
     * are in the list.
     */
    void *getFromBackingArray(int index);
  
    void *remove(int index);

    /**
     * Resets the number of elements to zero.
     */
    void clear();
  
    void **arrayList;
    int numElements;

  private:
    void growArrayList();
  
    int maxLength;
};

#endif // _LIST_H_
