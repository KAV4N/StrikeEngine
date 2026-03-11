#include <gtest/gtest.h>
#include <StrikeEngine.h>

static constexpr float kEps = 1e-5f;


// ============================================================
// Default constructor
// ============================================================

TEST(BoundsTest, DefaultConstructor_MinIsMaxFloat) {
    Strike::Bounds b;
    EXPECT_FLOAT_EQ(b.aabbMin.x, std::numeric_limits<float>::max());
    EXPECT_FLOAT_EQ(b.aabbMin.y, std::numeric_limits<float>::max());
    EXPECT_FLOAT_EQ(b.aabbMin.z, std::numeric_limits<float>::max());
}

TEST(BoundsTest, DefaultConstructor_MaxIsLowestFloat) {
    Strike::Bounds b;
    EXPECT_FLOAT_EQ(b.aabbMax.x, std::numeric_limits<float>::lowest());
    EXPECT_FLOAT_EQ(b.aabbMax.y, std::numeric_limits<float>::lowest());
    EXPECT_FLOAT_EQ(b.aabbMax.z, std::numeric_limits<float>::lowest());
}


// ============================================================
// Single-point constructor
// ============================================================

TEST(BoundsTest, PointConstructor_MinEqualsPoint) {
    Strike::Bounds b(glm::vec3(1.0f, 2.0f, 3.0f));
    EXPECT_NEAR(b.aabbMin.x, 1.0f, kEps);
    EXPECT_NEAR(b.aabbMin.y, 2.0f, kEps);
    EXPECT_NEAR(b.aabbMin.z, 3.0f, kEps);
}

TEST(BoundsTest, PointConstructor_MaxEqualsPoint) {
    Strike::Bounds b(glm::vec3(1.0f, 2.0f, 3.0f));
    EXPECT_NEAR(b.aabbMax.x, 1.0f, kEps);
    EXPECT_NEAR(b.aabbMax.y, 2.0f, kEps);
    EXPECT_NEAR(b.aabbMax.z, 3.0f, kEps);
}

TEST(BoundsTest, PointConstructor_Origin) {
    Strike::Bounds b(glm::vec3(0.0f));
    EXPECT_NEAR(b.aabbMin.x, 0.0f, kEps);
    EXPECT_NEAR(b.aabbMax.x, 0.0f, kEps);
}

TEST(BoundsTest, PointConstructor_NegativePoint) {
    Strike::Bounds b(glm::vec3(-5.0f, -3.0f, -1.0f));
    EXPECT_NEAR(b.aabbMin.x, -5.0f, kEps);
    EXPECT_NEAR(b.aabbMax.x, -5.0f, kEps);
}


// ============================================================
// Min/max constructor
// ============================================================

TEST(BoundsTest, MinMaxConstructor_StoresMin) {
    Strike::Bounds b(glm::vec3(-1.0f, -2.0f, -3.0f), glm::vec3(1.0f, 2.0f, 3.0f));
    EXPECT_NEAR(b.aabbMin.x, -1.0f, kEps);
    EXPECT_NEAR(b.aabbMin.y, -2.0f, kEps);
    EXPECT_NEAR(b.aabbMin.z, -3.0f, kEps);
}

TEST(BoundsTest, MinMaxConstructor_StoresMax) {
    Strike::Bounds b(glm::vec3(-1.0f, -2.0f, -3.0f), glm::vec3(1.0f, 2.0f, 3.0f));
    EXPECT_NEAR(b.aabbMax.x, 1.0f, kEps);
    EXPECT_NEAR(b.aabbMax.y, 2.0f, kEps);
    EXPECT_NEAR(b.aabbMax.z, 3.0f, kEps);
}

TEST(BoundsTest, MinMaxConstructor_Degenerate_MinEqualsMax) {
    Strike::Bounds b(glm::vec3(4.0f), glm::vec3(4.0f));
    EXPECT_NEAR(b.aabbMin.x, 4.0f, kEps);
    EXPECT_NEAR(b.aabbMax.x, 4.0f, kEps);
}


// ============================================================
// Three-point constructor
// ============================================================

TEST(BoundsTest, ThreePointConstructor_MaxIsComponentwiseMax) {
    Strike::Bounds b(
        glm::vec3(1.0f, 5.0f, 3.0f),
        glm::vec3(4.0f, 2.0f, 6.0f),
        glm::vec3(2.0f, 8.0f, 1.0f)
    );
    EXPECT_NEAR(b.aabbMax.x, 4.0f, kEps);
    EXPECT_NEAR(b.aabbMax.y, 8.0f, kEps);
    EXPECT_NEAR(b.aabbMax.z, 6.0f, kEps);
}

TEST(BoundsTest, ThreePointConstructor_MinIsComponentwiseMin) {
    Strike::Bounds b(
        glm::vec3(1.0f, 5.0f, 3.0f),
        glm::vec3(4.0f, 2.0f, 6.0f),
        glm::vec3(2.0f, 8.0f, 1.0f)
    );
    EXPECT_NEAR(b.aabbMin.x, 1.0f, kEps);
    EXPECT_NEAR(b.aabbMin.y, 2.0f, kEps);
    EXPECT_NEAR(b.aabbMin.z, 1.0f, kEps);
}

