#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "allocore/types/al_MsgQueue.hpp"

namespace al{

MsgQueue :: MsgQueue(int size, malloc_func mfunc, free_func ffunc) 
:	mHead(NULL), mTail(NULL), mLen(0), mChunkSize(size), mNow(0), 
	mMalloc(mfunc ? mfunc : malloc), mFree(ffunc ? ffunc : free)
{
	growPool();
}

MsgQueue :: ~MsgQueue() {
	Msg * m;
	while (mHead) {
		m = mHead->next;
		recycle(mHead);
		mHead = m;
	}
	while (mPool) {
		m = mPool->next;
		mFree(m);
		mPool = m;
	}
}

void MsgQueue :: growPool() {
	int size = mChunkSize;
	mPool = (Msg *)mMalloc(sizeof(Msg));
	Msg * m = mPool;
	while (size--) {
		m->next = (Msg *)mMalloc(sizeof(Msg));
		m = m->next;
	}
	m->next = NULL;
}

/* push a message back into the pool */
void MsgQueue :: recycle(Msg * m) {
	m->next = mPool;
	mPool = m;
	if (m->isBigMessage()) {
		char * args = *(char **)(m->mArgs);
		mFree(args);
	}
}

/* schedule a new message */
void MsgQueue :: sched(al_sec at, msg_func func, char * data, size_t size) {

	// get a message-holder from the pool:
	if (mPool == NULL) growPool();
	Msg * m = mPool;
	mPool= m->next;
	
	// prepare Msg:
	m->next = NULL;
	m->t = at;
	m->func = func;
	m->size = size;
	if (m->isBigMessage()) {
		// too big to fit in the Msg.
		char * args = (char *)mMalloc(size);
		memcpy(args, data, size);
		*(char **)(m->mArgs) = args;
	} else {
		memcpy(m->mArgs, data, size);
	}
	
	// insert into queue
	
	// empty queue? set as new head and tail:
	if (mHead == NULL) {
		mHead = m;
		mTail = m;
		mLen = 1;
		return;
	}
	
	// prepend case? insert as new head:
	if (at < mHead->t) {
		m->next = mHead;
		mHead = m;
		mLen++;
		return;
	}
	
	// append case? add as new tail:
	if (at >= mTail->t) {
		mTail->next = m;
		mTail = m;
		mLen++;
		return;
	}
	
	// else: insert somewhere between head and tail:
	Msg * p = mHead;
	Msg * n = p->next;
	// compare with <= so that events with same timestamp will be in order of insertion 
	while (n && n->t <= at) {
		p = n;
		n = n->next;
	}
	m->next = n;
	p->next = m;
	mLen++;
}

void MsgQueue :: update(al_sec until, bool defer) {
	Msg * m = mHead;
	while (m && m->t <= until) { 
		mHead = m->next;
		
//		if (defer && m->retry > 0.) {
//			m->msg.t = x->now + m->retry;
//			al_pq_sched_msg(x, m);
//		} else {	

		mNow = MAX(mNow, m->t); 
		(m->func)(mNow, m->args());
		
		recycle(m);
		m = mHead;
	}
	mNow = until;
}

} // al::
