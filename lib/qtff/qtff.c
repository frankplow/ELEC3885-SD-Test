#include <stdio.h>

#include "qtff.h"

// Convert a number from network (big) endian to host endian.
// These must be implemented here as newlib does not provide the
// standard library implementations ntohs & ntohl (arpa/inet.h).
//
// taken from:
// https://stackoverflow.com/questions/2100331/macro-definition-to-determine-big-endian-or-little-endian-machine
static uint16_t qtff_ntoh_16(uint16_t n) {
  unsigned char *np = (unsigned char *)&n;

  return ((uint32_t)np[0] << 8) | (uint32_t)np[1];
}

static uint32_t qtff_ntoh_24(uint32_t n) {
  unsigned char *np = (unsigned char *)&n;

  return ((uint32_t)np[0] << 16) | ((uint32_t)np[0] << 8) | (uint32_t)np[1];
}

static uint32_t qtff_ntoh_32(uint32_t n) {
  unsigned char *np = (unsigned char *)&n;

  return ((uint32_t)np[0] << 24) | ((uint32_t)np[1] << 16) |
         ((uint32_t)np[2] << 8) | (uint32_t)np[3];
}

static uint64_t qtff_ntoh_64(uint64_t n) {
  unsigned char *np = (unsigned char *)&n;

  return ((uint64_t)np[0] << 56) | ((uint64_t)np[1] << 48) |
         ((uint64_t)np[2] << 40) | ((uint64_t)np[3] << 32) |
         ((uint64_t)np[4] << 24) | ((uint64_t)np[5] << 16) |
         ((uint64_t)np[6] << 8) | (uint64_t)np[7];
}

// Read a single 16-bit unsigned integer from the current offset in the file.
// Error handling and EOF detection are done by ferror and feof respectively.
static QTFFError qtff_read_8(FILE *fd, void *out) {
  fread(&out, 1, 1, fd);
  if (ferror(fd)) {
    return QTFFErrorIOError;
  }
  if (feof(fd)) {
    return QTFFErrorEOF;
  }
  return QTFFErrorNone;
}

// Read a single 16-bit unsigned integer from the current offset in the file.
// Error handling and EOF detection are done by ferror and feof respectively.
static QTFFError qtff_read_16(FILE *fd, void *out) {
  uint16_t n;
  fread(&n, 2, 1, fd);
  if (ferror(fd)) {
    return QTFFErrorIOError;
  }
  if (feof(fd)) {
    return QTFFErrorEOF;
  }
  *(uint16_t *)out = qtff_ntoh_16(n);
  return QTFFErrorNone;
}

static QTFFError qtff_read_24(FILE *fd, void *out) {
  uint32_t n;
  fread(&n, 3, 1, fd);
  if (ferror(fd)) {
    return QTFFErrorIOError;
  }
  if (feof(fd)) {
    return QTFFErrorEOF;
  }
  *(uint32_t *)out = qtff_ntoh_24(n);
  return QTFFErrorNone;
}

// Read a single 32-bit unsigned integer from the current offset in the file.
// Error handling and EOF detection are done by ferror and feof respectively.
static QTFFError qtff_read_32(FILE *fd, void *out) {
  uint32_t n;
  fread(&n, 4, 1, fd);
  if (ferror(fd)) {
    return QTFFErrorIOError;
  }
  if (feof(fd)) {
    return QTFFErrorEOF;
  }
  *(uint32_t *)out = qtff_ntoh_32(n);
  return QTFFErrorNone;
}

static QTFFError qtff_read_64(FILE *fd, void *out) {
  uint64_t n;
  fread(&n, 8, 1, fd);
  if (ferror(fd)) {
    return QTFFErrorIOError;
  }
  if (feof(fd)) {
    return QTFFErrorEOF;
  }
  *(uint64_t *)out = qtff_ntoh_64(n);
  return QTFFErrorNone;
}

QTFFError qtff_read_atom_header(FILE *fd, QTFFAtomHeader *out) {
  QTFFError err;
  QTFFAtomHeader atom;
  atom.offset = ftell(fd);
  if ((err = qtff_read_32(fd, &atom.size))) {
    return err;
  }
  // @TODO: use qtff_read32 instead
  fread(&atom.type, 4, 1, fd);

  if (feof(fd)) {
    return QTFFErrorEOF;
  }
  if (ferror(fd)) {
    return QTFFErrorIOError;
  }

  *out = atom;
  return QTFFErrorNone;
}