TEST(BoundsTest, ThreePointConstructor_AllSamePoint) {
    glm::vec3 p(3.0f, 3.0f, 3.0f);
    Strike::Bounds b(p, p, p);
    EXPECT_NEAR(b.aabbMin.x, 3.0f, kEps);
    EXPECT_NEAR(b.aabbMax.x, 3.0f, kEps);
}

TEST(BoundsTest, ThreePointConstructor_NegativeCoordinates) {
    Strike::Bounds b(
        glm::vec3(-3.0f, -1.0f, -5.0f),
        glm::vec3(-1.0f, -4.0f, -2.0f),
        glm::vec3(-2.0f, -2.0f, -3.0f)
    );
    EXPECT_NEAR(b.aabbMin.x, -3.0f, kEps);
    EXPECT_NEAR(b.aabbMax.x, -1.0f, kEps);
}


// ============================================================
// unionBounds
// ============================================================

TEST(BoundsTest, UnionBounds_ExpandsMaxToLarger) {
    Strike::Bounds a(glm::vec3(0.0f), glm::vec3(1.0f));
    Strike::Bounds b(glm::vec3(0.0f), glm::vec3(3.0f));
    a.unionBounds(b);
    EXPECT_NEAR(a.aabbMax.x, 3.0f, kEps);
    EXPECT_NEAR(a.aabbMax.y, 3.0f, kEps);
    EXPECT_NEAR(a.aabbMax.z, 3.0f, kEps);
}

TEST(BoundsTest, UnionBounds_ExpandsMinToSmaller) {
    Strike::Bounds a(glm::vec3(0.0f), glm::vec3(1.0f));
    Strike::Bounds b(glm::vec3(-2.0f), glm::vec3(0.0f));
    a.unionBounds(b);
    EXPECT_NEAR(a.aabbMin.x, -2.0f, kEps);
    EXPECT_NEAR(a.aabbMin.y, -2.0f, kEps);
    EXPECT_NEAR(a.aabbMin.z, -2.0f, kEps);
}

TEST(BoundsTest, UnionBounds_WithSelf_Unchanged) {
    Strike::Bounds a(glm::vec3(-1.0f), glm::vec3(1.0f));
    a.unionBounds(a);
    EXPECT_NEAR(a.aabbMin.x, -1.0f, kEps);
    EXPECT_NEAR(a.aabbMax.x,  1.0f, kEps);
}

TEST(BoundsTest, UnionBounds_WithContainedBounds_Unchanged) {
    Strike::Bounds a(glm::vec3(-5.0f), glm::vec3(5.0f));
    Strike::Bounds b(glm::vec3(-1.0f), glm::vec3(1.0f));
    a.unionBounds(b);
    EXPECT_NEAR(a.aabbMin.x, -5.0f, kEps);
    EXPECT_NEAR(a.aabbMax.x,  5.0f, kEps);
}

TEST(BoundsTest, UnionBounds_WithDefaultBounds_Unchanged) {
    Strike::Bounds a(glm::vec3(-1.0f), glm::vec3(1.0f));
    Strike::Bounds def;
    a.unionBounds(def);
    EXPECT_NEAR(a.aabbMin.x, -1.0f, kEps);
    EXPECT_NEAR(a.aabbMax.x,  1.0f, kEps);
}

TEST(BoundsTest, UnionBounds_IsCommutativeOnResult) {
    Strike::Bounds a(glm::vec3(-2.0f, 0.0f, -1.0f), glm::vec3(1.0f, 3.0f, 2.0f));
    Strike::Bounds b(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(3.0f, 2.0f, 4.0f));

    Strike::Bounds ab = a; ab.unionBounds(b);
    Strike::Bounds ba = b; ba.unionBounds(a);

    EXPECT_NEAR(ab.aabbMin.x, ba.aabbMin.x, kEps);
    EXPECT_NEAR(ab.aabbMin.y, ba.aabbMin.y, kEps);
    EXPECT_NEAR(ab.aabbMin.z, ba.aabbMin.z, kEps);
    EXPECT_NEAR(ab.aabbMax.x, ba.aabbMax.x, kEps);
    EXPECT_NEAR(ab.aabbMax.y, ba.aabbMax.y, kEps);
    EXPECT_NEAR(ab.aabbMax.z, ba.aabbMax.z, kEps);
}

TEST(BoundsTest, UnionBounds_Accumulate_ThreeBoxes) {
    Strike::Bounds result(glm::vec3(0.0f), glm::vec3(1.0f));
    result.unionBounds(Strike::Bounds(glm::vec3(-3.0f), glm::vec3(0.0f)));
    result.unionBounds(Strike::Bounds(glm::vec3(0.0f), glm::vec3(5.0f)));
    EXPECT_NEAR(result.aabbMin.x, -3.0f, kEps);
    EXPECT_NEAR(result.aabbMax.x,  5.0f, kEps);
}


