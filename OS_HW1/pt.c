#include "os.h"
#include <stdio.h>

#define VALID (1)
#define INVALID (0)
#define NUMBER_OF_LEVELS (5)
#define BITS_0_44_MASK (0x00001fffffffffffu)
#define BITS_9_17_MASK (0x000000000003fe00u)
#define BITS_18_26_MASK (0x0000000007fc0000u)
#define BITS_27_35_MASK (0x0000000ff8000000u)
#define BITS_36_44_MASK (0x00001ff000000000u)
#define FRAME_NUMBER_MASK (0xfffffffffffff000)

/****************************************************
*                                                   *
*           Operating Systems (0368-2162)           *
*        Multi-Level Page Tables Assignment         *
*                                                   *
*****************************************************/

uint64_t rotate_vpn(uint64_t vpn);
uint64_t check_if_valid(uint64_t entry);

void page_table_update(uint64_t pt, uint64_t vpn, uint64_t ppn) {
    uint64_t current_index, valid;

    uint64_t * current_node = phys_to_virt(pt << 12u);
    uint64_t rotated_vpn = rotate_vpn(vpn);

    for (int i = 1; i <= NUMBER_OF_LEVELS - 1; ++i){

        current_index = rotated_vpn & 0x1ffu;
        rotated_vpn = rotated_vpn >> 9u;
        valid = check_if_valid(current_node[current_index]);

        if (valid){
            current_node = phys_to_virt(current_node[current_index] & FRAME_NUMBER_MASK);
        }

        else{
            current_node[current_index] = (((alloc_page_frame() << 12u) & FRAME_NUMBER_MASK) | 0x1u);
            current_node = phys_to_virt(current_node[current_index] & FRAME_NUMBER_MASK);
        }
    }

    current_index = rotated_vpn & 0x1ffu;
    current_node[current_index] = (NO_MAPPING == ppn) ? ((ppn << 12u) & FRAME_NUMBER_MASK) : (((ppn << 12u) & FRAME_NUMBER_MASK) | 0x1u);

}

uint64_t page_table_query(uint64_t pt, uint64_t vpn){
    uint64_t current_index, valid;
    uint64_t * current_node = phys_to_virt(pt << 12u);
    uint64_t rotated_vpn = rotate_vpn(vpn);

    for (int i = 1; i <= NUMBER_OF_LEVELS; ++i){

        current_index = rotated_vpn & 0x1ffu;
        rotated_vpn = rotated_vpn >> 9u;
        valid = check_if_valid(current_node[current_index]);

        if (valid){
            if (NUMBER_OF_LEVELS == i) {    /* i.e we have reached the final level and the valid bit is on */
                return (current_node[current_index] >> 12u);
            }
            current_node = phys_to_virt(current_node[current_index] & FRAME_NUMBER_MASK);
        }

        else{return NO_MAPPING;}
    }
    return NO_MAPPING;
}


/* rotate the virtual page number so it can be shift left until its all zeros*/
uint64_t rotate_vpn(uint64_t vpn)
    {
    uint64_t rotated_vpn;
    vpn = vpn & BITS_0_44_MASK;

    rotated_vpn = vpn >> 36u;
    rotated_vpn = (((vpn >> 18u) & BITS_9_17_MASK) | rotated_vpn);
    rotated_vpn = ((vpn & BITS_18_26_MASK) | rotated_vpn);
    rotated_vpn = (((vpn << 18u) & BITS_27_35_MASK) | rotated_vpn);
    rotated_vpn = (((vpn << 36u) & BITS_36_44_MASK) | rotated_vpn);

    return rotated_vpn;
    }

uint64_t check_if_valid(uint64_t entry){
    if (entry & 0x1u)
        {return VALID;}
    return INVALID;
}
