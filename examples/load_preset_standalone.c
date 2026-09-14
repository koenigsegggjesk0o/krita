/*
 * SPDX-FileCopyrightText: 2026 Krita Brush Engine Extraction Contributors
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * load_preset_standalone.c — pure-C, zero-dependency reader for the
 * Krita .kpp preset container format.
 *
 * ACTUAL .kpp FORMAT (verified from upstream kis_paintop_preset.cpp
 * and binary inspection of the shipped files):
 *
 *   A .kpp is a standard PNG file. The brush settings XML and a version
 *   tag are stored INSIDE the PNG as text chunks:
 *
 *     - tEXt chunk  key="version"  value="2.2" or "5.0"
 *     - zTXt chunk  key="preset"   value = zlib-compressed XML settings
 *
 *   KisPaintOpPreset::loadFromDevice() reads these via QImageReader::text()
 *   and QDomDocument. The PNG pixel data doubles as the preset thumbnail.
 *
 * This standalone reader parses the PNG chunks directly (no Qt needed),
 * extracts the "version" and "preset" text chunks, inflates the preset
 * data, and writes the thumbnail PNG + settings XML to disk. It lets you
 * verify that every .kpp in brush-presets/ is intact and readable.
 *
 * Build:
 *   cc -std=c99 -O2 -Wall load_preset_standalone.c -o load_preset_standalone -lz
 *
 * Usage:
 *   ./load_preset_standalone <preset.kpp> [out_thumb.png] [out_settings.xml]
 *   (defaults: ./thumbnail.png, ./settings.xml)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <zlib.h>

static const unsigned char PNG_SIG[8] = {
    0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A
};

static uint32_t be32(const unsigned char *p) {
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16)
         | ((uint32_t)p[2] << 8)  |  (uint32_t)p[3];
}

/* State for collecting PNG text chunks. */
typedef struct {
    char  key[256];
    int   key_len;
    unsigned char *val;
    long  val_len;
    long  val_cap;
    int   is_ztxt;       /* 1 if the value is zlib-compressed (zTXt) */
} text_chunk_t;

static int text_append(text_chunk_t *t, const unsigned char *src, long n) {
    if (t->val_len + n > t->val_cap) {
        long cap = t->val_cap ? t->val_cap * 2 : 4096;
        while (cap < t->val_len + n) cap *= 2;
        unsigned char *nb = (unsigned char *)realloc(t->val, cap);
        if (!nb) return -1;
        t->val = nb;
        t->val_cap = cap;
    }
    memcpy(t->val + t->val_len, src, n);
    t->val_len += n;
    return 0;
}

/* zlib-inflate a raw deflate stream (zTXt uses zlib-format deflate,
 * i.e. a 2-byte zlib header + deflate + 4-byte adler32). */
static long inflate_zlib(const unsigned char *src, long srclen,
                         unsigned char **out) {
    z_stream zs;
    memset(&zs, 0, sizeof(zs));
    if (inflateInit(&zs) != Z_OK) { fprintf(stderr, "inflateInit failed\n"); return -1; }
    zs.next_in   = (Bytef *)src;
    zs.avail_in  = (uInt)srclen;
    long cap = 1 << 16, used = 0;
    unsigned char *buf = (unsigned char *)malloc(cap);
    if (!buf) { inflateEnd(&zs); return -1; }
    int ret;
    do {
        if (used == cap) {
            cap *= 2;
            unsigned char *nb = (unsigned char *)realloc(buf, cap);
            if (!nb) { free(buf); inflateEnd(&zs); return -1; }
            buf = nb;
        }
        zs.next_out  = (Bytef *)(buf + used);
        zs.avail_out = (uInt)(cap - used);
        ret = inflate(&zs, Z_NO_FLUSH);
        if (ret == Z_STREAM_ERROR || ret == Z_NEED_DICT ||
            ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
            fprintf(stderr, "inflate error: %d\n", ret);
            free(buf); inflateEnd(&zs); return -1;
        }
        used += (cap - used) - zs.avail_out;
    } while (ret != Z_STREAM_END);
    inflateEnd(&zs);
    *out = buf;
    return used;
}