QTFFError qtff_read_file_type_compatibility_atom(
    FILE *fd, QTFFFileTypeCompatibilityAtom *out) {
  QTFFFileTypeCompatibilityAtom atom;
  QTFFError err;

  // read header
  if ((err = qtff_read_atom_header(fd, &atom.header))) {
    return err;
  }

  // read major brand
  if ((err = qtff_read_32(fd, &atom.major_brand))) {
    return err;
  }

  // read minor version
  if ((err = qtff_read_32(fd, &atom.minor_version))) {
    return err;
  }

  // read compatible brands
  atom.compatible_brands_count = (atom.header.size - 16) / 4;
  if (atom.compatible_brands_count > QTFF_MAX_COMPATIBLE_BRANDS) {
    return QTFFErrorTooManyAtoms;
  }
  // @TODO: use qtff_read32 instead
  fread(atom.compatible_brands, 4, atom.compatible_brands_count, fd);

  *out = atom;
  return QTFFErrorNone;
}

QTFFError qtff_read_movie_data_atom(FILE *fd, QTFFMovieDataAtom *out) {
  QTFFError err;
  if ((err = qtff_read_atom_header(fd, (QTFFAtomHeader *)out))) {
    return err;
  }
  fseek(fd, out->header.size - 8, SEEK_CUR);
  return QTFFErrorNone;
}

QTFFError qtff_read_free_atom(FILE *fd, QTFFFreeAtom *out) {
  QTFFError err;
  if ((err = qtff_read_atom_header(fd, (QTFFAtomHeader *)out))) {
    return err;
  }
  fseek(fd, out->header.size - 8, SEEK_CUR);
  return QTFFErrorNone;
}

QTFFError qtff_read_skip_atom(FILE *fd, QTFFSkipAtom *out) {
  QTFFError err;
  if ((err = qtff_read_atom_header(fd, (QTFFAtomHeader *)out))) {
    return err;
  }
  fseek(fd, out->header.size - 8, SEEK_CUR);
  return QTFFErrorNone;
}

QTFFError qtff_read_wide_atom(FILE *fd, QTFFWideAtom *out) {
  QTFFError err;
  if ((err = qtff_read_atom_header(fd, (QTFFAtomHeader *)out))) {
    return err;
  }
  fseek(fd, out->header.size - 8, SEEK_CUR);
  return QTFFErrorNone;
}

QTFFError qtff_read_preview_atom(FILE *fd, QTFFPreviewAtom *out) {
  QTFFError err;

  // read header
  if ((err = qtff_read_atom_header(fd, &out->header))) {
    return err;
  }

  // read modification time
  if ((err = qtff_read_32(fd, &out->modification_time))) {
    return err;
  }

  // read version number
  if ((err = qtff_read_16(fd, &out->version))) {
    return err;
  }

  // read atom type
  if ((err = qtff_read_32(fd, &out->atom_type))) {
    return err;
  }

  // read atom index
  if ((err = qtff_read_16(fd, &out->atom_index))) {
    return err;
  }

  return QTFFErrorNone;
}

QTFFError qtff_read_movie_header_atom(FILE *fd, QTFFMovieHeaderAtom *out) {
  QTFFError err;

  // read header
  if ((err = qtff_read_atom_header(fd, &out->header))) {
    return err;
  }
  if ((err = qtff_read_8(fd, &out->version))) {
    return err;
  }
  if ((err = qtff_read_24(fd, &out->flags))) {
    return err;
  }
  if ((err = qtff_read_32(fd, &out->creation_time))) {
    return err;
  }
  if ((err = qtff_read_32(fd, &out->modification_time))) {
    return err;
  }
  if ((err = qtff_read_32(fd, &out->time_scale))) {
    return err;
  }
  if ((err = qtff_read_32(fd, &out->duration))) {
    return err;
  }
  if ((err = qtff_read_32(fd, &out->preferred_rate))) {
    return err;
  }
  if ((err = qtff_read_16(fd, &out->preferred_volume))) {
    return err;
  }
  fread(&out->_reserved, 1, 10, fd);
  fread(&out->matrix_structure, 36, 1, fd);
  if ((err = qtff_read_32(fd, &out->preview_time))) {
    return err;
  }
  if ((err = qtff_read_32(fd, &out->preview_duration))) {
    return err;
  }
  if ((err = qtff_read_32(fd, &out->poster_time))) {
    return err;
  }
  if ((err = qtff_read_32(fd, &out->selection_time))) {
    return err;
  }
  if ((err = qtff_read_32(fd, &out->selection_duration))) {
    return err;
  }
  if ((err = qtff_read_32(fd, &out->current_time))) {
    return err;
  }
  if ((err = qtff_read_32(fd, &out->next_track_id))) {
    return err;
  }

  return QTFFErrorNone;
}

