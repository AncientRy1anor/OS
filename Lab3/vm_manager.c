#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define PAGE_SIZE 256
#define PAGE_COUNT 256
#define FRAME_COUNT 256
#define TLB_SIZE 16

typedef struct {
    int page;
    int frame;
} TLBEntry;

static int tlb_lookup(TLBEntry tlb[], int page) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb[i].page == page) {
            return tlb[i].frame;
        }
    }
    return -1;
}

static void tlb_insert_fifo(TLBEntry tlb[], int page, int frame, int *next_idx) {
    tlb[*next_idx].page = page;
    tlb[*next_idx].frame = frame;
    *next_idx = (*next_idx + 1) % TLB_SIZE;
}

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s addresses.txt [BACKING_STORE.bin]\n", argv[0]);
        return 1;
    }

    const char *addresses_path = argv[1];
    const char *backing_path = (argc == 3) ? argv[2] : "BACKING_STORE.bin";

    FILE *addr_file = fopen(addresses_path, "r");
    if (!addr_file) {
        perror("Cannot open addresses file");
        return 1;
    }

    FILE *backing_file = fopen(backing_path, "rb");
    if (!backing_file) {
        perror("Cannot open BACKING_STORE file");
        fclose(addr_file);
        return 1;
    }

    signed char physical_memory[FRAME_COUNT][PAGE_SIZE];

    int page_table[PAGE_COUNT];
    for (int i = 0; i < PAGE_COUNT; i++) {
        page_table[i] = -1;
    }

    TLBEntry tlb[TLB_SIZE];
    for (int i = 0; i < TLB_SIZE; i++) {
        tlb[i].page = -1;
        tlb[i].frame = -1;
    }

    int tlb_next = 0;
    int next_free_frame = 0;
    int total_addresses = 0;
    int page_faults = 0;
    int tlb_hits = 0;
    int logical_input;

    while (fscanf(addr_file, "%d", &logical_input) == 1) {
        total_addresses++;

        uint16_t logical = (uint16_t)(logical_input & 0xFFFF);
        int page = (logical >> 8) & 0xFF;
        int offset = logical & 0xFF;

        int frame = tlb_lookup(tlb, page);

        if (frame != -1) {
            tlb_hits++;
        } else {
            frame = page_table[page];

            if (frame == -1) {
                long seek_pos = (long)page * PAGE_SIZE;
                if (fseek(backing_file, seek_pos, SEEK_SET) != 0) {
                    perror("fseek failed");
                    fclose(addr_file);
                    fclose(backing_file);
                    return 1;
                }

                if (fread(physical_memory[next_free_frame], 1, PAGE_SIZE, backing_file) != PAGE_SIZE) {
                    fprintf(stderr, "Failed to read full page from BACKING_STORE.\n");
                    fclose(addr_file);
                    fclose(backing_file);
                    return 1;
                }

                frame = next_free_frame;
                next_free_frame++;
                page_table[page] = frame;
                page_faults++;
            }

            tlb_insert_fifo(tlb, page, frame, &tlb_next);
        }

        int physical_address = frame * PAGE_SIZE + offset;
        signed char value = physical_memory[frame][offset];

        printf("Virtual address: %d Physical address: %d Value: %d\n",
               logical_input, physical_address, (int)value);
    }

    // Вывод статистики (указано в задании вывести в конце)
    double page_fault_rate = total_addresses ? (double)page_faults / total_addresses : 0.0;
    double tlb_hit_rate = total_addresses ? (double)tlb_hits / total_addresses : 0.0;

    printf("Page-fault rate: %.3f\n", page_fault_rate);
    printf("TLB hit rate: %.3f\n", tlb_hit_rate);

    fclose(addr_file);
    fclose(backing_file);
    return 0;
}
