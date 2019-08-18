#include "CSlim/Fixtures.h"          // for SLIM_END, SLIM_FUNCTION, SLIM_CREAT...
#include "CSlim/SlimList.h"          // for SlimList, SlimList_GetStringAt
#include "CSlim/StatementExecutor.h" // for StatementExecutor
#include "Functional.h"              // for run
#include "FunctionalAttribute.h"     // for operator<<
#include <sstream>                   // for stringstream, ostream
#include <string>                    // for string

#ifndef CPP_COMPILING
extern "C" {
#endif
struct FunctionalFixture {
    std::string text;
    std::string result;
};

void* FunctionalFixture_Create(StatementExecutor* /*errorHandler*/, SlimList* /*args*/) {
    return new FunctionalFixture();
}

void FunctionalFixture_Destroy(void* void_self) { delete reinterpret_cast<FunctionalFixture*>(void_self); }

static char* setText(void* void_self, SlimList* args) {
    FunctionalFixture* self = reinterpret_cast<FunctionalFixture*>(void_self);
    self->text = SlimList_GetStringAt(args, 0);
    self->text += '\xFF'; /// @FIXME
    return self->result.data();
}

static char* Evaluate(void* void_self, SlimList* /*args*/) {
    FunctionalFixture* self = reinterpret_cast<FunctionalFixture*>(void_self);
    auto result = Functional::run(self->text.c_str());
    std::stringstream s;
    s << *result;
    self->result = s.str();
    return self->result.data();
}

SLIM_CREATE_FIXTURE(FunctionalFixture)
SLIM_FUNCTION(setText)
SLIM_FUNCTION(Evaluate)
SLIM_END

SLIM_FIXTURES
SLIM_FIXTURE(FunctionalFixture)
SLIM_END

#ifndef CPP_COMPILING
}
#endif