QTFFError qtff_read_clipping_region_atom(FILE *fd,
                                         QTFFClippingRegionAtom *out) {
  QTFFError err;
  if ((err = qtff_read_atom_header(fd, &out->header))) {
    return err;
  }
  if ((err = qtff_read_16(fd, &out->region_size))) {
    return err;
  }
  if ((err = qtff_read_64(fd, &out->region_boundary_box))) {
    return err;
  }
  fseek(fd, out->header.size - 18, SEEK_CUR);
  return QTFFErrorNone;
}

QTFFError qtff_read_clipping_atom(FILE *fd, QTFFClippingAtom *out) {
  QTFFError err;
  if ((err = qtff_read_atom_header(fd, &out->header))) {
    return err;
  }
  if ((err = qtff_read_clipping_region_atom(fd, &out->clipping_region))) {
    return err;
  }
  fseek(fd, out->header.size - 8, SEEK_CUR);
  return QTFFErrorNone;
}

QTFFError qtff_read_color_table_atom(FILE *fd, QTFFColorTableAtom *out) {
  QTFFError err;
  if ((err = qtff_read_atom_header(fd, &out->header))) {
    return err;
  }
  fseek(fd, out->header.size - 8, SEEK_CUR);
  return QTFFErrorNone;
}

QTFFError qtff_read_user_data_atom(FILE *fd, QTFFUserDataAtom *out) {
  QTFFError err;
  if ((err = qtff_read_atom_header(fd, &out->header))) {
    return err;
  }
  fseek(fd, out->header.size - 8, SEEK_CUR);
  return QTFFErrorNone;
}

QTFFError qtff_read_track_atom(FILE *fd, QTFFTrackAtom *out) {
  QTFFError err;
  if ((err = qtff_read_atom_header(fd, &out->header))) {
    return err;
  }
  fseek(fd, out->header.size - 8, SEEK_CUR);
  return QTFFErrorNone;
}

QTFFError qtff_read_movie_atom(FILE *fd, QTFFMovieAtom *out) {
  QTFFError err;
  QTFFAtomHeader atom;
  QTFFMovieAtom movie = {0};

  // read header
  if ((err = qtff_read_atom_header(fd, &movie.header))) {
    return err;
  }

  // read child atoms
  while (!(err = qtff_read_atom_header(fd, &atom))) {
    fseek(fd, -8, SEEK_CUR);

    switch (QTFF_ATOM_ID(atom.type)) {
      /* case QTFF_ATOM_ID("mvhd"): */
      case 0x6d766864:
        if (movie.movie_header.header.size) {
          return QTFFErrorTooManyAtoms;
        }
        QTFFMovieHeaderAtom movie_header;
        if ((err = qtff_read_movie_header_atom(fd, &movie_header))) {
          return err;
        }
        movie.movie_header = movie_header;
        break;

      /* case QTFF_ATOM_ID("clip"): */
      case 0x636c6970:
        if (movie.clipping.header.size) {
          return QTFFErrorTooManyAtoms;
        }
        QTFFClippingAtom clipping;
        if ((err = qtff_read_clipping_atom(fd, &clipping))) {
          return err;
        }
        movie.clipping = clipping;
        break;

      /* case QTFF_ATOM_ID("trak"): */
      case 0x7472616b:
        if (movie.track_count >= QTFF_MAX_TRACK_ATOMS) {
          return QTFFErrorTooManyAtoms;
        }
        QTFFTrackAtom track;
        if ((err = qtff_read_track_atom(fd, &track))) {
          return err;
        }
        movie.track[movie.track_count] = track;
        movie.track_count++;
        break;

      /* case QTFF_ATOM_ID("udta"): */
      case 0x75647461:
        if (movie.user_data.header.size) {
          return QTFFErrorTooManyAtoms;
        }
        QTFFUserDataAtom user_data;
        if ((err = qtff_read_user_data_atom(fd, &user_data))) {
          return err;
        }
        movie.user_data = user_data;
        break;

      /* case QTFF_ATOM_ID("ctab"): */
      case 0x63746162:
        if (movie.color_table.header.size) {
          return QTFFErrorTooManyAtoms;
        }
        QTFFColorTableAtom color_table;
        if ((err = qtff_read_color_table_atom(fd, &color_table))) {
          return err;
        }
        movie.color_table = color_table;
        break;

      default:
        // unrecognised atom type - skip as per spec
        fseek(fd, atom.size, SEEK_CUR);
        break;
    }
  }
  if (err != QTFFErrorEOF) {
    return err;
  }

  *out = movie;
  return QTFFErrorNone;
}

