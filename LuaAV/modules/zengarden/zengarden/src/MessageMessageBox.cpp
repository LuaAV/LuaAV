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

#include "MessageMessageBox.h"
#include "PdGraph.h"

/*
 * The message box is overloaded with many kinds of functionality.
 * A) The simplest case is one where only one message is specified, including a list of primitives
 * which should be included in one message. The list may also include variable indicies (in the form
 * of, e.g. $1, $2, etc.) which refer to those locations in the incoming message which triggers
 * the message box.
 * B) A slightly more complicated case is where several messages in the form of A) are separated
 * by a comma (','). Each of the messages is processed and sent independently from the message box
 * when it is triggered.
 * C) The most complex case is where messages in the form of A) are separated by a semicolon (';').
 * The first symbol is the name of a message receiver. The remainder of the string is converted
 * into a message.
 *
 * NOTE(mhroth): MessageBoxes can only support up to 32 total messages.
 */
MessageMessageBox::MessageMessageBox(char *initString, PdGraph *graph) :
    MessageObject(1, 128, graph) {
  // parse the entire initialisation string
  List *messageInitListAll = StaticUtils::tokenizeString(initString, "\\;");
  
  // parse the first "message" for individual messages that should be sent from the outlet 
  localMessageList = new List();
  List *messageInitList = StaticUtils::tokenizeString((char *) messageInitListAll->get(0), "\\,");
  for (int i = 0; i < messageInitList->size(); i++) {
    char *initString = (char *) messageInitList->get(i);
    // StaticUtils::tokenizeString does not remove the trailing ";" from the
    // original string. We should not process it because it will result in an empty message. 
    if (strcmp(initString, ";") != 0) {
      PdMessage *message = new PdMessage(initString);
      localMessageList->add(message);
    }
  }
  StaticUtils::destroyTokenizedStringList(messageInitList);
  
  // parse the remainder of the init list for all remote messages
  remoteMessageList = new List();
  for (int i = 1; i < messageInitListAll->size(); i++) {
    char *initString = (char *) messageInitListAll->get(i);
    if (strcmp(initString, ";") != 0) {
      MessageNamedDestination *namedDestination =
          (MessageNamedDestination *) malloc(sizeof(MessageNamedDestination));
      // NOTE(mhroth): name string is not resolved
      namedDestination->name = StaticUtils::copyString(strtok(initString, " "));
      namedDestination->message = new PdMessage(strtok(NULL, ";"));
      remoteMessageList->add(namedDestination);
    }
  }
      
  StaticUtils::destroyTokenizedStringList(messageInitListAll);
}

MessageMessageBox::~MessageMessageBox() {
  // delete the message list and all of the messages in it
  for (int i = 0; i < localMessageList->size(); i++) {
    PdMessage *message = (PdMessage *) localMessageList->get(i);
    delete message;
  }
  delete localMessageList;
  
  // delete the remote message list
  for (int i = 0; i < remoteMessageList->size(); i++) {
    MessageNamedDestination *namedDestination =
        (MessageNamedDestination *) remoteMessageList->get(i);
    free(namedDestination->name);
    delete namedDestination->message;
    free(namedDestination);
  }
  delete remoteMessageList;
}

const char *MessageMessageBox::getObjectLabel() {
  return "msg";
}

void MessageMessageBox::processMessage(int inletIndex, PdMessage *message) {
  
  // send local messages
  int objMessageIndex = 0;
  for (int i = 0; i < localMessageList->size(); i++, objMessageIndex++) {
    PdMessage *messageTemplate = (PdMessage *) localMessageList->get(i);
    PdMessage *outgoingMessage = getNextResolvedMessage(objMessageIndex, messageTemplate, message);
    sendMessage(0, outgoingMessage);
  }
  
  // send remote messages
  for (int i = 0; i < remoteMessageList->size(); i++, objMessageIndex++) {
    MessageNamedDestination *namedDestination =
        (MessageNamedDestination *) remoteMessageList->get(i);
    PdMessage *outgoingMessage = getNextResolvedMessage(objMessageIndex,
        namedDestination->message, message);
    char *resolvedName = PdMessage::resolveString(namedDestination->name, message, 1);
    graph->sendMessageToNamedReceivers(resolvedName, outgoingMessage);
  }
}

PdMessage *MessageMessageBox::getNextResolvedMessage(int objMessageIndex,
    PdMessage *templateMessage, PdMessage *incomingMessage) {
  
  PdMessage *outgoingMessage = getNextOutgoingMessage(objMessageIndex);
  outgoingMessage->setTimestamp(incomingMessage->getTimestamp());
  for (int i = 0; i < templateMessage->getNumElements(); i++) {
    if (templateMessage->isSymbol(i)) {
      PdMessage::resolveElement(templateMessage->getSymbol(i), incomingMessage,
          outgoingMessage->getElement(i));
    }
  }
  
  return outgoingMessage;
}

PdMessage *MessageMessageBox::newCanonicalMessage(int outletIndex) {
  // outletIndex in this case is actually the objMessageIndex
  PdMessage *message = NULL;
  if (outletIndex < localMessageList->size()) {
    message = (PdMessage *) localMessageList->get(outletIndex);
  } else {
    outletIndex -= localMessageList->size();
    MessageNamedDestination *namedDestination =
        (MessageNamedDestination *) remoteMessageList->get(outletIndex);
    message = namedDestination->message;
  }
  return message->copy();
}
