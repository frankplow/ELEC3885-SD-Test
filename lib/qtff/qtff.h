#ifndef QTFF_H_
#define QTFF_H_

/// @file qtff.h
/// @author Frank Plowman <post@frankplowman.com>
/// @brief QuickTime file format library main header file

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/// @addtogroup QTFF
/// @{

#define QTFF_ATOM_ID(str) \
  ((str[0] << 24) + (str[1] << 16) + (str[2] << 8) + str[3])

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
typedef float QTMatrix[9];

typedef uint32_t QTTrackID;

typedef struct {
  size_t offset;
  QTFFAtomSize size;
  QTFFAtomType type;
} QTFFAtomHeader;

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

///
/// @brief The maximum number of compatible brands .
///
#define QTFF_MAX_COMPATIBLE_BRANDS 4

///
/// @brief QuickTime identifier for a file format.
///
typedef char QTFileFormat[4];

///
/// @brief File type compatibility atom
/// @see
/// https://developer.apple.com/library/archive/documentation/QuickTime/QTFF/QTFFChap1/qtff1.html#//apple_ref/doc/uid/TP40000939-CH203-CJBCBIFF
///
typedef struct {
  QTFFAtomHeader header;
  uint32_t major_brand;
  uint32_t minor_version;
  size_t compatible_brands_count;
  QTFileFormat compatible_brands[QTFF_MAX_COMPATIBLE_BRANDS];
} QTFFFileTypeCompatibilityAtom;

///
/// @brief Read a file type compatibility atom
///
/// @param [in] fd    The file descriptor
/// @param [out] out  The parsed atom
/// @return           Whether or not the atom was read successfully
///
QTFFError qtff_read_file_type_compatibility_atom(
    FILE *fd, QTFFFileTypeCompatibilityAtom *out);

typedef struct {
  QTFFAtomHeader header;
} QTFFMovieDataAtom;

///
/// @brief Read a movie data atom
///
/// @param [in] fd    The file descriptor
/// @param [out] out  The parsed atom
/// @return           Whether or not the atom was read successfully
///
QTFFError qtff_read_movie_data_atom(FILE *fd, QTFFMovieDataAtom *out);

///
/// @brief Free (unused) space atom
/// @see
/// https://developer.apple.com/library/archive/documentation/QuickTime/QTFF/QTFFChap1/qtff1.html#//apple_ref/doc/uid/TP40000939-CH203-55464
///
typedef struct {
  QTFFAtomHeader header;
} QTFFFreeAtom;

///
/// @brief Read a free atom
///
/// @param [in] fd    The file descriptor
/// @param [out] out  The parsed atom
/// @return           Whether or not the atom was read successfully
///
QTFFError qtff_read_free_atom(FILE *fd, QTFFFreeAtom *out);

///
/// @brief Skip (unused) space atom
/// @see
/// https://developer.apple.com/library/archive/documentation/QuickTime/QTFF/QTFFChap1/qtff1.html#//apple_ref/doc/uid/TP40000939-CH203-55464
///
typedef struct {
  QTFFAtomHeader header;
} QTFFSkipAtom;

///
/// @brief Read a skip atom
///
/// @param [in] fd    The file descriptor
/// @param [out] out  The parsed atom
/// @return           Whether or not the atom was read successfully
///
QTFFError qtff_read_skip_atom(FILE *fd, QTFFSkipAtom *out);

///
/// @brief Wide (reserved) space atom
/// @see
/// https://developer.apple.com/library/archive/documentation/QuickTime/QTFF/QTFFChap1/qtff1.html#//apple_ref/doc/uid/TP40000939-CH203-55464
///
typedef struct {
  QTFFAtomHeader header;
} QTFFWideAtom;

///
/// @brief Read a wide atom
///
/// @param [in] fd    The file descriptor
/// @param [out] out  The parsed atom
/// @return           Whether or not the atom was read successfully
///
QTFFError qtff_read_wide_atom(FILE *fd, QTFFWideAtom *out);

///
/// @brief Preview atom
/// @see
/// https://developer.apple.com/library/archive/documentation/QuickTime/QTFF/QTFFChap1/qtff1.html#//apple_ref/doc/uid/TP40000939-CH203-38240
///
typedef struct {
  QTFFAtomHeader header;
  MacTime modification_time;
  uint16_t version;
  QTFFAtomType atom_type;
  uint16_t atom_index;
} QTFFPreviewAtom;

///
/// @brief Read a preview atom
///
/// @param [in] fd    The file descriptor
/// @param [out] out  The parsed atom
/// @return           Whether or not the atom was read successfully
///
QTFFError qtff_read_preview_atom(FILE *fd, QTFFPreviewAtom *out);

///
/// @brief Movie header atom.
///
// @TODO: assert sizeof(MovieHeaderAtom) == 108
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

///
/// @brief Read a movie header atom
///
/// @param [in] fd    The file descriptor to read from
/// @param [out] out  The parsed atom
/// @return           Whether or not the atom was read successfully
///
QTFFError qtff_read_movie_header_atom(FILE *fd, QTFFMovieHeaderAtom *out);

