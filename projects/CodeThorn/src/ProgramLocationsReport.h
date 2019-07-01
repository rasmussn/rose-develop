#ifndef PROGRAM_LOCATIONS_REPORT_H
#define PROGRAM_LOCATIONS_REPORT_H

#include <string>
#include "Labeler.h"

namespace CodeThorn {
  class ProgramLocationsReport {
  public:
    void writeResultFile(std::string fileName, CodeThorn::Labeler* labeler);
    CodeThorn::LabelSet definitiveLocations;
    CodeThorn::LabelSet potentialLocations;
    void recordDefinitiveLocation(CodeThorn::Label lab);
    void recordPotentialLocation(CodeThorn::Label lab);
    size_t numDefinitiveLocations();
    size_t numPotentialLocations();
  private:
    std::string programLocation(CodeThorn::Labeler* labeler, CodeThorn::Label lab);
    std::string sourceCodeAtProgramLocation(CodeThorn::Labeler* labeler, CodeThorn::Label lab);
  };
}
#endif
