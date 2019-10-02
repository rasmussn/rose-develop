#ifndef IO_ANALYZER_H
#define IO_ANALYZER_H

#include "Analyzer.h"

namespace CodeThorn {
  /*! 
  * \author Marc Jasper
  * \date 2017.
  * \brief Analyzer with additional input/output-related functionality
 */
  class IOAnalyzer : public Analyzer {

    friend class Solver8;
    friend class Solver10;

  public:
    IOAnalyzer();

  protected:
    static Sawyer::Message::Facility logger;

  public:
    static void initDiagnostics();

    // overwritten or extended analyzer functions
    virtual void initializeSolver(std::string functionToStartAt,SgNode* root, bool oneFunctionOnly);
    void resetAnalysis();

    void setModeLTLDriven(bool ltlDriven) { transitionGraph.setModeLTLDriven(ltlDriven); }
    bool getModeLTLDriven() { return transitionGraph.getModeLTLDriven(); }
    // only used in LTL-driven mode
    void setSpotConnection(SpotConnection* connection) { _spotConnection = connection; }
    void setStartEState(const EState* estate);
    // only used to initialize solver 10
    void setStartPState(PState startPState) { _startPState=startPState; }

    // Extracts input sequences leading to each discovered failing assertion where discovered for the first time.
    // Stores results in PropertyValueTable "reachabilityResults".
    void extractRersIOAssertionTraces();
    // erases transitions that lead directly from one output state to another output state
    void removeOutputOutputTransitions();
    // erases transitions that lead directly from one input state to another input state
    void removeInputInputTransitions();
    // cuts off all paths in the transition graph that lead to leaves 
    // (recursively until only paths of infinite length remain)
    void pruneLeaves();
    // reduction based on all states, works also for disconnected STGs (used by CEGPRA)
    void reduceToObservableBehavior();

    //solver 8 becomes the active solver used by the analyzer. Deletion of previous data iff "resetAnalyzerData" is set to true.
    void setAnalyzerToSolver8(EState* startEState, bool resetAnalyzerData);
    void continueAnalysisFrom(EState* newStartEState);

    const EState* getEstateBeforeMissingInput() {return _estateBeforeMissingInput;}
    const EState* getLatestErrorEState() {return _latestErrorEState;}
    std::set<int> getInputVarValues() { return _inputVarValues; }

    // first: list of new states (worklist), second: set of found existing states
    typedef pair<EStateWorkList,std::set<const EState*> > SubSolverResultType;
    SubSolverResultType subSolver(const EState* currentEStatePtr);

  private:
    bool isLTLRelevantEState(const EState* estate);
    // adds a string representation of the input (/output) path from start state to assertEState to reachabilityResults.
    void addCounterexample(int assertCode, const EState* assertEState);

    // solver 8
    const EState* _estateBeforeMissingInput = nullptr;
    const EState* _latestOutputEState  = nullptr;
    const EState* _latestErrorEState  = nullptr;

    // only used in LTL-driven mode
    SpotConnection* _spotConnection = nullptr;
    size_t _prevStateSetSizeDisplay = 0;
    size_t _prevStateSetSizeResource = 0;

    // solver 10
    PState _startPState;

  }; // end of class IOAnalyzer
} // end of namespace CodeThorn

#endif
