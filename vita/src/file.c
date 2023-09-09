#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

#include <psp2/io/devctl.h>
#include <psp2/io/dirent.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>
#include <png.h>

#include "file.h"
#include "utils.h"

#define SCE_ERROR_ERRNO_EEXIST 0x80010011
#define SCE_ERROR_ERRNO_ENODEV 0x80010013

char **file_valid_exts = NULL;

int IsValidFile(char *path)
{
    if (!file_valid_exts)
        return 0;

    char *ext = strrchr(path, '.');
    if (!ext++)
        return 0;

    int i = 0;
    while (file_valid_exts[i])
    {
        if (strcasecmp(ext, file_valid_exts[i]) == 0)
            return 1;
        i++;
    }

    return 0;
}

int MakeBaseDirectoryEx(char *base, int base_size, const char *path, int path_len)
{
    if (!base || base_size <= 0)
        return -1;
    base[0] = '\0';

    if (!path || path_len <= 0)
        return -1;

    if ((path_len == 1 && path[0] == '/') || path[path_len - 1] == ':')
    {
        if (base_size > path_len)
        {
            strncpy(base, path, path_len);
            base[path_len] = '\0';
        }
        return -1;
    }

    if (path[path_len - 1] == '/')
        path_len--;

    int sep = -1;

    int i;
    for (i = path_len - 1; i >= 0; i--)
    {
        if (path[i] == '/' || path[i] == ':')
        {
            sep = i;
            break;
        }
    }
    if (sep == -1)
        return -1;

    int new_len = sep + 1;
    if (new_len > base_size - 1)
        new_len = base_size - 1;
    if (new_len > 0)
        strncpy(base, path, new_len);
    base[new_len] = '\0';

    return 0;
}

int MakeFileNameEx(char *name, int name_size, const char *path, int path_len)
{
    if (!name || name_size <= 0)
        return -1;
    name[0] = '\0';

    if (!path || path_len <= 0)
        return -1;

    if (path[path_len - 1] == '/' || path[path_len - 1] == ':')
        return -1;

    int sep = -1;

    int i;
    for (i = path_len - 1; i >= 0; i--)
    {
        if (path[i] == '/' || path[i] == ':')
        {
            sep = i;
            break;
        }
    }
    if (sep == -1)
        return -1;

    int new_len = path_len - (sep + 1);
    if (new_len > name_size - 1)
        new_len = name_size - 1;
    if (new_len > 0)
        strncpy(name, path + (sep + 1), new_len);
    name[new_len] = '\0';

    return 0;
}

int MakeBaseNameEx(char *name, int name_size, const char *path, int path_len)
{
    if (!name || name_size <= 0)
        return -1;
    name[0] = '\0';

    if (!path || path_len <= 0)
        return -1;

    int head = -1;
    int sep = path_len;

    int i;
    for (i = path_len - 1; i >= 0; i--)
    {
        if (path[i] == '/' || path[i] == ':')
        {
            head = i;
            break;
        }
        else if (path[i] == '.' && sep == path_len)
        {
            sep = i;
        }
    }

    int new_len = sep - (head + 1);
    if (new_len > name_size - 1)
        new_len = name_size - 1;
    if (new_len > 0)
        strncpy(name, path + (head + 1), new_len);
    name[new_len] = '\0';

    return 0;
}

int MakeBaseDirectory(char *base_dir, const char *path, int size)
{
    return MakeBaseDirectoryEx(base_dir, size, path, strlen(path));
}

int MakeFilename(char *name, const char *path, int size)
{
    return MakeFileNameEx(name, size, path, strlen(path));
}

int MakeBaseName(char *name, const char *path, int size)
{
    return MakeBaseNameEx(name, size, path, strlen(path));
}

int ReadFile(const char *file, void *buf, int size)
{
    SceUID fd = sceIoOpen(file, SCE_O_RDONLY, 0);
    if (fd < 0)
        return fd;

    int read = sceIoRead(fd, buf, size);

    sceIoClose(fd);
    return read;
}

int WriteFile(const char *file, const void *buf, int size)
{
    SceUID fd = sceIoOpen(file, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0777);
    if (fd < 0)
        return fd;

    int written = sceIoWrite(fd, buf, size);

    sceIoClose(fd);
    return written;
}

int AllocateReadFile(const char *file, void **buffer)
{
    SceUID fd = sceIoOpen(file, SCE_O_RDONLY, 0);
    if (fd < 0)
        return fd;

    int size = sceIoLseek32(fd, 0, SCE_SEEK_END);
    sceIoLseek32(fd, 0, SCE_SEEK_SET);

    *buffer = malloc(size);
    if (!*buffer)
    {
        sceIoClose(fd);
        return -1;
    }

    int read = sceIoRead(fd, *buffer, size);
    sceIoClose(fd);

    return read;
}

int GetFileSize(const char *file)
{
    SceUID fd = sceIoOpen(file, SCE_O_RDONLY, 0);
    if (fd < 0)
        return fd;

    int fileSize = sceIoLseek(fd, 0, SCE_SEEK_END);

    sceIoClose(fd);
    return fileSize;
}

