#pragma once
#include "xxlib_utils.h"
#include "xxlib_fp.h"
#include "assert.h"
#include <memory>
#include <functional>
#include <tuple>
#include <vector>

namespace xxlib
{
    // 内存优化原则, 亲和 cpu cache
    // 左下 右上 index, 宽高 坐标   与  其他属性 分离, 数组列存储, 内存相对集中( 这种集中可能对于部分热点操作有利, 有待观察 )
    // 另一种考虑是实际使用中, 创建多份 AbcdGrid, 从而不必使用 groupId, layerMask 来分类, 进一步收紧数据结构, 减少无畏的 if
    // 在这种情况下, 或许将 item1, 2 合成一个, 效果也不错


    // item 中心点坐标
    struct AbcdPoint
    {
        int x, y;
    };

    // item 宽高
    struct AbcdSize
    {
        int w, h;
    };

    // 2d cell 坐标
    struct AbcdCellIndex2d
    {
        int                         rowIndex, columnIndex;
    };

    // cell 坐标 + item node index
    struct AbcdCellIndex_ItemIndex
    {
        AbcdCellIndex2d             cellIndex2d;                    // 为了快速过滤节点范围
        int                         nodeIndex;                      // 为了快速移除节点
    };

    // Item 分成 1, 2 两部分存储. 这是第 1 部分, 方便集中 Update() 性能提升
    struct AbcdItem1
    {
        int                         ci_left, ri_bottom;             // Index / 上次 Update 操作结果之 左下 右上 位于的 cell
        int                         ci_right, ri_top;
        AbcdSize                    halfSize;                       // 宽高的一半
        AbcdPoint                   centerPos;                      // 中心点坐标
    };

    // Item 分成 1, 2 两部分存储. 这是第 2 部分, 用于后期取其他属性
    template<int MaxCrossCellCount>
    struct AbcdItem2
    {
        bool                        handled;                        // 用于单次集合操作去重

        int                         groupId;                        // 0 组号将和其他组号发生碰撞，相同组号不发生
        unsigned int                layerMask;                      // maskA & maskB 不为 0 的发生碰撞. 默认值为 uint.MAX_VALUE

        int                         crossCellsCount;
        AbcdCellIndex_ItemIndex     crossCells[MaxCrossCellCount];  // 横跨/占据 了哪些格子 以及位于这些格子中的节点指针( for 快删 )

        void                       *userData;                       // 对外接口用做数据 bind
    };


    // 位于 Cell 中的 item index 链表
    struct AbcdItemNode
    {
        int                         next, prev;                     // 链表元素
        int                         itemIndex;
    };

    // Grid 管理器( 含 items, nodes, cells )
    // item 与 cell 是多对多的关系, 通过 node 来 bind( node 主要是为高速增删服务 )
    template<int rowCount, int columnCount, int cellWidth, int cellHeight, int MaxCrossCellCount>
    struct AbcdGrid
    {
        typedef AbcdItem1                       IT1;
        typedef AbcdItem2<MaxCrossCellCount>    IT2;
        typedef AbcdItemNode                    NT;

        FlatPool<IT1, IT2>                      items;              // item 池
        FlatPool<NT>                            nodes;              // item node 池
        int                                    *cells;              // 长度为 rowCount * columnCount, 所有格子 -- item节点 链表头映射. -1 为空

        int GetRowCount() const
        {
            return rowCount;
        }
        int GetColumnCount() const
        {
            return columnCount;
        }
        int GetCellWidth() const
        {
            return cellWidth;
        }
        int GetCellHeight() const
        {
            return cellHeight;
        }

        int GetCellCount() const
        {
            return rowCount * columnCount;
        }

        AbcdGrid()
            : AbcdGrid(512)
        {
        }

        explicit AbcdGrid(int capacity)
            : items(capacity)
            , nodes(capacity * MaxCrossCellCount)
        {
            cells = (int*)malloc(GetCellCount() * sizeof(int));
            memset(cells, -1, GetCellCount() * sizeof(int));                        // -1 代表 "空"
        }

        // 封装 items 成员的一些函数, 方便用

        int Alloc()
        {
            return items.Alloc();
        }

        void Free(int itemIndex)
        {
            items.Free(itemIndex);
        }

        IT1& Ref(int itemIndex)
        {
            return items.template Ref<0>(itemIndex);
        }

