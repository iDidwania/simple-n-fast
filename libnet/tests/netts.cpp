#include "test.h"
#include "testmain.h"
#include "sockattr.h"
#include "key.h"
#include "certificate.h"
#include "sctx.h"

namespace snf {
namespace tf {

test *test_list[] = {
	DBG_NEW sock_attr(),
	DBG_NEW priv_key(),
	DBG_NEW certificate(),
	DBG_NEW sctx(),
	0
};

} // namespace tf
} // namespace snf
