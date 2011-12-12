/*
 *  Copyright 2009 Reality Jockey, Ltd.
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

#include "MessageInlet.h"
#include "PdGraph.h"

MessageInlet::MessageInlet(PdGraph *graph) : MessageObject(1, 1, graph) {
  numMessageInlets = 0;
  canvasX = 0;
}

MessageInlet::~MessageInlet() {
  numMessageInlets = 1;
}

const char *MessageInlet::getObjectLabel() {
  return "inlet";
}

ObjectType MessageInlet::getObjectType() {
  return MESSAGE_INLET;
}

void MessageInlet::processMessage(int inletIndex, PdMessage *message) {
  sendMessage(0, message);
}

List *MessageInlet::getProcessOrderFromInlet() {
  List *processList = new List();
  List *incomingMessageConnections = incomingMessageConnectionsListAtInlet[0];
  for (int i = 0; i < incomingMessageConnections->size(); i++) {
    ObjectLetPair *objectLetPair = (ObjectLetPair *) incomingMessageConnections->get(i);
    List *parentProcessList = objectLetPair->object->getProcessOrder();
    processList->add(parentProcessList);
    delete parentProcessList;
  }
  return processList;
}

int MessageInlet::getCanvasPosition() {
  return canvasX;
}

void MessageInlet::setCanvasPosition(int pos) {
  canvasX = pos;
}
