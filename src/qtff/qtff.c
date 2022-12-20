#include <stdio.h>

#include "qtff.h"

// Convert a number from network (big) endian to host endian.
// These must be implemented here as newlib does not provide the
// standard library implementations ntohs & ntohl (arpa/inet.h).
//
// taken from:
// https://stackoverflow.com/questions/2100331/macro-definition-to-determine-big-endian-or-little-endian-machine
static uint16_t qtff_ntohs(uint16_t n) {
  unsigned char *np = (unsigned char *)&n;

  return ((uint32_t)np[0] << 8) | (uint32_t)np[1];
}

static uint32_t qtff_ntohl(uint32_t n) {
  unsigned char *np = (unsigned char *)&n;

  return ((uint32_t)np[0] << 24) | ((uint32_t)np[1] << 16) |
         ((uint32_t)np[2] << 8) | (uint32_t)np[3];
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
  *(uint16_t *)out = qtff_ntohs(n);
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
  *(uint32_t *)out = qtff_ntohl(n);
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
  if ((err = qtff_read_atom_header(fd, &out->header))) {
    return err;
  }
  fseek(fd, out->header.size - 8, SEEK_CUR);
  return QTFFErrorNone;
}

QTFFError qtff_read_clipping_atom(FILE *fd, QTFFClippingAtom *out) {
  QTFFError err;
  if ((err = qtff_read_atom_header(fd, &out->header))) {
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

    printf("parsing %c%c%c%c atom with length %u\n", atom.type[0], atom.type[1],
           atom.type[2], atom.type[3], atom.size);

    switch (QTFF_ATOM_ID(atom.type)) {
      case QTFF_ATOM_ID("mvhd"):
        if (movie.movie_header.header.size) {
          return QTFFErrorTooManyAtoms;
        }
        QTFFMovieHeaderAtom movie_header;
        if ((err = qtff_read_movie_header_atom(fd, &movie_header))) {
          return err;
        }
        movie.movie_header = movie_header;
        break;

      case QTFF_ATOM_ID("clip"):
        if (movie.clipping.header.size) {
          return QTFFErrorTooManyAtoms;
        }
        QTFFClippingAtom clipping;
        if ((err = qtff_read_clipping_atom(fd, &clipping))) {
          return err;
        }
        movie.clipping = clipping;
        break;

      case QTFF_ATOM_ID("trak"):
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

      case QTFF_ATOM_ID("udta"):
        if (movie.user_data.header.size) {
          return QTFFErrorTooManyAtoms;
        }
        QTFFUserDataAtom user_data;
        if ((err = qtff_read_user_data_atom(fd, &user_data))) {
          return err;
        }
        movie.user_data = user_data;
        break;

      case QTFF_ATOM_ID("ctab"):
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
      case QTFF_ATOM_ID("ftyp"):
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

      case QTFF_ATOM_ID("moov"):
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

      case QTFF_ATOM_ID("mdat"):
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

      case QTFF_ATOM_ID("free"):
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

      case QTFF_ATOM_ID("skip"):
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

      case QTFF_ATOM_ID("wide"):
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

      case QTFF_ATOM_ID("pnot"):
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
