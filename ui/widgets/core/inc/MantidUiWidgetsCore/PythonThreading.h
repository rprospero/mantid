#ifndef PYTHONTHREADING_H_
#define PYTHONTHREADING_H_

#include "MantidUiWidgetsCore/PythonSystemHeader.h" // this needs to go first
#include "MantidUiWidgetsCore/DllOption.h"

//------------------------------------------------------------------------------
// PythonGIL
//------------------------------------------------------------------------------
/**
 * Defines a structure for acquiring/releasing the Python GIL
 * using the RAII pattern. Modeled after QMutex
 *
 */
class MANTIDUI_WIDGETS_CORE_PUBLIC PythonGIL {
public:
  PythonGIL();

  void acquire();
  void release();

private:
  PythonGIL(const PythonGIL &);
  /// Current GIL state
  PyGILState_STATE m_state;
};

//------------------------------------------------------------------------------
// RecursiveGlobalInterpreterLock
//------------------------------------------------------------------------------

/**
 * A thread can call acquire multiple times and will only be unlocked
 * when a corresponding number of release calls are made.
 */
class MANTIDUI_WIDGETS_CORE_PUBLIC RecursivePythonGIL {
public:
  RecursivePythonGIL();

  void acquire();
  void release();

private:
  int m_count;
  PythonGIL m_lock;
};

//------------------------------------------------------------------------------
// ScopedInterpreterLock
//------------------------------------------------------------------------------

/**
  * Acquires a lock in the constructor and releases it in the destructor.
  * @tparam T Templated on the lock type
  */
template <typename T> class ScopedGIL {
public:
  ScopedGIL() : m_lock() { m_lock.acquire(); }
  ~ScopedGIL() { m_lock.release(); }

private:
  T m_lock;
};

/// Typedef for scoped lock
typedef ScopedGIL<PythonGIL> ScopedPythonGIL;
/// Typedef for scoped recursive lock
typedef ScopedGIL<RecursivePythonGIL> ScopedRecursivePythonGIL;

#endif /* PYTHONTHREADING_H_ */
