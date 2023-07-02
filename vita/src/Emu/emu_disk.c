#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "emu/emu.h"

int Emu_HasDiskControl()
{
    return (core_disk_control_ext_callback != NULL);
}

int Emu_DiskSetEjectState(int ejected)
{
    if (!core_disk_control_ext_callback || !core_disk_control_ext_callback->set_eject_state)
        return -1;
    
    if (!core_disk_control_ext_callback->set_eject_state(ejected))
        return -1;
    
    return 0;
}

int Emu_DiskGetEjectState()
{
    if (!core_disk_control_ext_callback || !core_disk_control_ext_callback->get_eject_state)
        return 0;
    
    return core_disk_control_ext_callback->get_eject_state();
}

int Emu_DiskGetImageIndex()
{
    if (!core_disk_control_ext_callback || !core_disk_control_ext_callback->get_image_index)
        return 0;
    
    return core_disk_control_ext_callback->get_image_index();
}

int Emu_DiskSetImageIndex(unsigned index)
{
    if (!core_disk_control_ext_callback || !core_disk_control_ext_callback->set_image_index)
        return -1;
    
    if (!core_disk_control_ext_callback->set_image_index(index))
        return -1;
    
    return 0;
}

int Emu_DiskGetNumImages()
{
    if (!core_disk_control_ext_callback || !core_disk_control_ext_callback->get_num_images)
        return 0;
    
    return core_disk_control_ext_callback->get_num_images();
}

int Emu_DiskReplaceImageIndex(unsigned index, const struct retro_game_info *info)
{
    if (!core_disk_control_ext_callback || !core_disk_control_ext_callback->replace_image_index)
        return -1;
    
    if (!core_disk_control_ext_callback->replace_image_index(index, info))
        return -1;
    
    return 0;
}

int Emu_DiskAddImageIndex()
{
    if (!core_disk_control_ext_callback || !core_disk_control_ext_callback->add_image_index)
        return -1;
    
    if (!core_disk_control_ext_callback->add_image_index())
        return -1;
    
    return 0;
}

int Emu_DiskSetInitialImage(unsigned index, const char *path)
{
    if (!core_disk_control_ext_callback || !core_disk_control_ext_callback->set_initial_image)
        return -1;
    
    if (!core_disk_control_ext_callback->set_initial_image(index, path))
        return -1;
    
    return 0;
}

int Emu_DiskGetImagePath(unsigned index, char *path, size_t len)
{
    if (!core_disk_control_ext_callback || !core_disk_control_ext_callback->get_image_path)
        return -1;
    
    if (!core_disk_control_ext_callback->get_image_path(index, path, len))
        return -1;
    
    return 0;
}

int Emu_DiskGetImageLabel(unsigned index, char *label, size_t len)
{
    if (!core_disk_control_ext_callback || !core_disk_control_ext_callback->get_image_label)
        return -1;
    
    if (!core_disk_control_ext_callback->get_image_label(index, label, len))
        return -1;
    
    return 0;
}

void printDiskInfo()
{
    if (!core_disk_control_ext_callback)
        return;
    
    int eject_state = Emu_DiskGetEjectState();
    int image_index = Emu_DiskGetImageIndex();
    int num_images = Emu_DiskGetNumImages();

    printf("[DISK] eject_state = %d, image_index = %d, num_images = %d\n", eject_state, image_index, num_images);
}

int Emu_DiskChangeImageIndex(int index)
{
    if (!Emu_HasDiskControl())
        return -1;
    
    if (!Emu_DiskGetEjectState())
    {
        Emu_DiskSetEjectState(1);
        retro_run();
    }
    Emu_DiskSetImageIndex(index);
    Emu_DiskSetEjectState(0);
    
    return 0;
}
