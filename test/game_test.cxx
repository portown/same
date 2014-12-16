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
        // 5 5 4 1 4
        // 5 3 4 3 4
        // 2 2 2 5 4
        // 2 2 4 5 4
        // 2 5 2 5 5
        same_ = std::make_shared<CSAME>(160, 160, 0, 0);
    }

    void teardown()
    {
    }
};

TEST(CSAMETestGroup, makeAreaTest)
{
    std::vector<unsigned char> const v {
        5, 5, 4, 1, 4,
        5, 3, 4, 3, 4,
        2, 2, 2, 5, 4,
        2, 2, 4, 5, 4,
        2, 5, 2, 5, 5,
    };

    for (unsigned short i = 0; i < 25; ++i)
    {
        BYTES_EQUAL(v[i], same_->getAt(i % 5, i / 5));
    }
}

TEST(CSAMETestGroup, selectsAtTest)
{
    CHECK(!same_->selectsAt(0, 0));
    same_->Select(POINT { 0, 0 });
    CHECK(same_->selectsAt(0, 0));
}
