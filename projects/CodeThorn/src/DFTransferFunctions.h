#ifndef PATRANSFERFUNCTIONS_H
#define PATRANSFERFUNCTIONS_H

#include "Labeler.h"
#include "Lattice.h"
#include "Flow.h"
#include "PointerAnalysisInterface.h"
#include "ProgramAbstractionLayer.h"

namespace CodeThorn {

class DFTransferFunctions {
public:
  DFTransferFunctions();
  CodeThorn::Labeler* getLabeler() { return _programAbstractionLayer->getLabeler(); }
  VariableIdMapping* getVariableIdMapping() { return _programAbstractionLayer->getVariableIdMapping(); }
  virtual void setProgramAbstractionLayer(CodeThorn::ProgramAbstractionLayer* pal) {_programAbstractionLayer=pal; }
  // allow for some pointer analysis to be used directly
  void setPointerAnalysis(CodeThorn::PointerAnalysisInterface* pointerAnalysisInterface) { _pointerAnalysisInterface=pointerAnalysisInterface; }
  CodeThorn::PointerAnalysisInterface* getPointerAnalysisInterface() { return _pointerAnalysisInterface; }

  virtual void transfer(CodeThorn::Edge edge, Lattice& element);
  virtual void transferCondition(Edge edge, Lattice& element);

  virtual void transfer(CodeThorn::Label lab, Lattice& element);
  virtual void transferExpression(CodeThorn::Label label, SgExpression* expr, Lattice& element);
  virtual void transferEmptyStmt(CodeThorn::Label label, SgStatement* stmt, Lattice& element);
  virtual void transferDeclaration(CodeThorn::Label label, SgVariableDeclaration* decl, Lattice& element);
  virtual void transferReturnStmtExpr(CodeThorn::Label label, SgExpression* expr, Lattice& element);
  virtual void transferSwitchCase(CodeThorn::Label lab,SgStatement* condStmt, SgCaseOptionStmt* caseStmt,Lattice& pstate);
  virtual void transferSwitchDefault(CodeThorn::Label lab,SgStatement* condStmt, SgDefaultOptionStmt* defaultStmt,Lattice& pstate);
  virtual void transferFunctionCall(CodeThorn::Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, Lattice& element);
  virtual void transferExternalFunctionCall(CodeThorn::Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, Lattice& element);
  virtual void transferFunctionCallReturn(CodeThorn::Label lab, SgVarRefExp* lhsVar, SgFunctionCallExp* callExp, Lattice& element);
  virtual void transferFunctionCallReturn(CodeThorn::Label lab, CodeThorn::VariableId varId, SgFunctionCallExp* callExp, Lattice& element);
  virtual void transferFunctionEntry(CodeThorn::Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, Lattice& element);
  virtual void transferFunctionExit(CodeThorn::Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, Lattice& element);
  virtual ~DFTransferFunctions() {}
  virtual void addParameterPassingVariables();
  VariableId getParameterVariableId(int paramNr);
  VariableId getResultVariableId();
  bool isExternalFunctionCall(Label l);
  //protected:
 public:
  CodeThorn::PointerAnalysisInterface* _pointerAnalysisInterface;
  virtual void setSkipSelectedFunctionCalls(bool flag);
  virtual bool getSkipSelectedFunctionCalls();
 protected:
  bool _skipSelectedFunctionCalls=false;

 private:
  CodeThorn::ProgramAbstractionLayer* _programAbstractionLayer;
  VariableId parameter0VariableId;
  VariableId resultVariableId;
};

}

#endif
