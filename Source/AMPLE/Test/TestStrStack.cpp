
#include "STB/Test.h"

#include "../StrStack.h"

TEST(StrStack, basic)
{
   AMPLE::StrStack<512> stack{};

   EXPECT_TRUE(stack.empty());
   EXPECT_STREQ(nullptr, stack.peek());

   stack.push("hello");
   EXPECT_FALSE(stack.empty());
   EXPECT_STREQ("hello", stack.peek());

   stack.push("world");
   EXPECT_STREQ("world", stack.peek());

   EXPECT_STREQ("world", stack.pop());
   EXPECT_STREQ("hello", stack.peek());
   EXPECT_STREQ("hello", stack.pop());

   EXPECT_TRUE(stack.empty());
}

TEST(StrStack, construct)
{
   AMPLE::StrStack<512> stack{"cabbage"};

   EXPECT_FALSE(stack.empty());
   EXPECT_STREQ("cabbage", stack.peek());
   EXPECT_STREQ("cabbage", stack.pop());

   EXPECT_TRUE(stack.empty());
}

TEST(StrStack, add)
{
   AMPLE::StrStack<512> stack{"pinky"};

   stack.add("&");
   stack.add("perky");

   EXPECT_STREQ("pinky&perky", stack.peek());
}