        IT2& Ref1(int itemIndex)
        {
            return items.template Ref<1>(itemIndex);
        }

        // 格子坐标 2 维转 1 维: 行idx * 列数 + 列idx
        static int GetCellIndex(int rowIndex, int columnIndex)
        {
            return rowIndex * columnCount + columnIndex;
        }
        static int GetCellIndex(AbcdCellIndex2d const &ci2d)
        {
            return ci2d.rowIndex * columnCount + ci2d.columnIndex;
        }

        // 初始化 item
        void Init(int itemIndex, AbcdSize const& halfSize, AbcdPoint const& centerPos, int groupId = 0, unsigned int layerMask = 0xFFFFFFFFu, void* userData = nullptr)
        {
            auto& item_part1 = Ref(itemIndex);
            item_part1.ci_left = item_part1.ci_right = item_part1.ri_bottom = item_part1.ri_top = -1;
            item_part1.halfSize = halfSize;
            item_part1.centerPos = centerPos;

            auto& item_part2 = Ref1(itemIndex);
            item_part2.handled = false;
            item_part2.groupId = groupId;
            item_part2.layerMask = layerMask;
            item_part2.crossCellsCount = 0;
            item_part2.userData = userData;
        }



    protected:

        // 内部函数, 向 指定坐标 cell 添加一个 itemNode( 同时填充 item 内的 crossCells )
        void AddNode(int ri, int ci, int itemIndex, IT2 &item_part2)
        {
            // 定位到 cell
            auto cellIndex = GetCellIndex(ri, ci);                  // cell 2维转1维

                                                                    // 往 cell 链入 item节点
            int index = nodes.Alloc();                              // 请求一个新节点
            if (cells[cellIndex] >= 0)                              // 如果 cell 中已存在链表
            {
                nodes.Ref<0>(cells[cellIndex]).prev = index;        // 令链表 prev 指向新节点
            }
            nodes.Ref<0>(index).prev = -1;                          // 令新节点的 prev 为空
            nodes.Ref<0>(index).next = cells[cellIndex];            // 令新节点的 next 指向已存在链表
            nodes.Ref<0>(index).itemIndex = itemIndex;              // 设新节点的 itemIndex
            cells[cellIndex] = index;                               // 令 cell 存新链表头

                                                                    // 往 当前 item 的 crossCells 加 cell ci ri,  node_item_index
            assert(item_part2.crossCellsCount < MaxCrossCellCount);         // 不应该超过 item 占 cell 个数限制
            item_part2.crossCells[item_part2.crossCellsCount] = { { ri, ci }, index };
            ++item_part2.crossCellsCount;
        }

        // 内部函数, 根据 item_part2.croossCells[ i ] 移除节点
        void RemoveNode(IT2 &item_part2, int i)
        {
            auto& cc = item_part2.crossCells[i];
            if (nodes.Ref<0>(cc.nodeIndex).prev >= 0)               // 如果本节点的上个节点不为空, 则令上个节点的 next 为本节点的 next
            {
                nodes.Ref<0>(nodes.Ref<0>(cc.nodeIndex).prev).next = nodes.Ref<0>(cc.nodeIndex).next;
            }
            else                                                    // 本节点的 prev 为空意味着它位于 cell
            {
                cells[GetCellIndex(item_part2.crossCells[i].cellIndex2d)] = nodes.Ref<0>(cc.nodeIndex).next;    // 令 cell 等于本节点的 next
            }
            if (nodes.Ref<0>(cc.nodeIndex).next >= 0)               // 如果本节点的下个节点不为空, 则令下个节点的 prev 为本节点的 prev
            {
                nodes.Ref<0>(nodes.Ref<0>(cc.nodeIndex).next).prev = nodes.Ref<0>(cc.nodeIndex).prev;
            }
            nodes.Free(cc.nodeIndex);
        }


    public:

