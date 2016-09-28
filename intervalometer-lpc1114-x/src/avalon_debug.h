//! @file
//! @author Jon Dubovsky, 28 Oct 2015
//! Debug utility functions like ASSERT, VERIFY, DEBUGLOG, etc.
#ifndef AVALON_DEBUG_H_
#define AVALON_DEBUG_H_

#ifdef NDEBUG
	inline void dummy_func() { }
#else
	//! Called when an assertion fires.
	void do_assert();
#endif

//! ASSERT is like C's assert; if the condition is false, a debug assertion
//! is fired; the condition is not even evaluated in release builds.
#ifdef NDEBUG
	#define ASSERT(_a) dummy_func()
#else
	#define ASSERT(_a) do { if (!(_a)) { do_assert(); } } while (0)
#endif

//! VERIFY is like ASSERT, but the condition is always run, even in release
//! builds.
#ifdef NDEBUG
	#define VERIFY(_a) _a
#else
	#define VERIFY(_a) do { if (!(_a)) { do_assert(); } } while (0)
#endif


//! Debug trace printing, as printf.
#ifdef NDEBUG
	#define TRACE0(_s) dummy_func()
	#define TRACE(_s, ...) dummy_func()
#else
	#define TRACE0(_s)
	#define TRACE(...) printf(__VA_ARGS__)
#endif


#endif /* AVALON_DEBUG_H_ */
