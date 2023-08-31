#include "gtest/gtest.h"
#include "token.h"
TEST(TestToken, test_token_1)
{
    Token tst = {Type::OBJECT_BEGIN, ""};
    Token otr;
    otr.token_type = Type::OBJECT_BEGIN;
    otr.value = "";
    EXPECT_EQ(tst.token_type, otr.token_type);
    otr.token_type = Type::ARRAY_BEGIN;
    EXPECT_NE(tst.token_type, otr.token_type);
}