        // 创建 item 的格子索引( 每元素只能 Index 一次 )
        void Index(int itemIndex)
        {
            auto& item_part1 = Ref(itemIndex);
            auto& item_part2 = Ref1(itemIndex);
            assert(item_part1.ci_left == -1);                       // ensure Init

            // 计算并得到 左下, 右上的两个格子的 index( 坐标 左下为 0, 向右上增长 )

            // 1: bottom left
            int ci_left = (item_part1.centerPos.x - item_part1.halfSize.w) / cellWidth;
            int ri_bottom = (item_part1.centerPos.y - item_part1.halfSize.h) / cellHeight;

            // 2: top right
            int ci_right = (item_part1.centerPos.x + item_part1.halfSize.w) / cellWidth;
            int ri_top = (item_part1.centerPos.y + item_part1.halfSize.h) / cellHeight;

            assert(ci_left >= 0 && ci_left < columnCount && ri_bottom >= 0 && ri_bottom < rowCount
                && ci_right >= 0 && ci_right < columnCount && ri_top >= 0 && ri_top < rowCount);

            UnIndex(itemIndex);     // 防御性编程, 避免反复 Index 出错

            for (int ri = ri_bottom; ri <= ri_top; ++ri)
            {
                for (int ci = ci_left; ci <= ci_right; ++ci)
                {
                    AddNode(ri, ci, itemIndex, item_part2);
                }
            }

            item_part1.ci_left = ci_left;
            item_part1.ci_right = ci_right;
            item_part1.ri_bottom = ri_bottom;
            item_part1.ri_top = ri_top;
        }


        // 更新 item 的格子索引
        void Update(int itemIndex)
        {
            auto& item_part1 = Ref(itemIndex);

            // 这几行计算同 Index
            int ci_left = (item_part1.centerPos.x - item_part1.halfSize.w) / cellWidth;
            int ri_bottom = (item_part1.centerPos.y - item_part1.halfSize.h) / cellHeight;
            int ci_right = (item_part1.centerPos.x + item_part1.halfSize.w) / cellWidth;
            int ri_top = (item_part1.centerPos.y + item_part1.halfSize.h) / cellHeight;
            assert(ci_left >= 0 && ci_left < columnCount && ri_bottom >= 0 && ri_bottom < rowCount
                && ci_right >= 0 && ci_right < columnCount && ri_top >= 0 && ri_top < rowCount);

            // 如果所占格子没有变化, 直接短路退出
            if (item_part1.ci_left == ci_left && item_part1.ci_right == ci_right
                && item_part1.ci_right == ci_right && item_part1.ri_top == ri_top) return;

            auto& item_part2 = Ref1(itemIndex);

            // 同步 item 所占格子: 移除掉未占格子( 同时从格子链表中移除自己 )
            for (int i = item_part2.crossCellsCount - 1; i >= 0; --i)         // 倒循环, 交换删除法
            {
                auto& cc = item_part2.crossCells[i];
                if (cc.cellIndex2d.rowIndex < ri_bottom || cc.cellIndex2d.rowIndex > ri_top
                    || cc.cellIndex2d.columnIndex < ci_left || cc.cellIndex2d.columnIndex > ci_right)
                {
                    RemoveNode(item_part2, i);
                    // 交换删除法( 并非真交换. 只是把最后一个元素的值覆盖到当下即可. 因为内存不需要释放 )
                    if (i < item_part2.crossCellsCount - 1)     // 如果 i 不是末尾, 则取末尾值覆盖到当前
                    {
                        item_part2.crossCells[i] = item_part2.crossCells[item_part2.crossCellsCount - 1];
                    }
                    --item_part2.crossCellsCount;               // 交换后总长减 1
                    assert(item_part2.crossCellsCount >= 0);
                }
            }

            for (int ri = ri_bottom; ri <= ri_top; ++ri)
            {
                for (int ci = ci_left; ci <= ci_right; ++ci)
                {
                    if (ri < item_part1.ri_bottom || ri > item_part1.ri_top || ci < item_part1.ci_left || ci > item_part1.ci_right)
                    {
                        AddNode(ri, ci, itemIndex, item_part2);
                    }
                }
            }

            item_part1.ci_left = ci_left;
            item_part1.ci_right = ci_right;
            item_part1.ri_bottom = ri_bottom;
            item_part1.ri_top = ri_top;
        }

        // 将 item 从 crossCells 各链表中 移除
        void UnIndex(int itemIndex)
        {
            auto& item_part2 = Ref1(itemIndex);
            for (int i = 0; i < item_part2.crossCellsCount; ++i)
            {
                RemoveNode(item_part2, i);
            }
            item_part2.crossCellsCount = 0;     // 防御式编程, 避免反复 UnIndex 出问题
        }









        /******************************************************************************************/
        // 下面是各种碰撞检测工具函数
        /******************************************************************************************/






