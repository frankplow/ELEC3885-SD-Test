#include <gtest/gtest.h>
#include <stdio.h>
#include <string.h>

extern "C" {
#include "qtff.h"
}

class QTFFTest : public ::testing::Test {
 protected:
  FILE *fd;
  size_t size;

  void SetUp() override { fd = fopen("test.mov", "rb"); }

  void TearDown() override { fclose(fd); }
};

TEST_F(QTFFTest, TopAtoms) {
  QTFFMovieFile movie_file;
  const QTFFError err = qtff_read_movie_file(fd, &movie_file);
  ASSERT_EQ(err, QTFFErrorNone) << "Error parsing movie file";

  ASSERT_EQ(movie_file.file_type_compatibility_count, 1);
  ASSERT_EQ(movie_file.file_type_compatibility[0].header.offset, 0);
  ASSERT_EQ(movie_file.file_type_compatibility[0].header.size, 20);

  ASSERT_EQ(movie_file.movie_count, 1);
  ASSERT_EQ(movie_file.movie[0].header.offset, 28330);
  ASSERT_EQ(movie_file.movie[0].header.size, 706);

  ASSERT_EQ(movie_file.movie_data_count, 1);
  ASSERT_EQ(movie_file.movie_data[0].header.offset, 28);
  ASSERT_EQ(movie_file.movie_data[0].header.size, 28302);

  ASSERT_EQ(movie_file.free_count, 0);

  ASSERT_EQ(movie_file.skip_count, 0);

  ASSERT_EQ(movie_file.wide_count, 1);
  ASSERT_EQ(movie_file.wide[0].header.offset, 20);
  ASSERT_EQ(movie_file.wide[0].header.size, 8);

  ASSERT_EQ(movie_file.preview_count, 0);
}
