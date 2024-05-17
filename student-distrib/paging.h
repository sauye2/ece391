#ifndef PAGING_H
#define PAGING_H

#include "types.h"
#include "terminal.h"

#define PAGE_STORAGE 1024
#define PAGE_ALIGN 4096
#define VIDTOPHYS 0xB8000
#define KERNELMEMLOC 0x400000
#define VIRTUALMEM (0x3FF000 & VIDTOPHYS)
#define MAX_TERM    3
#define TERMINAL1      VIDTOPHYS + FOURKB
#define TERMINAL2      VIDTOPHYS + FOURKB * 2
#define TERMINAL3      VIDTOPHYS + FOURKB * 3


extern void init_paging();

uint32_t pagedir[PAGE_STORAGE] __attribute__((aligned (PAGE_ALIGN)));
uint32_t pagetab[PAGE_STORAGE] __attribute__((aligned (PAGE_ALIGN)));
uint32_t vidmaptab[PAGE_STORAGE] __attribute__((aligned (PAGE_ALIGN)));
extern inline void asm_pagedir(uint32_t a);
extern inline void flush_tlb();
#endif
