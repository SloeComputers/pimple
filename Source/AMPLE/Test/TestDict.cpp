
#include "STB/Test.h"

#include "../Dict.h"

TEST(Dict, basic)
{
   AMPLE::Dict<unsigned,128> dict{};

   EXPECT_EQ(0, dict.level());

   EXPECT_TRUE(dict.add("fred", 10));

   EXPECT_EQ(4, dict.level());

   unsigned value{0};
   const char* s;

   s = dict.lookup("fredjames", value);
   EXPECT_STREQ("james", s);
   EXPECT_EQ(10, value);

   s = dict.lookup("georgefred", value);
   EXPECT_STREQ("georgefred", s);

   dict.remove("fred");
   s = dict.lookup("fredjames", value);
   EXPECT_STREQ("fredjames", s);

   EXPECT_EQ(0, dict.level());
}

TEST(Dict, common_prefix)
{
   AMPLE::Dict<unsigned,128> dict{};

   EXPECT_TRUE(dict.add("fred", 10));
   EXPECT_TRUE(dict.add("freddie", 12));

   unsigned value{0};
   const char* s;

   // Match fred
   s = dict.lookup("fredjames", value);
   EXPECT_STREQ("james", s);
   EXPECT_EQ(10, value);

   // Match freddie
   s = dict.lookup("freddiejames", value);
   EXPECT_STREQ("james", s);
   EXPECT_EQ(12, value);

   // Remove fred
   dict.remove("fred");

   // Fail to match fred
   s = dict.lookup("fredjames", value);
   EXPECT_STREQ("fredjames", s);

   // Match freddie
   s = dict.lookup("freddiejames", value);
   EXPECT_STREQ("james", s);
   EXPECT_EQ(12, value);

   // Fail to match freddie
   dict.remove("freddie");
   s = dict.lookup("freddiejames", value);
   EXPECT_STREQ("freddiejames", s);

   EXPECT_EQ(0, dict.level());
}

TEST(Dict, one_or_more)
{
   AMPLE::Dict<unsigned,128> dict{};

   EXPECT_TRUE(dict.add(" +", 10));
   EXPECT_TRUE(dict.add("ABC", 11));

   unsigned value{0};
   const char* s;

   s = dict.lookup("ABC    ABC", value);
   EXPECT_STREQ("    ABC", s);
   EXPECT_EQ(11, value);

   s = dict.lookup(s, value);
   EXPECT_STREQ("ABC", s);
   EXPECT_EQ(10, value);

   s = dict.lookup(s, value);
   EXPECT_STREQ("", s);
   EXPECT_EQ(11, value);

   s = dict.lookup(s, value);
   EXPECT_STREQ("", s);
}

TEST(Dict, zero_or_more)
{
   AMPLE::Dict<unsigned,128> dict{};

   EXPECT_TRUE(dict.add("AB*", 10));

   unsigned value{0};
   const char* s;

   s = dict.lookup("B", value);
   EXPECT_STREQ("B", s);

   s = dict.lookup("A", value);
   EXPECT_STREQ("", s);
   EXPECT_EQ(10, value);

   s = dict.lookup("AB", value);
   EXPECT_STREQ("", s);
   EXPECT_EQ(10, value);

   s = dict.lookup("ABB", value);
   EXPECT_STREQ("", s);
   EXPECT_EQ(10, value);

   s = dict.lookup("ABBB", value);
   EXPECT_STREQ("", s);
   EXPECT_EQ(10, value);
}

TEST(Dict, string)
{
   AMPLE::Dict<unsigned,128> dict{};

   EXPECT_TRUE(dict.add("\".*\"", 10));

   dict.print();

   unsigned value{0};
   const char* s;

   s = dict.lookup("\"hello\"ABC", value);
   EXPECT_STREQ("ABC", s);
   EXPECT_EQ(10, value);

   s = dict.lookup("\"\"ABC", value);
   EXPECT_STREQ("ABC", s);
   EXPECT_EQ(10, value);

   s = dict.lookup("\"\"", value);
   EXPECT_STREQ("", s);
   EXPECT_EQ(10, value);
}

TEST(Dict, number)
{
   AMPLE::Dict<unsigned,128> dict{};

   EXPECT_TRUE(dict.add("[0-9]+", 10));
   EXPECT_TRUE(dict.add("-[0-9]+", 11));
   EXPECT_TRUE(dict.add("-", 12));

   unsigned value{0};
   const char* s;

   s = dict.lookup("1", value);
   EXPECT_STREQ("", s);
   EXPECT_EQ(10, value);

   s = dict.lookup("12", value);
   EXPECT_STREQ("", s);
   EXPECT_EQ(10, value);

   s = dict.lookup("-12", value);
   EXPECT_STREQ("", s);
   EXPECT_EQ(11, value);

   s = dict.lookup("-A", value);
   EXPECT_STREQ("A", s);
   EXPECT_EQ(12, value);
}