///
/// @brief Clipping region atom
///
// @TODO: clipping data field (variable size)
typedef struct {
  QTFFAtomHeader header;
  uint16_t region_size;
  uint64_t region_boundary_box;
} QTFFClippingRegionAtom;

///
/// @brief Read a clipping region atom
///
/// @param [in] fd    The file descriptor to read from
/// @param [out] out  The parsed atom
/// @return           Whether or not the atom was read successfully
///
QTFFError qtff_read_clipping_region_atom(FILE *fd, QTFFClippingRegionAtom *out);

///
/// @brief Clipping atom
///
typedef struct {
  QTFFAtomHeader header;
  QTFFClippingRegionAtom clipping_region;
} QTFFClippingAtom;

///
/// @brief Read a clipping atom
///
/// @param [in] fd    The file descriptor to read from
/// @param [out] out  The parsed atom
/// @return           Whether or not the atom was read successfully
///
QTFFError qtff_read_clipping_atom(FILE *fd, QTFFClippingAtom *out);

///
/// @brief The maximum number of entries in the color table
///
#define QTFF_MAX_COLOR_TABLE_SIZE 16

///
/// @brief Color table atom
///
typedef struct {
  QTFFAtomHeader header;
  uint32_t color_table_seed;
  uint16_t color_table_flags;
  uint16_t color_table_size;
  uint16_t color_array[QTFF_MAX_COLOR_TABLE_SIZE][4];
} QTFFColorTableAtom;

///
/// @brief Read a color table atom
///
/// @param [in] fd    The file descriptor to read from
/// @param [out] out  The parsed atom
/// @return           Whether or not the atom was read successfully
///
QTFFError qtff_read_color_table_atom(FILE *fd, QTFFColorTableAtom *out);

///
/// @brief The maximum number of entries in the user data list
///
#define QTFF_MAX_USER_DATA_ITEMS 16

///
/// @brief User data atom
///
typedef struct {
  QTFFAtomHeader header;
  QTFFAtomHeader user_data_list[QTFF_MAX_USER_DATA_ITEMS];
} QTFFUserDataAtom;

///
/// @brief Read a user data atom
///
/// @param [in] fd    The file descriptor to read from
/// @param [out] out  The parsed atom
/// @return           Whether or not the atom was read successfully
///
QTFFError qtff_read_user_data_atom(FILE *fd, QTFFUserDataAtom *out);

typedef struct {
  QTFFAtomHeader header;
  /* QTFFTrackHeaderAtom track_header; */
  /* QTFFTrackApertureModeDimensionsAtom track_aperture_mode_dimensions; */
  /* QTFFClippingAtom clipping; */
  /* QTFFTrackMatteAtom track_matte; */
  /* QTFFEditAtom edit; */
  /* QTFFTrackReferenceAtom track_reference; */
  /* QTFFTrackExcludeFromAutoselectionAtom track_exclude_from_autoselection; */
  /* QTFFTrackLoadSettingsAtom track_load_settings; */
  /* QTFFMediaAtom media; */
  /* QTFFUserDataAtom user_data; */
} QTFFTrackAtom;

///
/// @brief Read a track atom
///
/// @param [in] fd    The file descriptor to read from
/// @param [out] out  The parsed atom
/// @return           Whether or not the atom was read successfully
///
QTFFError qtff_read_track_atom(FILE *fd, QTFFTrackAtom *out);

///
/// @brief The maximum number of track atoms in a movie atom
///
#define QTFF_MAX_TRACK_ATOMS 4

///
/// @brief Movie atom.
///
typedef struct {
  QTFFAtomHeader header;
  QTFFMovieHeaderAtom movie_header;
  QTFFClippingAtom clipping;
  QTFFColorTableAtom color_table;
  QTFFUserDataAtom user_data;
  size_t track_count;
  QTFFTrackAtom track[QTFF_MAX_TRACK_ATOMS];
} QTFFMovieAtom;

///
/// @brief Read a movie atom
///
/// @param [in] fd    The file descriptor
/// @param [out] out  The parsed atom
/// @return           Whether or not the atom was read successfully
///
QTFFError qtff_read_movie_atom(FILE *fd, QTFFMovieAtom *out);

#define QTFF_MAX_FILE_TYPE_COMPATIBILITY_ATOMS 1
#define QTFF_MAX_MOVIE_ATOMS 1
#define QTFF_MAX_MOVIE_DATA_ATOMS 4
#define QTFF_MAX_FREE_ATOMS 4
#define QTFF_MAX_SKIP_ATOMS 4
#define QTFF_MAX_WIDE_ATOMS 4
#define QTFF_MAX_PREVIEW_ATOMS 1

///
/// @brief A QuickTime movie file
///
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
/// @brief Read a QuickTime movie file
///
/// @param [in] fd    The file descriptor to read from
/// @param [out] out  The parsed file
/// @return           Whether or not the file was read successfully
///
QTFFError qtff_read_movie_file(FILE *fd, QTFFMovieFile *out);

///
/// @brief Write a QuickTime movie file
///
/// @param [in] fd    The file descriptor to write to
/// @param [in] in    The file
/// @return           Whether or not the file was written successfully
///
QTFFError qtff_write_movie_file(FILE *fd, QTFFMovieFile *in);

/// @}
// group QTFF
#endif  // QTFF_H_