        // 检查两个 item 是否相交
        static bool Intersection(IT1 const &a, IT1 const&b)
        {
            unsigned int r;
            r = a.halfSize.w + b.halfSize.w;
            if ((unsigned int)(a.centerPos.x - b.centerPos.x + r) > r + r) return false;
            r = a.halfSize.h + b.halfSize.h;
            if ((unsigned int)(a.centerPos.y - b.centerPos.y + r) > r + r) return false;
            return true;
        }

        // 检查 pos 是否位于 item 内
        static bool Contains(IT1 const &item, AbcdPoint const &pos)
        {
            return item.centerPos.x - item.halfSize.w <= pos.x
                && item.centerPos.x + item.halfSize.w >= pos.x
                && item.centerPos.y - item.halfSize.h <= pos.y
                && item.centerPos.y + item.halfSize.h >= pos.y;
        }


        // 根据 handler 向 inSearchArea 填充的 cells foreach, 
        // 对于符合 groupId, layerMask 检索 条件的 item 将放入 outPassItems 队列
        // 不符合条件的,  放入 outNopassItems 队列
        // 如果 handler 返回 false 表示终止扫描
        // 因为函数末尾会恢复处理标记, 故理论上讲不应该异常退出导致执行不到那段
        void FindByArea(
            // 会反复被调用( 填充 inSearchArea ) 以扩大搜索范围, 直到返回 0. 通常返回一波要扫的 area 个数
            std::function<size_t()> handler
            // 该容器可能是被预填充的, 也可能是每次在 handler 中填充的
            , std::vector<int>& inSearchArea
            // 有效的 items 输出容器
            , std::vector<int>& outPassItems
            // 无效的 items 输出容器( 这个可以不必理会. 要求传入是为了空间复用考虑 )
            , std::vector<int>& outNopassItems
            // 传空则不做附加检查. 否则根据返回值来确定是否 pass
            , std::function<bool(int)> checker
            // Find 行为本身是模拟一个临时创建的 item 来的
            , int groupId = 0, unsigned int layerMask = 0xFFFFFFFFu
            )
        {
            outPassItems.clear();
            outNopassItems.clear();

            size_t i = 0;
            while (auto searchAreaLen = handler())
            {
                assert(searchAreaLen <= inSearchArea.size());

                for (; i < searchAreaLen; ++i)
                {
                    // 遍历 cell 中的链表
                    for (int nodeIndex = cells[inSearchArea[i]]; nodeIndex >= 0; nodeIndex = nodes.Ref<0>(nodeIndex).next)
                    {
                        auto& node = nodes.Ref<0>(nodeIndex);       // 引用到实例
                        auto& item_part2 = Ref1(node.itemIndex);

                        if (item_part2.handled) continue;           // 本次函数中, 已处理过的, 略过
                        else item_part2.handled = true;

                        if ((groupId & item_part2.groupId) != 0)
                        {
                            if (groupId == item_part2.groupId) goto LabNopass;
                        }
                        if ((layerMask & item_part2.layerMask) == 0) goto LabNopass;

                        if (checker) if (!checker(node.itemIndex)) goto LabNopass;

                        outPassItems.push_back(node.itemIndex);
                        continue;

                    LabNopass:
                        outNopassItems.push_back(node.itemIndex);
                    }
                }
            }

            // 恢复本次处理标记
            for (size_t i = 0; i < outPassItems.size(); ++i) Ref1(outPassItems[i]).handled = false;
            for (size_t i = 0; i < outNopassItems.size(); ++i) Ref1(outNopassItems[i]).handled = false;
        }


        // 查找出与某个坐标值相重叠的 items
        void FindIntersection(
            AbcdPoint const &pos
            // 有效的 items 输出容器
            , std::vector<int>& outPassItems
            // Find 行为本身是模拟一个临时创建的 item 来的
            , int groupId = 0, unsigned int layerMask = 0xFFFFFFFFu
            )
        {
            outPassItems.clear();

            // 算出 pos 对应的格子的 2维 坐标
            int ci = pos.x / cellWidth;
            int ri = pos.y / cellHeight;

            // 遍历 cell 中的链表
            for (int nodeIndex = cells[GetCellIndex(ci, ri)]; nodeIndex >= 0; nodeIndex = nodes.Ref<0>(nodeIndex).next)
            {
                auto& node = nodes.Ref<0>(nodeIndex);
                auto& item_part1 = Ref(node.itemIndex);
                auto& item_part2 = Ref1(node.itemIndex);

                if ((groupId & item_part2.groupId) != 0)
                {
                    if (groupId == item_part2.groupId) continue;
                }
                if ((layerMask & item_part2.layerMask) == 0) continue;

                if (Contains(item_part1, pos))
                {
                    outPassItems.push_back(node.itemIndex);
                }
            }
        }


