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
static uint16_t qtff_read_u16(FILE *fd) {
  uint16_t no;
  fread(&no, 2, 1, fd);
  return qtff_ntohs(no);
}

// Read a single 32-bit unsigned integer from the current offset in the file.
// Error handling and EOF detection are done by ferror and feof respectively.
static uint32_t qtff_read_u32(FILE *fd) {
  uint32_t no;
  fread(&no, 4, 1, fd);
  return qtff_ntohl(no);
}

// @TODO: make this output something
// @TODO: handle special sizes:
//  0 => atom continues until end of file
//  1 => atoms length is given in 64-bit extended size field instead
QTFFError qtff_read_movie_file(FILE *fd, QTFFMovieFile *out) {
  QTFFMovieFile movie = {0};
  QTFFError err = QTFFErrorNone;
  QTFFAtomHeader atom;
  rewind(fd);

  while (!(err = qtff_read_atom_header(fd, &atom))) {
    switch (QTFF_ATOM_ID(atom.type)) {
      case QTFF_ATOM_ID("ftyp"):
        if (movie.file_type_compatibility_count >=
            QTFF_MAX_FILE_TYPE_COMPATIBILITY_ATOMS) {
          return QTFFErrorTooManyAtoms;
        }
        movie.file_type_compatibility[movie.file_type_compatibility_count] =
            *((QTFFFileTypeCompatibilityAtom *)&atom);
        movie.file_type_compatibility_count++;
        break;

      case QTFF_ATOM_ID("moov"):
        if (movie.movie_count >= QTFF_MAX_MOVIE_ATOMS) {
          return QTFFErrorTooManyAtoms;
        }
        movie.movie[movie.movie_count] = *((QTFFMovieAtom *)&atom);
        movie.movie_count++;
        break;

      case QTFF_ATOM_ID("mdat"):
        if (movie.movie_data_count >= QTFF_MAX_MOVIE_DATA_ATOMS) {
          return QTFFErrorTooManyAtoms;
        }
        movie.movie_data[movie.movie_data_count] =
            *((QTFFMovieDataAtom *)&atom);
        movie.movie_data_count++;
        break;

      case QTFF_ATOM_ID("free"):
        if (movie.free_count >= QTFF_MAX_FREE_ATOMS) {
          return QTFFErrorTooManyAtoms;
        }
        movie.free[movie.free_count] = *((QTFFFreeAtom *)&atom);
        movie.free_count++;
        break;

      case QTFF_ATOM_ID("skip"):
        if (movie.skip_count >= QTFF_MAX_SKIP_ATOMS) {
          return QTFFErrorTooManyAtoms;
        }
        movie.skip[movie.skip_count] = *((QTFFSkipAtom *)&atom);
        movie.skip_count++;
        break;

      case QTFF_ATOM_ID("wide"):
        if (movie.wide_count >= QTFF_MAX_WIDE_ATOMS) {
          return QTFFErrorTooManyAtoms;
        }
        movie.wide[movie.wide_count] = *((QTFFWideAtom *)&atom);
        movie.wide_count++;
        break;

      case QTFF_ATOM_ID("pnot"):
        printf("%c%c%c%c is a basic atom type.\n", atom.type[0], atom.type[1],
               atom.type[2], atom.type[3]);
        break;
      default:
        fprintf(stderr, "%c%c%c%c (%x) is not a basic atom type.\n",
                atom.type[0], atom.type[1], atom.type[2], atom.type[3],
                *((uint32_t *)atom.type));
        return QTFFErrorNotBasicAtomType;
    }
    fseek(fd, atom.size, SEEK_CUR);
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

QTFFError qtff_read_atom_header(FILE *fd, QTFFAtomHeader *out) {
  QTFFAtomHeader atom;
  atom.offset = ftell(fd);
  atom.size = qtff_read_u32(fd);
  fread(&atom.type, 4, 1, fd);

  if (feof(fd)) {
    return QTFFErrorEOF;
  }
  if (ferror(fd)) {
    return QTFFErrorIOError;
  }

  fseek(fd, -8, SEEK_CUR);
  *out = atom;
  return QTFFErrorNone;
}