int CheckFileExist(const char *file)
{
    SceUID fd = sceIoOpen(file, SCE_O_RDONLY, 0);
    if (fd < 0)
        return 0;

    sceIoClose(fd);
    return 1;
}

int CheckFolderExist(const char *folder)
{
    SceUID dfd = sceIoDopen(folder);
    if (dfd < 0)
        return 0;

    sceIoDclose(dfd);
    return 1;
}

int CreateFolder(const char *path)
{
    if (CheckFolderExist(path))
        return 0;

    int len = strlen(path);
    if (len <= 0)
        return -1;

    char *buf = malloc(len + 1);
    strcpy(buf, path);

    char ch;
    int i;
    for (i = 0; i < len; i++)
    {
        ch = buf[i];
        if (ch == '/')
        {
            buf[i] = '\0';
            sceIoMkdir(buf, 0777);
            buf[i] = ch;
        }
    }

    free(buf);
    return sceIoMkdir(path, 0777);
}

int RemovePath(const char *path)
{
    SceUID dfd = sceIoDopen(path);
    if (dfd >= 0)
    {
        int res = 0;

        do
        {
            SceIoDirent dir;
            memset(&dir, 0, sizeof(SceIoDirent));

            res = sceIoDread(dfd, &dir);
            if (res > 0)
            {
                char *new_path = malloc(strlen(path) + strlen(dir.d_name) + 2);
                snprintf(new_path, MAX_PATH_LENGTH, "%s%s%s", path, HasEndSlash(path) ? "" : "/", dir.d_name);

                if (SCE_S_ISDIR(dir.d_stat.st_mode))
                {
                    int ret = RemovePath(new_path);
                    if (ret <= 0)
                    {
                        free(new_path);
                        sceIoDclose(dfd);
                        return ret;
                    }
                }
                else
                {
                    int ret = sceIoRemove(new_path);
                    if (ret < 0)
                    {
                        free(new_path);
                        sceIoDclose(dfd);
                        return ret;
                    }
                }

                free(new_path);
            }
        } while (res > 0);

        sceIoDclose(dfd);

        int ret = sceIoRmdir(path);
        if (ret < 0)
            return ret;
    }
    else
    {
        int ret = sceIoRemove(path);
        if (ret < 0)
            return ret;
    }

    return 1;
}

int CopyFile(const char *src_path, const char *dst_path)
{
    // The source and destination paths are identical
    if (strcasecmp(src_path, dst_path) == 0)
    {
        return -1;
    }

    // The destination is a subfolder of the source folder
    int len = strlen(src_path);
    if (strncasecmp(src_path, dst_path, len) == 0 && (dst_path[len] == '/' || dst_path[len - 1] == '/'))
    {
        return -1;
    }

    SceUID fdsrc = sceIoOpen(src_path, SCE_O_RDONLY, 0);
    if (fdsrc < 0)
        return fdsrc;

    SceUID fddst = sceIoOpen(dst_path, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0777);
    if (fddst < 0)
    {
        sceIoClose(fdsrc);
        return fddst;
    }

    void *buf = memalign(4096, TRANSFER_SIZE);

    while (1)
    {
        int read = sceIoRead(fdsrc, buf, TRANSFER_SIZE);

        if (read < 0)
        {
            free(buf);

            sceIoClose(fddst);
            sceIoClose(fdsrc);

            sceIoRemove(dst_path);

            return read;
        }

        if (read == 0)
            break;

        int written = sceIoWrite(fddst, buf, read);

        if (written < 0)
        {
            free(buf);

            sceIoClose(fddst);
            sceIoClose(fdsrc);

            sceIoRemove(dst_path);

            return written;
        }
    }

    free(buf);

    // Inherit file stat
    SceIoStat stat;
    memset(&stat, 0, sizeof(SceIoStat));
    sceIoGetstatByFd(fdsrc, &stat);
    sceIoChstatByFd(fddst, &stat, 0x3B);

    sceIoClose(fddst);
    sceIoClose(fdsrc);

    return 1;
}