        // 查找出与某个 item 相重叠的 items
        void FindIntersection(
            int tarItemIndex
            // 有效的 items 输出容器
            , std::vector<int>& outPassItems
            // 无效的 items 输出容器( 这个可以不必理会. 要求传入是为了空间复用考虑 )
            , std::vector<int>& outNopassItems
            )
        {
            outPassItems.clear();
            outNopassItems.clear();

            auto& tar = Ref(tarItemIndex);
            auto& tar1 = Ref1(tarItemIndex);

            // 遍历 item 的 crossCells
            for (int i = 0; i < tar1.crossCellsCount; ++i)
            {
                // 遍历 cell 中的链表
                for (int nodeIndex = cells[GetCellIndex(tar1.crossCells[i].cellIndex2d)]; nodeIndex >= 0; nodeIndex = nodes.Ref<0>(nodeIndex).next)
                {
                    auto& node = nodes.Ref<0>(nodeIndex);
                    auto& item = Ref(node.itemIndex);
                    auto& item1 = Ref1(node.itemIndex);

                    if (item1.handled) continue;                 // 本次函数中, 已处理过的, 略过
                    else item1.handled = true;

                    if ((tar1.groupId & item1.groupId) != 0)
                    {
                        if (tar1.groupId == item1.groupId) goto LabNopass;
                    }
                    if ((tar1.layerMask & item1.layerMask) == 0) goto LabNopass;

                    if (Intersection(tar, item))
                    {
                        outPassItems.push_back(node.itemIndex);
                        continue;
                    }

                LabNopass:
                    outNopassItems.push_back(node.itemIndex);
                }
            }

            // 恢复本次处理标记
            for (size_t i = 0; i < outPassItems.size(); ++i) Ref1(outPassItems[i]).handled = false;
            for (size_t i = 0; i < outNopassItems.size(); ++i) Ref1(outNopassItems[i]).handled = false;
        }


        // 查找出与某个 item 所占的 cells 相重叠的 items ( 从 FindIntersection 简化而来 )
        void FindNeighbors(
            int tarItemIndex
            // 有效的 items 输出容器
            , std::vector<int>& outPassItems
            )
        {
            outPassItems.clear();

            auto& tar1 = Ref1(tarItemIndex);

            // 遍历 item 的 crossCells
            for (int i = 0; i < tar1.crossCellsCount; ++i)
            {
                // 遍历 cell 中的链表
                for (int nodeIndex = cells[GetCellIndex(tar1.crossCells[i].cellIndex2d)]; nodeIndex >= 0; nodeIndex = nodes.Ref<0>(nodeIndex).next)
                {
                    auto& item1 = Ref1(nodes.Ref<0>(nodeIndex).itemIndex);

                    if ((tar1.groupId & item1.groupId) != 0)
                    {
                        if (tar1.groupId == item1.groupId) continue;
                    }
                    if ((tar1.layerMask & item1.layerMask) == 0) continue;

                    outPassItems.push_back(nodes.Ref<0>(nodeIndex).itemIndex);
                }
            }
        }



        // 查找出与某个坐标值相重叠的 items 的第 1 个( 直接拿上面函数来简化的。 有其他需求再说 )
        int FirstIntersection(
            AbcdPoint const &pos
            // Find 行为本身是模拟一个临时创建的 item 来的
            , int groupId = 0, unsigned int layerMask = 0xFFFFFFFFu
            )
        {
            // 算出 pos 对应的格子的 2维 坐标
            int ci = pos.x / cellWidth;
            int ri = pos.y / cellHeight;

            // 取 cell 中的链表头, 准备遍历
            auto nodeIndex = cells[GetCellIndex(ri, ci)];
            if (nodeIndex == -1) return -1;
            auto& node = nodes.Ref<0>(nodeIndex);
            auto& item = Ref(node.itemIndex);

            if ((groupId & item.groupId) != 0)
            {
                if (groupId == item.groupId) return -1;
            }
            if ((layerMask & item.layerMask) == 0) return -1;

            if (Contains(item, pos))
            {
                return node.itemIndex;
            }
            return -1;
        }

    };

}
