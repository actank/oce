#include "gtest/gtest.h"

#include "arch/adapter2.h"
#include "arch/configserver.h"

using namespace xce;

#if 0
template <typename PtrType, typename Cate, int Count>
struct AtomicArray {
  BOOST_STATIC_ASSERT(sizeof(AtomicWord) == sizeof(void *));
  BOOST_STATIC_ASSERT(sizeof(PtrType) == sizeof(AtomicWord));

  static volatile AtomicWord values[Count];
  // TODO: check sizeof(PtrType) == sizeof pointer
  // TODO: atomic access

  static PtrType AtomicGet(int index) {
    return reinterpret_cast<PtrType>(static_cast<AtomicWord>(
#ifdef POSIX
      subtle::Acquire_Load(&values[index])
#else
      // fuck msvc
      subtle::Acquire_Load((volatile Atomic32*)&values[index])
#endif
      ));
  }
  static PtrType AtomicCheckPut(int index, PtrType old_value, PtrType new_value) {
    return reinterpret_cast<PtrType>(static_cast<AtomicWord>(
#ifdef POSIX
      subtle::Acquire_CompareAndSwap(&values[index], (AtomicWord)old_value, (AtomicWord)new_value)
#else
      subtle::Acquire_CompareAndSwap((volatile Atomic32*)&values[index], (AtomicWord)old_value, (AtomicWord)new_value)
#endif
      ));
  }
};

template <typename PtrType, typename Cate, int Count>
volatile AtomicWord AtomicArray<PtrType, Cate, Count>::values[Count] = {0};
#endif

// TODO: 启动一个 Registry

TEST(Adapter2, Grid) {
  Grid grid3("CacheGrid/Locator:tcp -h 10.22.202.47 -p 12000");
  EXPECT_TRUE(grid3.c_);
  
  Grid grid2("CacheGrid", "10.22.202.47", 12000);
  EXPECT_TRUE(grid2.c_);

  Grid grid1("CacheGrid", "10.22.202.47:12000");
  EXPECT_TRUE(grid1.c_);
}

TEST(Adapter2, Create) {
  // CAUTION: 替换掉了缺省Grid
  Adapter::DefaultGrid() = Grid("ConfigGrid", "10.22.202.47:4061");

  // 使用缺省 grid
  ConfigServerPrx p0 = Adapter::Create<ConfigServerPrx>("config@config1");
  EXPECT_TRUE(p0);

  ConfigServerPrx p1 = Adapter::Create<ConfigServerPrx>("config@config", 1);
  EXPECT_TRUE(p1);

  // ConfigServerPrx p2 = Adapter::Create<ConfigServerPrx>();

  Adapter::test::WrapType::DefaultGrid() = Adapter::DefaultGrid();

  // 使用 grid = xiaonei
  ConfigServerPrx xp = Adapter::test::Create<ConfigServerPrx>("config@config1");
  EXPECT_TRUE(xp);
}

TEST(Adapter2, Cached) {
  ConfigServerPrx cp1 = Adapter::Cached<ConfigServerPrx>("config@config", 1);
  EXPECT_TRUE(cp1);
  // ConfigServerPrx cp1_10 = Adapter::Cached<ConfigServerPrx>("config@config", 1);
  // EXPECT_TRUE(cp1_10);
}