static long slurp(const char *path, unsigned char **out) {
    FILE *f = fopen(path, "rb");
    if (!f) { perror(path); return -1; }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char *buf = (unsigned char *)malloc(size);
    if (!buf) { fclose(f); return -1; }
    if (fread(buf, 1, size, f) != (size_t)size) {
        free(buf); fclose(f); return -1;
    }
    fclose(f);
    *out = buf;
    return size;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr,
            "usage: %s <preset.kpp> [out_thumb.png] [out_settings.xml]\n"
            "  A .kpp is a PNG whose text chunks hold the brush settings:\n"
            "    tEXt version = \"2.2\"|\"5.0\"\n"
            "    zTXt preset  = zlib-compressed XML settings\n",
            argv[0]);
        return 1;
    }
    const char *in      = argv[1];
    const char *thumb_p = argc > 2 ? argv[2] : "thumbnail.png";
    const char *xml_p   = argc > 3 ? argv[3] : "settings.xml";

    unsigned char *all = NULL;
    long size = slurp(in, &all);
    if (size < 0) return 2;

    if (size < 8 || memcmp(all, PNG_SIG, 8) != 0) {
        fprintf(stderr, "%s: not a .kpp (no PNG signature)\n", in);
        free(all);
        return 3;
    }

    /* Walk PNG chunks. The whole PNG is the thumbnail; collect text chunks. */
    text_chunk_t tc_version = {0}, tc_preset = {0};
    tc_preset.is_ztxt = 1; /* will be set when we see a zTXt */

    long pos = 8;
    long thumbnail_end = size; /* the whole file IS the thumbnail PNG */
    int saw_iend = 0;
    while (pos + 12 <= size) {
        uint32_t length = be32(all + pos);
        char type[5];
        memcpy(type, all + pos + 4, 4);
        type[4] = 0;
        long data_off = pos + 8;
        if (data_off + length + 4 > size) break; /* truncated */

        if (strcmp(type, "tEXt") == 0) {
            /* tEXt: keyword \0 text */
            const unsigned char *d = all + data_off;
            long klen = 0;
            while (klen < (long)length && d[klen] != 0) klen++;
            if (klen < (long)length) {
                if (klen < sizeof(tc_version.key)) {
                    memcpy(tc_version.key, d, klen);
                    tc_version.key[klen] = 0;
                    tc_version.key_len = (int)klen;
                    text_append(&tc_version, d + klen + 1, length - klen - 1);
                }
            }
        } else if (strcmp(type, "zTXt") == 0) {
            /* zTXt: keyword \0 compression_method(1 byte) compressed_text */
            const unsigned char *d = all + data_off;
            long klen = 0;
            while (klen < (long)length && d[klen] != 0) klen++;
            if (klen + 2 <= (long)length) {
                if (klen < sizeof(tc_preset.key)) {
                    memcpy(tc_preset.key, d, klen);
                    tc_preset.key[klen] = 0;
                    tc_preset.key_len = (int)klen;
                }
                /* compression_method is d[klen+1]; only 0 (zlib) is defined */
                text_append(&tc_preset, d + klen + 2, length - klen - 2);
            }
        } else if (strcmp(type, "IEND") == 0) {
            /* end of PNG — but in a .kpp the PNG IS the whole file */
            saw_iend = 1;
            thumbnail_end = data_off + length + 4; /* after IEND+CRC */
            pos = data_off + length + 4;
            /* keep walking in case there's trailing data, though upstream
             * stores everything inside the PNG chunks. */
            continue;
        }

        pos = data_off + length + 4; /* data + CRC */
    }

    /* 1) write the thumbnail PNG (the whole .kpp is a valid PNG) */
    FILE *ft = fopen(thumb_p, "wb");
    if (!ft) { perror(thumb_p); free(all); return 5; }
    long thumb_size = saw_iend ? thumbnail_end : size;
    fwrite(all, 1, (size_t)thumb_size, ft);
    fclose(ft);
    printf("thumbnail PNG: %ld bytes -> %s\n", thumb_size, thumb_p);

    printf("version text chunk: key=\"%s\" value=\"%.*s\"\n",
        tc_version.key_len ? tc_version.key : "(none)",
        (int)tc_version.val_len, tc_version.val ? (char *)tc_version.val : "");

    /* 2) inflate + write the preset XML */
    if (tc_preset.key_len == 0) {
        printf("preset zTXt chunk: NOT FOUND (this .kpp has no embedded settings)\n");
        free(all);
        free(tc_version.val);
        free(tc_preset.val);
        return 0;
    }
    printf("preset zTXt chunk: key=\"%s\" compressed=%ld bytes\n",
           tc_preset.key, tc_preset.val_len);

    unsigned char *xml = NULL;
    long xmlsize = inflate_zlib(tc_preset.val, tc_preset.val_len, &xml);
    free(tc_preset.val);
    free(tc_version.val);
    free(all);
    if (xmlsize < 0) return 6;

    FILE *fx = fopen(xml_p, "wb");
    if (!fx) { perror(xml_p); free(xml); return 7; }
    fwrite(xml, 1, (size_t)xmlsize, fx);
    fclose(fx);
    printf("preset XML: %ld bytes -> %s\n", xmlsize, xml_p);

    /* peek: print the first <paintop ...> engine id */
    for (long i = 0; i + 7 <= xmlsize; i++) {
        if (memcmp(xml + i, "paintop", 7) == 0) {
            long ctx = xmlsize - i;
            long n = ctx < 80 ? ctx : 80;
            printf("engine hint (first 80 bytes at 'paintop'): %.*s\n",
                   (int)n, (const char *)(xml + i));
            break;
        }
    }
    free(xml);
    return 0;
}
