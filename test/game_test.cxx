// game_test.cxx

#include <CppUTest/TestHarness.h>

#include <memory>
#include <vector>
#include <Windows.h>
#include "structs.hxx"


TEST_GROUP(CSAMETestGroup)
{
    std::shared_ptr<CSAME> same_;

    void setup()
    {
        same_ = std::make_shared<CSAME>(320, 320, 0, 0);
    }

    void teardown()
    {
    }
};

TEST(CSAMETestGroup, selectsAtTest)
{
    CHECK(!same_->selectsAt(0, 0));
    same_->Select(POINT { 0, 0 });
    CHECK(same_->selectsAt(0, 0));
}
