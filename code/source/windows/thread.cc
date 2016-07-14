#include "cmx_thread.h"
#define WIN32_LEAN_AND_MEAN
#include<windows.h>
#include<process.h>
#include<cstdio>
#include<cstdlib>
#include<cstring>

namespace cmx {
	namespace thread {

		void create_thread(cmx_thread *t, void *param, thread_func callback) {
			unsigned int id;
			HANDLE hThread;
			t->param = (void *)_beginthreadex(NULL, 0, callback, 0, 0, &id);
			t->id = id;
		}

		void kill_thread(cmx_thread *t) {
			TerminateThread((HANDLE)t->param, 0x0);
			CloseHandle((HANDLE)t->param);
		}

		int exit_thread(int code) {
			_endthreadex(code);
			return code;
		}

		void join_thread(cmx_thread *t) {
			wait_thread(t);
		}

		void wait_thread(cmx_thread *t) {
			WaitForSingleObject((HANDLE)t->param, INFINITE);
			CloseHandle((HANDLE)t->param);
		}

		void create_mutex(mutex *m) {
			LPCRITICAL_SECTION lp = (LPCRITICAL_SECTION)calloc(1, sizeof(CRITICAL_SECTION));
			InitializeCriticalSection(lp);
			m->param = (void*)lp;
		}

		void lock_mutex(mutex *m) {
			EnterCriticalSection((LPCRITICAL_SECTION)m->param);
		}

		void unlock_mutex(mutex *m) {
			LeaveCriticalSection((LPCRITICAL_SECTION)m->param);
		}

		void destroy_mutex(mutex *m) {
			if(m->param == 0) return;
			DeleteCriticalSection((LPCRITICAL_SECTION)m->param);
			free(m->param);
		}
	}

}

