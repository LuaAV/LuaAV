#include "types/al_pq.h"

#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "math.h"

#ifndef HUGE_VALF
	#define HUGE_VALF 1e38f
#endif

#define ddebug(...) 
//#define ddebug(...) printf(__VA_ARGS__)

void * pq_default_allocator(void * userdata, size_t size) {
	return malloc(size);
}

/* allocate a new priority queue */
pq al_pq_create(int size, al_sec birth, pq_custom_allocator alloc, void * alloc_userdata) {
	pq_msg array;
	int i;
	pq x = (pq)malloc(sizeof(struct al_pq));
	if (x == NULL) return x;
	x->alloc_func = alloc != NULL ? alloc : pq_default_allocator;
	x->alloc_func_userdata = alloc_userdata;
	array = (pq_msg)(x->alloc_func)(x->alloc_func_userdata, size * sizeof(struct al_pq_msg));
	if (x && array) {
		x->head = x->tail = NULL;
		for (i=0; i<size; i++) {
			memset(array[i].msg.mem, 0, AL_PQ_MSG_ARGS_SIZE);
			array[i].msg.func = NULL;
			array[i].retry = 0;
			if (i==size-1) {
				array[i].next = NULL;
			} else {
				array[i].next = &array[i+1];
			}
		}
		x->pool = array;
		x->len = 0;
		x->now = birth;
		x->retry_period = 1;
		ddebug("pq using %i bytes\n", sizeof(struct al_pq) + sizeof(struct al_pq_msg) * size);
	}
	return x;
}

/* free a priority queue */
void al_pq_free(pq * ptr, int flush) {
	pq x;
	if (ptr && *ptr) {
		x = *ptr;
		if (flush) {
			/* flush scheduled messages */
			al_pq_update(x, HUGE_VALF, 0);
		}
		free(x);
		*ptr = 0;
	}
}

/* grab memory for the next message */
char * al_pq_msg(pq x) {
	if (x->pool == NULL) {
		x->pool = (pq_msg)(x->alloc_func)(x->alloc_func_userdata, sizeof(struct al_pq_msg));
		x->pool->next = NULL;
	}
	return x->pool->msg.mem;
}

/* (re?)schedule a message */
void al_pq_sched_msg(pq x, pq_msg m) {
	pq_msg p, n;
	m->next = NULL;
	
	/* fill new msg struct */
	
	/* insert into queue */
	
	/* empty queue case */
	if (x->head == NULL) {
		ddebug("al_pq_sched case: empty %f %f\n", x->now, m->msg.t);
		x->head = m;
		x->tail = m;
		x->len = 1;
		return;
	}
	
	ddebug("head %f ", x->head->msg.t);
	
	/* prepend case */
	if (m->msg.t < x->head->msg.t) {
		ddebug("al_pq_sched case: prepend %f %f\n", x->now, m->msg.t);
		m->next = x->head;
		x->head = m;
		x->len++;
		ddebug("new head %f\n", x->head->msg.t);
		return;
	}
	
	/* append case */
	if (m->msg.t >= x->tail->msg.t) {
		ddebug("al_pq_sched case: append %f %f\n", x->now, m->msg.t);
		x->tail->next = m;
		x->tail = m;
		x->len++;
		return;
	}
	
	/* somewhere between head & tail */
	ddebug("al_pq_sched case: insert %f %f\n", x->now, m->msg.t);
	p = x->head;
	n = x->head->next;
	/* compare with <= so that events with same timestamp will be in order of insertion */
	while (n && n->msg.t <= m->msg.t) {
		p = n;
		n = n->next;
	}
	m->next = n;
	p->next = m;
	x->len++;
}

/* schedule a new message */
void al_pq_sched(pq x, al_sec t, al_sec retry, al_msg_func f) {
	pq_msg m;
	
	/* pop from pool */
	m = x->pool;
	if (m == NULL) {
		m = (pq_msg)(x->alloc_func)(x->alloc_func_userdata, sizeof(struct al_pq_msg));
	} else {
		x->pool = m->next;
	}
	
	/* fill new msg struct */
	m->next = NULL;
	m->msg.t = t;
	m->msg.func = f;
	m->retry = retry;
	
	/* insert into queue */
	al_pq_sched_msg(x, m);
}

/* push a message back into the pool */
void al_pq_recycle(pq x, pq_msg m) {
	m->next = x->pool;
	x->pool = m;
}


// TODO: this is wrong; need to check value of mem, not address!
void al_pq_cancel_ptr(pq x, void * ptr) {
	// iterate entire queue
	pq_msg p, n, m;
	p = x->head;
	
	//printf("cancel pq msg %p (%i)\n", ptr, x->len);
	
	/* check head(s) */
	while (p && *(void **)p->msg.mem == ptr) {
		x->head = p->next;
		p->next = x->pool;
		x->pool = p;
		p = x->head;
	}
	
	/* make sure there are still values */
	if (p == NULL) return;
	
	n = p->next;
	while (n) {
		if (*(void **)n->msg.mem == ptr) {
			// todo: verify this
			m = n->next;
			p->next = m;
			al_pq_recycle(x, n);
			n = m;
		} else {
			p = n;
			n = n->next;
		}
	}
}

/* free one */
void al_pq_msg_free(pq_msg * ptr) {
	// TODO: use a per-queue pool?
	pq_msg x;
	if (ptr && *ptr) {
		x = *ptr;
		free(x);
		*ptr = 0;
	}
}

/* read the top message */
pq_msg al_pq_top(pq x) {
	return x->head;
}
/* pop the top message, return the next */
pq_msg al_pq_pop(pq x) {
	pq_msg n = NULL;
	if (x->head) {
		n = x->head;
		x->head = n->next;
		x->len--;
	}
	return n;
}

/* convenience wrapper to call all scheduled functions up to a given timestamp */
void al_pq_update(pq x, al_sec until, int defer) {
	int result;
	pq_msg m = al_pq_top(x);
	ddebug("update to %f\n", until);
	while (m && m->msg.t < until) {
		x->now = MAX(x->now, m->msg.t); 
		al_pq_pop(x);
		ddebug("call next %f\n", m->msg.t);
		
		// deferable?
		if (defer && m->retry > 0.) {
			m->msg.t = x->now + m->retry;
			al_pq_sched_msg(x, m);
		} else {	
			result = m->msg.func(m->msg.t, m->msg.mem);
			if (result) {
				/* message was not handled; reschedule it: */
				m->msg.t = x->now + x->retry_period;
				al_pq_sched_msg(x, m);
			} else {
				al_pq_recycle(x, m);
			}
		}
		m = al_pq_top(x);
	}
	/* update clock */
	x->now = until;		// why is it commented out?
}
void al_pq_advance(pq x, al_sec step, int defer) {
	al_pq_update(x, x->now + step, defer);
}


int al_pq_used(pq x) {
	return x->len;
}
