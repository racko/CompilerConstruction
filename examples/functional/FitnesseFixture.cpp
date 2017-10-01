#include "SlimList.h"
#include "Fixtures.h"
#include "Functional.h"
#include "FunctionalAttribute.h"
#include <string>
#include <sstream>

#ifndef CPP_COMPILING
extern "C" {
#endif
struct Functional
{
    std::string text;
    std::string result;
};

void* Functional_Create(StatementExecutor* errorHandler, SlimList* args)
{
    return new Functional();
}

void Functional_Destroy(void* void_self)
{
    delete (Functional*)void_self;
}

static char* setText(void* void_self, SlimList* args) {
    Functional* self = (Functional*)void_self;
    self->text = SlimList_GetStringAt(args, 0);
    self->text += '\xFF'; /// @FIXME
    return self->result.data();
}

static char* Evaluate(void* void_self, SlimList* args) {
    Functional* self = (Functional*)void_self;
    auto result = run(self->text.c_str());
    std::stringstream s;
    s << *result;
    self->result = s.str();
    return self->result.data();
}

SLIM_CREATE_FIXTURE(Functional)
    SLIM_FUNCTION(setText)
    SLIM_FUNCTION(Evaluate)
    SLIM_END

SLIM_FIXTURES
    SLIM_FIXTURE(Functional)
SLIM_END

#ifndef CPP_COMPILING
}
#endif
