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

#include "MessagePrint.h"
#include "PdGraph.h"

MessagePrint::MessagePrint(PdMessage *initMessage, PdGraph *graph) : MessageObject(1, 0, graph) {
  if (initMessage->isSymbol(0)) {
    name = initMessage->isSymbol(0, "-n") ? NULL : StaticUtils::copyString(initMessage->getSymbol(0));
  } else {
    name = StaticUtils::copyString((char *) "print");
  }
}

MessagePrint::~MessagePrint() {
  free(name);
}

const char *MessagePrint::getObjectLabel() {
  return "print";
}

void MessagePrint::processMessage(int inletIndex, PdMessage *message) {
  char *out = message->toString();
  if (name != NULL) {
    graph->printStd("[@ %.3fms] %s: %s", message->getTimestamp(), name, out);
  } else {
    graph->printStd("[@ %.3fms] %s", message->getTimestamp(), out);
  }
  free(out);
}
