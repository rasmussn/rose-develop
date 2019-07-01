#ifndef FUNCTIONIDMAPPING_H
#define FUNCTIONIDMAPPING_H

#include <string>
#include <map>
#include <vector>
#include <set>

#include "RoseAst.h"
#include "SgNodeHelper.h"
#include "FunctionIdMapping.h"
#include "Flow.h"
#include "Labeler.h"


namespace CodeThorn {

class FunctionId;

/*! 
  * \author Simon Schroder (based on Markus Schordan's VariableIdMapping)
  * \date 2016.
 */
class FunctionIdMapping {
public:
  // Set of ids
  typedef std::set<FunctionId> FunctionIdSet;

  FunctionIdMapping();

  // the computation of the ROSE-based function-symbol mapping
  // creates a mapping of functionNames and its computed UniqueFunctionSymbol
  void computeFunctionSymbolMapping(SgProject* project);
  void computeFunctionSymbolMapping(/*const*/ Flow& ICFG, /*const*/ Labeler& labeler);

  SgFunctionDeclaration* getFunctionDeclaration(FunctionId funcFunctionId) const;




  void generateDot(std::string filename,SgNode* astRoot);

  FunctionIdSet getFunctionIdsOfFunctionDeclarations(std::set<SgFunctionDeclaration*> varDecls);
  FunctionIdSet getFunctionIdsOfAstSubTree(SgNode* node);

  // Looks up the given id in the mapping and returns the corresponding symbol
  //  Returns 0 if no mapping for the given id exists (e.g. if the id is invalid).
  SgSymbol* getSymbolFromFunctionId(FunctionId) const;

  // Looks up the given symbol in the mapping and returns the corresponding id.
  //  Returns an invalid id if no mapping for the given symbol exists.
  FunctionId getFunctionIdFromSymbol(SgSymbol*) const;

  // Looks up the symbol of the given declaration in the mapping and returns the corresponding id
  //  Returns an invalid id if no such mapping exists.
  FunctionId getFunctionIdFromDeclaration(const SgDeclarationStatement*) const;

  // Looks up the symbol of the given function reference in the mapping and returns the corresponding id
  //  Returns an invalid id if no such mapping exists.
  FunctionId getFunctionIdFromFunctionRef(SgFunctionRefExp* functionRef) const;
  FunctionId getFunctionIdFromFunctionRef(SgTemplateFunctionRefExp* functionRef) const;
  FunctionId getFunctionIdFromFunctionRef(SgMemberFunctionRefExp* functionRef) const;
  FunctionId getFunctionIdFromFunctionRef(SgTemplateMemberFunctionRefExp* functionRef) const;

  // Looks up the given id in the mapping and returns the type associated with the corresponding symbol
  SgFunctionType* getTypeFromFunctionId(FunctionId) const;

  bool isTemporaryFunctionId(FunctionId id) const;
  std::string getFunctionNameFromFunctionId(FunctionId id) const;
  std::string getUniqueShortNameFromFunctionId(FunctionId id) const;

  // Adds the given symbol to the mapping
  void registerNewSymbol(SgSymbol* sym);
  // Outputs the string representation of the mapping to the given stream
  void toStream(std::ostream& os) const;
  // Returns the string representation of the mapping
  std::string toString() const;

  // Returns a string representation of the given id
  std::string getStringFromFunctionId(FunctionId) const;

  void generateDot(std::string filename,SgNode* astRoot) const;

  // Returns all ids that are present in the mapping
  FunctionIdSet getFunctionIdSet() const;

  std::string getFunctionIdSetAsString(const FunctionIdSet& set);

  // Uses the mapping to return the ids for the given declarations
  FunctionIdSet determineFunctionIdsOfDeclarations(std::set<const SgDeclarationStatement*> decls) const;

  /* create a new unique symbol (should be used together with deleteUniqueSymbol)
       this is useful if additional (e.g. temporary) entities are introduced in an analysis
       this function does NOT insert this new symbol in any symbol table
   */
  FunctionId createUniqueTemporaryFunctionId(std::string name);

  // delete a unique symbol (should be used together with createUniqueSymbol)
  void deleteUniqueTemporaryFunctionId(FunctionId uniqueFunctionId);

  class UniqueTemporarySymbol : public SgSymbol {
  public:
    // Constructor: we only allow this single constructor
    UniqueTemporarySymbol(std::string name);
    // Destructor: default is sufficient

    // overrides inherited get_name (we do not use a declaration)
    SgName get_name() const;

    // implement pure member function:
    SgType* get_type() const;
  private:
    std::string _tmpName;
  };

  // Creates a new id from the given id code
  static FunctionId getFunctionIdFromCode(int);

  virtual ~FunctionIdMapping() {

  }

 protected:
  static void generateStmtSymbolDotEdge(std::ofstream&, SgNode* node, FunctionId id);
  static std::string generateDotSgSymbol(SgSymbol* sym);

  // Temporary mapping between ids and names:
  typedef std::string FunctionIdName;
  typedef std::pair<FunctionId,FunctionIdName> PairOfFunctionIdAndFunctionIdName;
  typedef std::set<PairOfFunctionIdAndFunctionIdName> TemporaryFunctionIdMapping;
  TemporaryFunctionIdMapping temporaryFunctionIdMapping;

  // The actual mapping (two data structures for fast mapping in both directions)
  // TODO: change SgSymbol* to const SgSymbol* because the mapping does not change
  //  symbols?
  std::vector<SgSymbol*> mappingFunctionIdToSymbol;
  std::map<SgSymbol*,size_t> mappingSymbolToFunctionId;

}; // end of class FunctionIdMapping

typedef FunctionIdMapping::FunctionIdSet FunctionIdSet;

/*!
  * \author Simon Schroder (based on Markus Schordan's VariableId)
  * \date 2012.
 */
class FunctionId {
  friend class FunctionIdMapping;
  friend bool operator<(FunctionId id1, FunctionId id2);
  friend bool operator==(FunctionId id1, FunctionId id2);
  friend class ConstraintSetHashFun;
 public:
  FunctionId();
  static const char * const idKindIndicator;
  // Returns the id code as a string with the given prefix
  std::string toString(const char* prefix = idKindIndicator) const;
  // Uses the given mapping to return a more detailed string representation
  std::string toString(const FunctionIdMapping& vid) const;
  // Returns the id code
  int getIdCode() const { return _id; }
  // we intentionally do not provide a constructor for int because this would clash
  // with overloaded functions that are using AbstractValue (which has an implicit
  // type conversion for int)
  void setIdCode(int id);
  // Returns whether this id is valid
  bool isValid() const { return _id >= 0; }

 private:
  // The id code
  int _id;
};

size_t hash_value(const FunctionId& id);

bool operator<(FunctionId id1, FunctionId id2);
bool operator==(FunctionId id1, FunctionId id2);
bool operator!=(FunctionId id1, FunctionId id2);
FunctionIdSet& operator+=(FunctionIdSet& s1, const FunctionIdSet& s2);

}

#endif
