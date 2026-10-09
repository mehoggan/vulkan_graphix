#include "vulkan_graphix/VertexTypes/AttributeTraits.hpp"
#include "vulkan_graphix/VertexTypes/BatchData.hpp"
#include "vulkan_graphix/VertexTypes/Indices.hpp"
#include "vulkan_graphix/VertexTypes/InterleavedData.hpp"
#include "vulkan_graphix/VertexTypes/InterleavedDatum.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

#include <glm/glm.hpp>

#include <gtest/gtest.h>

namespace {

// Mimics a user-defined attribute type that isn't a glm vector: it must
// expose its own `dimension` for AttributeDimension to fall back to.
struct CustomAttribute {
  static constexpr std::size_t dimension = 5;
};

struct ZeroInitAttribute {
  std::int32_t m_value = 0;
};

}  // namespace

using vulkan_graphix::VertexTypes::AttributeDimension;
using vulkan_graphix::VertexTypes::AttributeTraits;
using vulkan_graphix::VertexTypes::BatchData;
using vulkan_graphix::VertexTypes::Indices;
using vulkan_graphix::VertexTypes::InterleavedData;
using vulkan_graphix::VertexTypes::InterleavedDatum;

TEST(AttributeDimensionTest, GlmVectorsReportTheirComponentCount) {
  EXPECT_EQ(2u, AttributeDimension<glm::vec2>::value);
  EXPECT_EQ(3u, AttributeDimension<glm::vec3>::value);
  EXPECT_EQ(4u, AttributeDimension<glm::vec4>::value);
  EXPECT_EQ(2u, AttributeDimension<glm::ivec2>::value);
  EXPECT_EQ(3u, AttributeDimension<glm::uvec3>::value);
}

TEST(AttributeDimensionTest, CustomTypeFallsBackToItsDimensionMember) {
  EXPECT_EQ(5u, AttributeDimension<CustomAttribute>::value);
}

TEST(AttributeTraitsTest, StrideIsTheSumOfAttributeSizes) {
  using traits = AttributeTraits<glm::vec3, glm::vec2>;
  EXPECT_EQ(sizeof(glm::vec3) + sizeof(glm::vec2), traits::stride);
  EXPECT_EQ(2u, traits::attribute_count);
  EXPECT_EQ(5u, traits::attribute_value_count);
}

TEST(AttributeTraitsTest, ByteOffsetsAreCumulative) {
  using traits = AttributeTraits<glm::vec3, glm::vec2, glm::vec4>;
  auto offsets = traits::byteOffsets();
  EXPECT_EQ(0u, offsets[0]);
  EXPECT_EQ(sizeof(glm::vec3), offsets[1]);
  EXPECT_EQ(sizeof(glm::vec3) + sizeof(glm::vec2), offsets[2]);
}

TEST(InterleavedDatumTest, DefaultConstructedValuesAreZeroInitialized) {
  InterleavedDatum<ZeroInitAttribute, ZeroInitAttribute> datum;
  EXPECT_EQ(0, datum.get<0>().m_value);
  EXPECT_EQ(0, datum.get<1>().m_value);
}

TEST(InterleavedDatumTest, ValueConstructorStoresEachAttribute) {
  InterleavedDatum<glm::vec3, glm::vec2> datum(
      glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(4.0f, 5.0f));
  EXPECT_EQ(glm::vec3(1.0f, 2.0f, 3.0f), datum.get<0>());
  EXPECT_EQ(glm::vec2(4.0f, 5.0f), datum.get<1>());
}

TEST(InterleavedDatumTest, EqualityComparesAllAttributes) {
  InterleavedDatum<glm::vec3, glm::vec2> first(
      glm::vec3(1.0f), glm::vec2(2.0f));
  InterleavedDatum<glm::vec3, glm::vec2> second(
      glm::vec3(1.0f), glm::vec2(2.0f));
  InterleavedDatum<glm::vec3, glm::vec2> third(
      glm::vec3(1.0f), glm::vec2(3.0f));

  EXPECT_EQ(first, second);
  EXPECT_NE(first, third);
}

TEST(InterleavedDatumTest, SwapExchangesValues) {
  InterleavedDatum<glm::vec2, glm::vec2> first(
      glm::vec2(1.0f, 2.0f), glm::vec2(3.0f, 4.0f));
  InterleavedDatum<glm::vec2, glm::vec2> second(
      glm::vec2(5.0f, 6.0f), glm::vec2(7.0f, 8.0f));

  swap(first, second);

  EXPECT_EQ(glm::vec2(5.0f, 6.0f), first.get<0>());
  EXPECT_EQ(glm::vec2(1.0f, 2.0f), second.get<0>());
}

