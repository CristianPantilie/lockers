/* Lockers – Implementati propria biblioteca care sa ofere obiecte de sincronizare:
mutecsi, semafoare, rwlocks fara a folosi pe cele existente ın biblioteci precum pthread.
Pentru implementare puteti folosi doar primitive atomice precum compare-and-swap. */

#include <stdatomic.h>

typedef enum {false, true} bool;
typedef struct
{
	atomic_flag available;
	bool destroyed;
}mymtx_t;
typedef struct
{
	atomic_int value;
	bool destroyed;
}mysem_t;
typedef struct
{
	mymtx_t mtx1, mtx2;
	atomic_int counter;
	bool destroyed;
}myrwlock_t;

void mymtx_init(mymtx_t *mtx)
{
	mtx->available = (atomic_flag) ATOMIC_FLAG_INIT;
	mtx->destroyed = false;
}

void mymtx_destroy(mymtx_t *mtx)
{
	mtx->destroyed = true;
}

void mymtx_lock(mymtx_t *mtx)
{
	if (mtx->destroyed == false)
		while(atomic_flag_test_and_set(&mtx->available) == 1);   //spin until the lock is acquired
}

void mymtx_unlock(mymtx_t *mtx)
{
	if (mtx->destroyed == false)
		atomic_flag_clear(&mtx->available);
}

void mysem_init (mysem_t *sem, int value)
{
	sem->value = value;
	sem->destroyed = false;
}

void mysem_wait(mysem_t *sem)
{	
	if (sem->destroyed == false)
	{
		while(atomic_load(&sem->value) <= 0)//atomically obtain the value stored in an atomic object
			; //wait for the semaphore to become available
		atomic_fetch_sub(&sem->value, 1); //returns the value held previously by sem_value
	}
}

void mysem_post(mysem_t *sem)
{	
	if (sem->destroyed == false)
    		atomic_fetch_add(&sem->value, 1);
}

void mysem_destroy (mysem_t *sem)
{
    	sem->destroyed = true;
}

void myrwlock_init (myrwlock_t *rw)
{
	mymtx_init(&rw->mtx1);
	mymtx_init(&rw->mtx2);
	rw->counter = 0;
	rw->destroyed = false;
}

void myrwlock_readbegin (myrwlock_t *rw)
{
	if (rw->destroyed == false)
	{
		mymtx_lock(&rw->mtx1);	//lock mtx1
		atomic_fetch_add(&rw->counter, 1);
		if (atomic_load(&rw->counter) == 1)	//if there's a single reader, lock mtx2
			mymtx_lock(&rw->mtx2);
		mymtx_unlock(&rw->mtx1);
	}
}

void myrwlock_readend (myrwlock_t *rw)
{
	if (rw->destroyed == false)
	{
		mymtx_lock(&rw->mtx1);	//lock mtx1
		atomic_fetch_sub(&rw->counter, 1);
		if (atomic_load(&rw->counter) == 0)	//if mtx2 was locked in readbegin, unlock it
			mymtx_unlock(&rw->mtx2);
		mymtx_unlock(&rw->mtx1);
	}
}

void myrwlock_writebegin (myrwlock_t *rw)
{
	if (rw->destroyed == false)
		mymtx_lock(&rw->mtx2);
}

void myrwlock_writeend (myrwlock_t *rw)
{
	if (rw->destroyed == false)
		mymtx_unlock(&rw->mtx2);
}

void myrwlock_destroy (myrwlock_t *rw)
{
	mymtx_destroy(&rw->mtx1);
	mymtx_destroy(&rw->mtx2);
	rw->destroyed = true;
}


