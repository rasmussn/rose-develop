#ifndef ROSE_RSIM_Simulator_H
#define ROSE_RSIM_Simulator_H

/* Being the top-level header file for ROSE's simulator support, this header should include everything one would normally need
 * for the basic simulator.  However, additional architecture- and OS-specific headers may be needed to actually instantiate a
 * simulator. */
#include "threadSupport.h"

#include "RSIM_Common.h"
#include "RSIM_Process.h"
#include "RSIM_SemanticPolicy.h"
#include "RSIM_Thread.h"
#include "RSIM_Templates.h"

#include <signal.h>

/** Top level class for simulating programs.
 *
 *  An RSIM_Simulator instance represents a single process that associated with a specified executable specimen.  The specimen
 *  process shares certain resources with the simulator process, such as process ID, signal mask, etc.  Therefore, only one
 *  RSIM_Simulator should be active at a time (see activate()).
 *
 *  The simulator points to a single simulated process (RSIM_Process), which in turn points to one or more simulated threads
 *  (RSIM_Thread). Each simulated thread runs in a real thread, and just as a simulated process shares certain state with the
 *  simulator process, the simulated threads share certain state with the real threads.
 *
 *  Example of a very simple simulator.  The simulator is configured from the command-line and the name and arguments of the
 *  executable specimen are supplied:
 *
 *  @code
 *  #include <rose.h>
 *  #include <RSIM_Simulator.h>
 *  int main(int argc, char *argv[], char *envp[]) {
 *      RSIM_Simulator s(argc, argv, envp);   // Create a simulated process and its initial thread
 *      s.activate();                         // Allow other real processes to signal this one
 *      s.main_loop();                        // Simulate until exit
 *      s.deactivate();                       // Restore original signal handlers
 *      return 0;
 *  }
 *  @endcode
 */
class RSIM_Simulator {
public:
    /** Construct a new simulator object by initializing it from the specified command-line arguments and creating the main
     *  process object and its first thread.  The argv[0] is ignored; the remainder are zero or more simulator switches followed
     *  by the name of the executable specimen, followed by zero or more specimen arguments. */
    RSIM_Simulator(int argc, char **argv, char **envp)
        : trace_flags(0), core_flags(CORE_ELF), dump_at(0), btrace_file(NULL), active(0), process(NULL),
          entry_va(0), seen_entry_va(false) {
        ctor(argc, argv, envp);
    }

    /** Activate a new simulator. Only one simulator should be active at any given time since activating a simulator causes the
     *  that simulator to trap all signals.  It is not necessary to activate a simulator if the simulator does not expect to
     *  receive signals from other processes.  Activation is recursive and should be matched by an equal number of
     *  deactivate() calls.
     *
     *  Thread safety:  This method is thread safe; it can be invoked on the same object from multiple threads. */
    void activate();

    /** A simulator should be deactivated when it is no longer needed.  Activation is recursive and should be matched by an
     *  equal number of deactivate() calls.  The outer-most deactivation causes signal handlers to be restored to the state
     *  they were in when the simulator was first activated (other deactivations are no-ops).
     *
     *  Thread safety:  This method is thread safe; it can be invoked on the same object from multiple threads. */
    void deactivate();

    /** Returns true if this simulator is active.
     *
     *  Thread safety: This method is thread safe; it can be invoked on the same object from multiple threads. */
    bool is_active() const;

    /** Returns a pointer to the currently active simulator, if any.
     *
     *  Thread safety: This method is thread safe; it can be invoked on the same object from multiple threads. */
    static RSIM_Simulator *which_active();

    /** Returns a pointer to the main process object. */
    RSIM_Process *get_process() const {
        return process;
    }

    /** Enter the main simulation loop. This loop will return when... */
    void main_loop();

private:
    /** Helper for object construction. */
    void ctor(int argc, char **argv, char **envp);

    /** Initialize the simulator object from a command line.  The ENVP is used only for the --showauxv switch and may be a null
     *  pointer.  This should normally be called only by constructors, before the main process is created.  The return value is
     *  the number of switch arguments that were processed.  In other words, argv[parse_cmdline(...)] is probably the name of
     *  the executable to load and simulate, and the subsequent arguments are the arguments to pass to that executable.
     *
     *  Thread safety: This method is thread safe provided it is not invoked on the same object concurrently. Note, however,
     *  that it may call functions registered with atexit(). */
    int parse_cmdline(int argc, char **argv,  char **envp=NULL);

    /** Create the main process object for the simulator. The process is initialized from properties already initialized in the
     *  simulator.  This method is normally called from constructors.  A single simulator simulates a single process that
     *  shares certain state (such as PID) with the real process running the simulator. */
    RSIM_Process *create_process();

    /** Signal handler installed by activate(). */
    static void signal_handler(int signo);
    

private:
    /* Configuration variables */
    std::string trace_file_name;        /**< Name pattern for debug trace output, or empty to disable. */
    unsigned trace_flags;               /**< What things should be traced for debugging? (See TraceFlags enum) */
    unsigned core_flags;                /**< Kinds of core dumps to produce. (See CoreStyle enum) */
    rose_addr_t dump_at;                /**< Address at which to produce a core dump, or zero to disable. */
    std::string dump_name;              /**< Optional name of file for core dump when "dump_at" is hit. */
    std::string interp_name;            /**< Name of command-line specified interpreter for dynamic linking. */
    std::vector<std::string> vdso_paths;/**< Files and/or directories to search for a virtual dynamic shared library. */
    FILE *btrace_file;                  /**< Name for binary trace file, which will log info about process execution. */

    /* Simulator activation/deactivation */
    unsigned active;                    /**< Levels of activation. See activate(). */
    static RSIM_Simulator *active_sim;  /**< Points to the currently active simulator (there can be at most one). */
    int signal_installed[_NSIG];        /**< Status (N>=0 implies installed; N<0 implies error where N==-errno). */
    struct sigaction signal_restore[_NSIG];/**< When active and installed, this is the signal action to restore to deactivate. */

    /* Other */
    RSIM_Process *process;              /**< Main process. */
    rose_addr_t entry_va;               /**< Main entry address. */
    bool seen_entry_va;                 /**< Set to true when any thread has executed the entry address. */
    static RTS_rwlock_t class_rwlock;   /**< For methods that access class variables. */
    
};



#endif /* ROSE_RSIM_Simulator_H */