// ============================================================
// getSize
// ============================================================

TEST(BoundsTest, GetSize_UnitBox) {
    Strike::Bounds b(glm::vec3(0.0f), glm::vec3(1.0f));
    glm::vec3 s = b.getSize();
    EXPECT_NEAR(s.x, 1.0f, kEps);
    EXPECT_NEAR(s.y, 1.0f, kEps);
    EXPECT_NEAR(s.z, 1.0f, kEps);
}

TEST(BoundsTest, GetSize_AsymmetricBox) {
    Strike::Bounds b(glm::vec3(1.0f, 2.0f, 3.0f), glm::vec3(4.0f, 6.0f, 9.0f));
    glm::vec3 s = b.getSize();
    EXPECT_NEAR(s.x, 3.0f, kEps);
    EXPECT_NEAR(s.y, 4.0f, kEps);
    EXPECT_NEAR(s.z, 6.0f, kEps);
}

TEST(BoundsTest, GetSize_Degenerate_IsZero) {
    Strike::Bounds b(glm::vec3(2.0f), glm::vec3(2.0f));
    glm::vec3 s = b.getSize();
    EXPECT_NEAR(s.x, 0.0f, kEps);
    EXPECT_NEAR(s.y, 0.0f, kEps);
    EXPECT_NEAR(s.z, 0.0f, kEps);
}

TEST(BoundsTest, GetSize_NegativeOrigin) {
    Strike::Bounds b(glm::vec3(-3.0f), glm::vec3(3.0f));
    glm::vec3 s = b.getSize();
    EXPECT_NEAR(s.x, 6.0f, kEps);
    EXPECT_NEAR(s.y, 6.0f, kEps);
    EXPECT_NEAR(s.z, 6.0f, kEps);
}


// ============================================================
// getMidPoint
// ============================================================

TEST(BoundsTest, GetMidPoint_UnitBox_IsCentre) {
    Strike::Bounds b(glm::vec3(0.0f), glm::vec3(2.0f));
    glm::vec3 m = b.getMidPoint();
    EXPECT_NEAR(m.x, 1.0f, kEps);
    EXPECT_NEAR(m.y, 1.0f, kEps);
    EXPECT_NEAR(m.z, 1.0f, kEps);
}

TEST(BoundsTest, GetMidPoint_SymmetricAroundOrigin_IsOrigin) {
    Strike::Bounds b(glm::vec3(-1.0f), glm::vec3(1.0f));
    glm::vec3 m = b.getMidPoint();
    EXPECT_NEAR(m.x, 0.0f, kEps);
    EXPECT_NEAR(m.y, 0.0f, kEps);
    EXPECT_NEAR(m.z, 0.0f, kEps);
}

TEST(BoundsTest, GetMidPoint_Degenerate_IsPoint) {
    Strike::Bounds b(glm::vec3(5.0f, 3.0f, 1.0f), glm::vec3(5.0f, 3.0f, 1.0f));
    glm::vec3 m = b.getMidPoint();
    EXPECT_NEAR(m.x, 5.0f, kEps);
    EXPECT_NEAR(m.y, 3.0f, kEps);
    EXPECT_NEAR(m.z, 1.0f, kEps);
}

TEST(BoundsTest, GetMidPoint_AsymmetricBox) {
    Strike::Bounds b(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(4.0f, 6.0f, 10.0f));
    glm::vec3 m = b.getMidPoint();
    EXPECT_NEAR(m.x, 2.0f, kEps);
    EXPECT_NEAR(m.y, 3.0f, kEps);
    EXPECT_NEAR(m.z, 5.0f, kEps);
}


// ============================================================
// area
// ============================================================

TEST(BoundsTest, Area_UnitCube) {
    Strike::Bounds b(glm::vec3(0.0f), glm::vec3(1.0f));
    EXPECT_NEAR(b.area(), 3.0f, kEps);
}

TEST(BoundsTest, Area_2x3x4Box) {
    Strike::Bounds b(glm::vec3(0.0f), glm::vec3(2.0f, 3.0f, 4.0f));
    EXPECT_NEAR(b.area(), 26.0f, kEps);
}

TEST(BoundsTest, Area_Degenerate_IsZero) {
    Strike::Bounds b(glm::vec3(1.0f), glm::vec3(1.0f));
    EXPECT_NEAR(b.area(), 0.0f, kEps);
}

TEST(BoundsTest, Area_FlatSlabXY) {
    Strike::Bounds b(glm::vec3(0.0f), glm::vec3(2.0f, 2.0f, 0.0f));
    EXPECT_NEAR(b.area(), 4.0f, kEps);
}

TEST(BoundsTest, Area_NegativeOrigin_SameAsCentred) {
    Strike::Bounds pos(glm::vec3(0.0f), glm::vec3(2.0f, 3.0f, 4.0f));
    Strike::Bounds neg(glm::vec3(-1.0f, -1.5f, -2.0f), glm::vec3(1.0f, 1.5f, 2.0f));
    EXPECT_NEAR(pos.area(), neg.area(), kEps);
}