int CopyPath(const char *src_path, const char *dst_path)
{
    // The source and destination paths are identical
    if (strcasecmp(src_path, dst_path) == 0)
    {
        return -1;
    }

    // The destination is a subfolder of the source folder
    int len = strlen(src_path);
    if (strncasecmp(src_path, dst_path, len) == 0 && (dst_path[len] == '/' || dst_path[len - 1] == '/'))
    {
        return -1;
    }

    SceUID dfd = sceIoDopen(src_path);
    if (dfd >= 0)
    {
        SceIoStat stat;
        memset(&stat, 0, sizeof(SceIoStat));
        sceIoGetstatByFd(dfd, &stat);

        stat.st_mode |= SCE_S_IWUSR;

        int ret = sceIoMkdir(dst_path, stat.st_mode & 0xFFF);
        if (ret < 0 && ret != SCE_ERROR_ERRNO_EEXIST)
        {
            sceIoDclose(dfd);
            return ret;
        }

        if (ret == SCE_ERROR_ERRNO_EEXIST)
        {
            sceIoChstat(dst_path, &stat, 0x3B);
        }

        int res = 0;

        do
        {
            SceIoDirent dir;
            memset(&dir, 0, sizeof(SceIoDirent));

            res = sceIoDread(dfd, &dir);
            if (res > 0)
            {
                char *new_src_path = malloc(strlen(src_path) + strlen(dir.d_name) + 2);
                snprintf(new_src_path, MAX_PATH_LENGTH, "%s%s%s", src_path, HasEndSlash(src_path) ? "" : "/", dir.d_name);

                char *new_dst_path = malloc(strlen(dst_path) + strlen(dir.d_name) + 2);
                snprintf(new_dst_path, MAX_PATH_LENGTH, "%s%s%s", dst_path, HasEndSlash(dst_path) ? "" : "/", dir.d_name);

                if (SCE_S_ISDIR(dir.d_stat.st_mode))
                {
                    ret = CopyPath(new_src_path, new_dst_path);
                }
                else
                {
                    ret = CopyFile(new_src_path, new_dst_path);
                }

                free(new_dst_path);
                free(new_src_path);

                if (ret <= 0)
                {
                    sceIoDclose(dfd);
                    return ret;
                }
            }
        } while (res > 0);

        sceIoDclose(dfd);
    }
    else
    {
        return CopyFile(src_path, dst_path);
    }

    return 1;
}

int MovePath(const char *src_path, const char *dst_path)
{
    // The source and destination paths are identical
    if (strcasecmp(src_path, dst_path) == 0)
    {
        return -1;
    }

    // The destination is a subfolder of the source folder
    int len = strlen(src_path);
    if (strncasecmp(src_path, dst_path, len) == 0 && (dst_path[len] == '/' || dst_path[len - 1] == '/'))
    {
        return -1;
    }

    int ret = sceIoRename(src_path, dst_path);

    if (ret < 0)
    {
        if (ret != SCE_ERROR_ERRNO_EEXIST)
            return ret;

        int res;

        // Src stat
        SceIoStat src_stat;
        memset(&src_stat, 0, sizeof(SceIoStat));
        res = sceIoGetstat(src_path, &src_stat);
        if (res < 0)
            return res;

        // Dst stat
        SceIoStat dst_stat;
        memset(&dst_stat, 0, sizeof(SceIoStat));
        res = sceIoGetstat(dst_path, &dst_stat);
        if (res < 0)
            return res;

        // Is dir
        int src_is_dir = SCE_S_ISDIR(src_stat.st_mode);
        int dst_is_dir = SCE_S_ISDIR(dst_stat.st_mode);

        // One of them is a file and the other a directory, no replacement or integration possible
        if (src_is_dir != dst_is_dir)
            return ret;

        if (!src_is_dir && !dst_is_dir) // Replace file
        {
            sceIoRemove(dst_path);

            res = sceIoRename(src_path, dst_path);
            if (res < 0)
                return res;

            return 1;
        }
        else if (src_is_dir && dst_is_dir) // Integrate directory
        {
            SceUID dfd = sceIoDopen(src_path);
            if (dfd < 0)
                return dfd;

            do
            {
                SceIoDirent dir;
                memset(&dir, 0, sizeof(SceIoDirent));

                res = sceIoDread(dfd, &dir);
                if (res > 0)
                {
                    char *new_src_path = malloc(strlen(src_path) + strlen(dir.d_name) + 2);
                    snprintf(new_src_path, MAX_PATH_LENGTH, "%s%s%s", src_path, HasEndSlash(src_path) ? "" : "/", dir.d_name);

                    char *new_dst_path = malloc(strlen(dst_path) + strlen(dir.d_name) + 2);
                    snprintf(new_dst_path, MAX_PATH_LENGTH, "%s%s%s", dst_path, HasEndSlash(dst_path) ? "" : "/", dir.d_name);

                    // Recursive move
                    ret = MovePath(new_src_path, new_dst_path);

                    free(new_dst_path);
                    free(new_src_path);

                    if (ret <= 0)
                    {
                        sceIoDclose(dfd);
                        return ret;
                    }
                }
            } while (res > 0);

            sceIoDclose(dfd);

            // Integrated, now remove this directory
            sceIoRmdir(src_path);
        }
    }

    return 1;
}

int WritePngFile(const char *path, unsigned char *pixels, int width, int height, int bit_depth)
{
    FILE *png_file = fopen(path, "wb");
    if (!png_file)
    {
        return -1;
    }

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL)
    {
        fclose(png_file);
        return -1;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        png_destroy_write_struct(&png_ptr, NULL);
        return -1;
    }

    png_init_io(png_ptr, png_file);
    png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, PNG_COLOR_TYPE_RGB_ALPHA,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);
    png_set_packing(png_ptr);

    png_bytepp row_ptr = (png_bytepp)png_malloc(png_ptr, height * sizeof(png_bytep));
    int i;
    for (i = 0; i < height; i++)
    {
        row_ptr[i] = (png_bytep)(pixels + i * width * 4);
    }

    png_write_image(png_ptr, row_ptr);
    png_write_end(png_ptr, info_ptr);
    png_free(png_ptr, row_ptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(png_file);

    return 0;
}
