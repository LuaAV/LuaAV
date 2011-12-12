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

#ifndef _DECLARE_LIST_H_
#define _DECLARE_LIST_H_

#include "ZGLinkedList.h"

/**
 * A DeclareList is resonsible for keeping track of [declare]d paths. It maintains ownership of all
 * path strings by copying them, and may modify them in order to maintain presentation consistency. Ultimately,
 * it is a list of paths where abstractions may be found. The DeclareList only maintains full paths.
 * If a path is [declare]d as a relative path, then it will be resolved to a pull path relative
 * to the root path of the original patch.
 * NOTE(mhroth): currently it does not do duplicate checking for given paths, though it probably should
 */
class DeclareList : public ZGLinkedList {
  
  public:
    DeclareList();
    ~DeclareList();
  
    /**
     * Add a full or relative path to the list. Relative paths are resolved relative to the
     * root path (which should be the first entry in the list).
     */
    void addPath(char *path);
  
    /** A convenience function returning the first entryin the list. */
    char *getRootPath();
  
    /** Returns true if the given path defines a full path, from root. */
    static bool isFullPath(char *path);
  
    /** Returns true if the given path has a trailing slash (indicating that it is a directory). */
    static bool hasTrailingSlash(char *path);  
};

#endif // _DECLARE_LIST_H_
