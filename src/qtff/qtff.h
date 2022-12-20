#ifndef QTFF_H_
#define QTFF_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define QTFF_ATOM_ID(str) \
  ((str[0] << 24) + (str[1] << 16) + (str[2] << 8) + str[3])

#define DocLink(url) (<a href = "url"> url</ a>)

typedef uint32_t QTFFAtomSize;

// @TODO: static assert sizeof(QTFFAtomType) == 32
typedef char QTFFAtomType[4];

// Macintosh date format time
// Time passed in seconds since 1904-01-01T00:00:00
typedef uint32_t MacTime;

// Quicktime movie time
// Time since start of movie in time units (1/time_scale)s
typedef uint32_t QTTime;
//
// Quicktime movie duration
// Measured in time units (1/time_scale)s
typedef uint32_t QTDuration;

// @TODO: assert sizeof(QTMatrix) = 36
typedef float QTMatrix[3][3];

typedef uint32_t QTTrackID;

typedef struct {
  size_t offset;
  QTFFAtomSize size;
  QTFFAtomType type;
} QTFFAtomHeader;

///
/// @brief The maximum number of compatible brands .
///
#define QT_MAX_COMPATIBLE_BRANDS 4

///
/// @brief QuickTime identifier for a file format.
///
typedef char QTFileFormat[4];

///
/// @brief File type compatibility atom
/// DocLink(https://developer.apple.com/library/archive/documentation/QuickTime/QTFF/QTFFChap1/qtff1.html#//apple_ref/doc/uid/TP40000939-CH203-CJBCBIFF)
///
typedef struct {
  QTFFAtomHeader header;
  uint32_t major_brand;
  uint32_t minor_version;
  size_t compatible_brands_count;
  QTFileFormat compatible_brands[QT_MAX_COMPATIBLE_BRANDS];
} QTFFFileTypeCompatibilityAtom;

typedef struct {
  QTFFAtomHeader header;
} QTFFMovieDataAtom;

///
/// @brief Free (unused) space atom
/// DocLink(https://developer.apple.com/library/archive/documentation/QuickTime/QTFF/QTFFChap1/qtff1.html#//apple_ref/doc/uid/TP40000939-CH203-55464)
///
typedef struct {
  QTFFAtomHeader header;
} QTFFFreeAtom;

///
/// @brief Skip (unused) space atom
/// DocLink(https://developer.apple.com/library/archive/documentation/QuickTime/QTFF/QTFFChap1/qtff1.html#//apple_ref/doc/uid/TP40000939-CH203-55464)
///
typedef struct {
  QTFFAtomHeader header;
} QTFFSkipAtom;

///
/// @brief Wide (reserved) space atom
/// DocLink(https://developer.apple.com/library/archive/documentation/QuickTime/QTFF/QTFFChap1/qtff1.html#//apple_ref/doc/uid/TP40000939-CH203-55464)
///
typedef struct {
  QTFFAtomHeader header;
} QTFFWideAtom;

///
/// @brief Preview atom
/// DocLink(https://developer.apple.com/library/archive/documentation/QuickTime/QTFF/QTFFChap1/qtff1.html#//apple_ref/doc/uid/TP40000939-CH203-38240)
///
typedef struct {
  QTFFAtomHeader header;
  MacTime modification_time;
  uint16_t version;
  QTFFAtomType atom_type;
  uint16_t atom_index;
} QTFFPreviewAtom;

///
/// @brief Maximum number of MovieAtom child atoms.
///
#define QT_MAX_MOVIE_ATOM_CHILDREN 16

///
/// @brief Movie atom.
///
typedef struct {
  QTFFAtomHeader header;
  size_t children_count;
  QTFFAtomHeader children[QT_MAX_MOVIE_ATOM_CHILDREN];
} QTFFMovieAtom;

///
/// @brief Movie header atom.
///
typedef struct {
  QTFFAtomHeader header;
  char version;
  char flags[3];
  MacTime creation_time;
  MacTime modification_time;
  uint32_t time_scale;
  QTDuration duration;
  uint32_t preferred_rate;
  uint16_t preferred_volume;
  char _reserved[10];
  QTMatrix matrix_structure;
  QTTime preview_time;
  QTDuration preview_duration;
  QTTime poster_time;
  QTTime selection_time;
  QTDuration selection_duration;
  QTTime current_time;
  QTTrackID next_track_id;
} QTFFMovieHeaderAtom;

// @TODO: assert sizeof(MovieHeaderAtom) == 108

///
/// @brief The maximum number of entries in the color table
///
#define QT_MAX_COLOR_TABLE_SIZE

///
/// @brief Color table atom
///
typedef struct {
  QTFFAtomHeader header;
  uint32_t color_table_seed;
} ColorTableAtom;

#define QTFF_MAX_FILE_TYPE_COMPATIBILITY_ATOMS 1
#define QTFF_MAX_MOVIE_ATOMS 1
#define QTFF_MAX_MOVIE_DATA_ATOMS 4
#define QTFF_MAX_FREE_ATOMS 4
#define QTFF_MAX_SKIP_ATOMS 4
#define QTFF_MAX_WIDE_ATOMS 4
#define QTFF_MAX_PREVIEW_ATOMS 1

typedef struct {
  size_t file_type_compatibility_count;
  QTFFFileTypeCompatibilityAtom
      file_type_compatibility[QTFF_MAX_FILE_TYPE_COMPATIBILITY_ATOMS];

  size_t movie_count;
  QTFFMovieAtom movie[QTFF_MAX_MOVIE_ATOMS];

  size_t movie_data_count;
  QTFFMovieDataAtom movie_data[QTFF_MAX_MOVIE_DATA_ATOMS];

  size_t free_count;
  QTFFFreeAtom free[QTFF_MAX_FREE_ATOMS];

  size_t skip_count;
  QTFFSkipAtom skip[QTFF_MAX_SKIP_ATOMS];

  size_t wide_count;
  QTFFWideAtom wide[QTFF_MAX_WIDE_ATOMS];

  size_t preview_count;
  QTFFPreviewAtom preview[QTFF_MAX_PREVIEW_ATOMS];
} QTFFMovieFile;

///
/// @brief A generic error in the QTFF library.
///
typedef enum {
  QTFFErrorNone = 0,
  QTFFErrorEOF,
  QTFFErrorIOError,
  QTFFErrorAtomTooLong,
  QTFFErrorNotBasicAtomType,
  QTFFErrorTooManyAtoms,
} QTFFError;

///
/// @brief Read a QuickTime movie file
///
/// @param [in] fd    The file descriptor
/// @param [out] out  The parsed file
/// @return           Whether or not the file was read successfully
///
QTFFError qtff_read_movie_file(FILE *fd, QTFFMovieFile *out);

///
/// @brief Write a QuickTime movie file
///
/// @param [in] fd    The file descriptor
/// @param [in] in    The file
/// @return           Whether or not the file was written successfully
///
QTFFError qtff_write_movie_file(FILE *fd, QTFFMovieFile *in);

///
/// @brief Read the header of an atom
///
/// The current file offset must be at the start of the atom.
///
/// @param [in] fd    The file descriptor
/// @param [out] out  Output
/// @return           Whether or not an atom was read successfully
///
QTFFError qtff_read_atom_header(FILE *fd, QTFFAtomHeader *out);

#endif  // QTFF_H_