QTFFError qtff_read_movie_file(FILE *fd, QTFFMovieFile *out) {
  QTFFMovieFile movie = {0};
  QTFFError err;
  QTFFAtomHeader atom;
  rewind(fd);

  while (!(err = qtff_read_atom_header(fd, &atom))) {
    fseek(fd, -8, SEEK_CUR);

    switch (QTFF_ATOM_ID(atom.type)) {
      /* case QTFF_ATOM_ID("ftyp"): */
      case 0x66747970:
        if (movie.file_type_compatibility_count >=
            QTFF_MAX_FILE_TYPE_COMPATIBILITY_ATOMS) {
          return QTFFErrorTooManyAtoms;
        }
        QTFFFileTypeCompatibilityAtom file_type_compatibility_atom;
        if ((err = qtff_read_file_type_compatibility_atom(
                 fd, &file_type_compatibility_atom))) {
          return err;
        };
        movie.file_type_compatibility[movie.file_type_compatibility_count] =
            file_type_compatibility_atom;
        movie.file_type_compatibility_count++;

        break;

      /* case QTFF_ATOM_ID("moov"): */
      case 0x6d6f6f76:
        if (movie.movie_count >= QTFF_MAX_MOVIE_ATOMS) {
          return QTFFErrorTooManyAtoms;
        }
        QTFFMovieAtom movie_atom;
        if ((err = qtff_read_movie_atom(fd, &movie_atom))) {
          return err;
        }
        movie.movie[movie.movie_count] = movie_atom;
        movie.movie_count++;
        break;

      /* case QTFF_ATOM_ID("mdat"): */
      case 0x6d646174:
        if (movie.movie_data_count >= QTFF_MAX_MOVIE_DATA_ATOMS) {
          return QTFFErrorTooManyAtoms;
        }
        QTFFMovieDataAtom movie_data_atom;
        if ((err = qtff_read_movie_data_atom(fd, &movie_data_atom))) {
          return err;
        }
        movie.movie_data[movie.movie_count] = movie_data_atom;
        movie.movie_data_count++;
        break;

      /* case QTFF_ATOM_ID("free"): */
      case 0x66726565:
        if (movie.free_count >= QTFF_MAX_FREE_ATOMS) {
          return QTFFErrorTooManyAtoms;
        }
        QTFFFreeAtom free_atom;
        if ((err = qtff_read_free_atom(fd, &free_atom))) {
          return err;
        }
        movie.free[movie.movie_count] = free_atom;
        movie.free_count++;
        break;

      /* case QTFF_ATOM_ID("skip"): */
      case 0x736b6970:
        if (movie.skip_count >= QTFF_MAX_SKIP_ATOMS) {
          return QTFFErrorTooManyAtoms;
        }
        QTFFSkipAtom skip_atom;
        if ((err = qtff_read_skip_atom(fd, &skip_atom))) {
          return err;
        }
        movie.skip[movie.movie_count] = skip_atom;
        movie.skip_count++;
        break;

      /* case QTFF_ATOM_ID("wide"): */
      case 0x77696465:
        if (movie.wide_count >= QTFF_MAX_WIDE_ATOMS) {
          return QTFFErrorTooManyAtoms;
        }
        QTFFWideAtom wide_atom;
        if ((err = qtff_read_wide_atom(fd, &wide_atom))) {
          return err;
        }
        movie.wide[movie.movie_count] = wide_atom;
        movie.wide_count++;
        break;

      /* case QTFF_ATOM_ID("pnot"): */
      case 0x706e6f74:
        if (movie.preview_count >= QTFF_MAX_WIDE_ATOMS) {
          return QTFFErrorTooManyAtoms;
        }
        QTFFPreviewAtom preview_atom;
        if ((err = qtff_read_preview_atom(fd, &preview_atom))) {
          return err;
        }
        movie.preview[movie.preview_count] = preview_atom;
        movie.preview_count++;
        break;

      default:
        // unsupported basic type - skip as per spec
        fseek(fd, atom.size, SEEK_CUR);
        break;
    }
  }
  if (err != QTFFErrorEOF) {
    return err;
  }

  *out = movie;
  return QTFFErrorNone;
}

/* QTFFError qtff_write_movie_file(FILE *fd, QTFFMovieFile *in) { */
/*   qtff_write_file_type_compatibility(fd, &in->file_type_compatibility); */
/*   qtff_write_movie(fd, &in->movie); */
/* } */
