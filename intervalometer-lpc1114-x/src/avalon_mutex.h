//! @file
//! @author Jon Dubovsky, 28 Oct 2015
//! Utility classes for mutexes, semaphores, etc.
#ifndef AVALON_MUTEX_H_
#define AVALON_MUTEX_H_

#include "avalon_time.h"

#include "FreeRTOS.h"	// required for task.h
#include "task.h"		// uses portTickType
#include "semphr.h"


class Mutex {
	xSemaphoreHandle h_;
	friend class SingleLock;
public:
	Mutex() :
		// Note: we're lazy and always create recursive mutexes so we don't have
		// to implement reference counting in the lock.
		h_(xSemaphoreCreateRecursiveMutex())
	{
	}
	~Mutex()
	{
		vSemaphoreDelete(h_);
	}
};

//! Magic wait value that means "forever"
extern const Milliseconds WAIT_FOREVER;

class SingleLock {
	Mutex & mutex_;		//!< mutex we're wrapping
	int locked_;		//!< lock count (0 == not yet locked)

	//! Forces an unlock regardless of locked_ count.
	void ForceUnlock_() {
		xSemaphoreGive(mutex_.h_);
	}
public:
	enum ECtorAction { DoNotClaimYet = 0, ClaimNow = 1 };

	//! Creates a lock around the mutex, either trying (indefinitely) to
	//! lock it now (eClaimNow) or deferring it until a later call to
	//! Lock (eDoNotClaimYet, the default).
	SingleLock(Mutex & m, ECtorAction action = DoNotClaimYet) :
		mutex_(m),
		locked_(0)
	{
		if (action == ClaimNow)
			Lock(WAIT_FOREVER);	// ignore return; infinite wait or we'd have to throw
	}
	//! Destroys this lock, releasing the mutex in the process.
	~SingleLock() {
		// This is the last lock (by definition, since we're about to destroy
		// locked_).  Make sure that we release the mutex!
		ForceUnlock_();
	}

	//! Tries to lock the mutex.  Safe to call even if locked.
	//! @returns true if successful lock on mutex, false if unable to take lock.
	bool Lock(Milliseconds howLongToTry = WAIT_FOREVER) {
		if (locked_) {
			// was already locked
			++locked_;
			return true;
		} else {
			// not yet locked; try the lock
			portTickType t = howLongToTry.rawMilliseconds() ? static_cast<portTickType>(howLongToTry) : portMAX_DELAY;
			if (xSemaphoreTake(mutex_.h_, t)) {
				++locked_;
				return true;
			} else {
				return false;
			}
		}
	}

	//! Safe to call even if not locked.
	void Unlock() {
		if (locked_) {
			if (--locked_ == 0) {
				ForceUnlock_();
			}
		}
	}
};

const Milliseconds WAIT_FOREVER{ 0 };

class Semaphore {
	xSemaphoreHandle h_;
public:
	Semaphore()
		// FreeRTOS 8 adds the ability to: h_(vSemaphoreCreateBinary())
	{
		vSemaphoreCreateBinary(h_);
	}
	~Semaphore()
	{
		vSemaphoreDelete(h_);
	}
	//! Raise (signal) the semaphore.
	void Raise() {
		xSemaphoreGive(h_);
	}
	//! Wait on the semaphore to be raised/signaled.
	//! @returns true if the wait succeeded (semaphore was raised) or false if the wait timed out.
	bool Wait(Milliseconds howLongToTry = WAIT_FOREVER) {
		portTickType t = howLongToTry.rawMilliseconds() ? static_cast<portTickType>(howLongToTry) : portMAX_DELAY;
		return xSemaphoreTake(h_, t);
	}
};


#endif /* AVALON_MUTEX_H_ */