TEST(InterleavedDataTest, TracksAttributeAndByteCounts) {
  using datum_type = InterleavedDatum<glm::vec3, glm::vec2>;
  InterleavedData<glm::vec3, glm::vec2> data(
      std::vector<datum_type>{
          datum_type(glm::vec3(1.0f), glm::vec2(2.0f)),
          datum_type(glm::vec3(3.0f), glm::vec2(4.0f)),
          datum_type(glm::vec3(5.0f), glm::vec2(6.0f)),
      });

  EXPECT_EQ(3u, data.getAttributeCount());
  EXPECT_EQ((sizeof(glm::vec3) + sizeof(glm::vec2)) * 3u, data.getByteCount());
  EXPECT_EQ(3u, data.getData().size());
}

TEST(InterleavedDataTest, BuildsUpLikeAVector) {
  using datum_type = InterleavedDatum<glm::vec3, glm::vec2>;
  InterleavedData<glm::vec3, glm::vec2> data = {
      {glm::vec3(1.0f), glm::vec2(2.0f)}};
  EXPECT_FALSE(data.empty());
  data.add({glm::vec3(3.0f), glm::vec2(4.0f)});
  data.append(
      {datum_type(glm::vec3(5.0f), glm::vec2(6.0f)),
       datum_type(glm::vec3(7.0f), glm::vec2(8.0f))});
  ASSERT_EQ(4u, data.size());
  EXPECT_EQ(glm::vec3(3.0f), data[1].get<0>());
  data[1].get<1>() = glm::vec2(9.0f);
  EXPECT_EQ(glm::vec2(9.0f), data[1].get<1>());

  float x_sum = 0.0f;
  for (const datum_type& datum : data) {
    x_sum += datum.get<0>().x;
  }
  EXPECT_FLOAT_EQ(16.0f, x_sum);

  InterleavedData<glm::vec3, glm::vec2> copy(2);
  EXPECT_EQ(datum_type(), copy[0]);
  copy.append(data);
  EXPECT_EQ(6u, copy.size());

  data.clear();
  EXPECT_TRUE(data.empty());
  EXPECT_EQ(0u, data.getByteCount());
}

TEST(InterleavedDataTest, PacksEachAttributeAtItsTraitsOffset) {
  InterleavedData<glm::vec3, glm::vec4, glm::vec2> data = {
      {glm::vec3(1.0f, 2.0f, 3.0f),
       glm::vec4(4.0f, 5.0f, 6.0f, 7.0f),
       glm::vec2(8.0f, 9.0f)},
      {glm::vec3(10.0f, 11.0f, 12.0f),
       glm::vec4(13.0f, 14.0f, 15.0f, 16.0f),
       glm::vec2(17.0f, 18.0f)}};
  const std::vector<std::byte> bytes = data.pack();
  ASSERT_EQ(data.getByteCount(), bytes.size());
  ASSERT_EQ(2u * 9u * sizeof(float), bytes.size());

  // Plain floats 1..18 in order: position, color, texcoord, per vertex.
  std::vector<float> floats(bytes.size() / sizeof(float));
  std::memcpy(floats.data(), bytes.data(), bytes.size());
  for (std::size_t i = 0; i < floats.size(); ++i) {
    EXPECT_FLOAT_EQ(static_cast<float>(i + 1), floats[i]) << "float " << i;
  }
}

TEST(BatchDataTest, TracksIndependentArraysPerAttribute) {
  BatchData<glm::vec3, glm::vec2> data(
      std::vector<glm::vec3>{glm::vec3(1.0f), glm::vec3(2.0f)},
      std::vector<glm::vec2>{glm::vec2(3.0f), glm::vec2(4.0f)});

  EXPECT_EQ(2u, (BatchData<glm::vec3, glm::vec2>::attribute_type_count));
  EXPECT_EQ(2u, data.attributeCount());
  EXPECT_EQ(2u, data.data<0>().size());
  EXPECT_EQ(sizeof(glm::vec3) * 2u, data.byteCount<0>());
  EXPECT_EQ(sizeof(glm::vec2) * 2u, data.byteCount<1>());
}

TEST(IndicesTest, TracksCountAndByteSize) {
  Indices<std::uint32_t> indices(
      std::vector<std::uint32_t>{0u, 1u, 2u, 0u, 2u, 3u});

  EXPECT_EQ(6u, indices.getIndicesCount());
  EXPECT_EQ(sizeof(std::uint32_t) * 6u, indices.getByteCount());
}

TEST(IndicesTest, EqualityComparesUnderlyingData) {
  Indices<std::uint16_t> first(std::vector<std::uint16_t>{1, 2, 3});
  Indices<std::uint16_t> second(std::vector<std::uint16_t>{1, 2, 3});
  Indices<std::uint16_t> third(std::vector<std::uint16_t>{1, 2, 4});

  EXPECT_EQ(first, second);
  EXPECT_NE(first, third);
}
