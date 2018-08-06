// PageTable.c ... implementation of Page Table operations
// COMP1521 17s2 Assignment 2
// Written by John Shepherd, September 2017

#include <stdlib.h>
#include <stdio.h>
#include "Memory.h"
#include "Stats.h"
#include "PageTable.h"

// Symbolic constants

#define NOT_USED 0
#define IN_MEMORY 1
#define ON_DISK 2

// PTE = Page Table Entry

typedef struct {
   char status;      // NOT_USED, IN_MEMORY, ON_DISK
   char modified;    // boolean: changed since loaded
   int  frame;       // memory frame holding this page
   int  accessTime;  // clock tick for last access
   int  loadTime;    // clock tick for last time loaded
   int  nPeeks;      // total number times this page read
   int  nPokes;      // total number times this page modified
   // TODO: add more fields here, if needed ...
   int prev;
   int next;
} PTE;

// The virtual address space of the process is managed
//  by an array of Page Table Entries (PTEs)
// The Page Table is not directly accessible outside
//  this file (hence the static declaration)

static PTE *PageTable;      // array of page table entries
static int  nPages;         // # entries in page table
static int  replacePolicy;  // how to do page replacement
static int  fifoList;       // index of first PTE in FIFO list
static int  fifoLast;       // index of last PTE in FIFO list

// Forward refs for private functions

static int findVictim(int);






// initPageTable: create/initialise Page Table data structures

void initPageTable(int policy, int np)
{
   PageTable = malloc(np * sizeof(PTE));
   if (PageTable == NULL) {
      fprintf(stderr, "Can't initialise Memory\n");
      exit(EXIT_FAILURE);
   }
   replacePolicy = policy;
   nPages = np;
   fifoList = NONE;
   fifoLast = NONE;
   for (int i = 0; i < nPages; i++) {
      PTE *p = &PageTable[i];
      p->status = NOT_USED;
      p->modified = 0;
      p->frame = NONE;
      p->accessTime = NONE;
      p->loadTime = NONE;
      p->nPeeks = p->nPokes = 0;
      p->prev = p->next = NONE;
   }
}






// requestPage: request access to page pno in mode
// returns memory frame holding this page
// page may have to be loaded
// PTE(status,modified,frame,accessTime,nextPage,nPeeks,nWrites)

int requestPage(int pno, char mode, int time)
{
   if (pno < 0 || pno >= nPages) {
      fprintf(stderr,"Invalid page reference\n");
      exit(EXIT_FAILURE);
   }

   PTE *p = &PageTable[pno];
   int fno; // frame number
   switch (p->status) {
   case NOT_USED:
   case ON_DISK:
      // TODO: add stats collection
      countPageFault();
      fno = findFreeFrame();
      if (fno == NONE) {
         int vno = findVictim(time);
#ifdef DBUG
         printf("Evict page %d\n",vno);
#endif
         // TODO:
         // if victim page modified, save its frame
         PTE * vp = &PageTable[vno];
         if(vp->modified) saveFrame(vp->frame);
         // collect frame# (fno) for victim page
         fno = vp->frame;
         // update PTE for victim page
         vp->prev = vp->next = NONE;
         // - new status
         vp->status = ON_DISK;
         // - no longer modified
         vp->modified = 0;
         // - no frame mapping
         vp->frame = NONE;
         // - not accessed, not loaded
         vp->accessTime = NONE;
         vp->loadTime = NONE;
         // - no prev and next
         vp->prev = NONE;
         vp->next = NONE;
      }
      printf("Page %d given frame %d\n",pno,fno);
      // TODO:
      // load page pno into frame fno
      loadFrame(fno, pno, time);
      // update PTE for page
      // - new status
      p->status = IN_MEMORY;
      // - not yet modified
      p->modified = 0;
      // - associated with frame fno
      p->frame = fno;
      // - update times
      p->accessTime = p->loadTime = time;
      
      // update the PTEs in PageTable
      if (fifoList == NONE && fifoLast == NONE)
      {
          // first time to add element in the list
          fifoList = pno;
          fifoLast = pno;
          p->prev = NONE;
          p->next = NONE;
      }
      else // not first time to add element in the list
      {
          PTE * pLast = &PageTable[fifoLast];
          pLast->next = pno;
          p->prev = fifoLast;
          p->next = NONE;
          fifoLast = pno;
      }
      break;
   case IN_MEMORY:
      // TODO: add stats collection
      countPageHit();
      if(replacePolicy == REPL_LRU) 
      {
          if(p->prev!=NONE && p->next!=NONE)
          {
              // page pno is not first nor last
              PTE * pPrev = &PageTable[p->prev];
              PTE * pNext = &PageTable[p->next];
              pPrev->next = p->next;
              pNext->prev = p->prev;
              PTE * pLast = &PageTable[fifoLast];
              pLast->next = pno;
              p->prev = fifoLast;
              fifoLast = pno;
              p->next = NONE;
          }
          else if(p->prev!=NONE && p->next==NONE)
          {
              // page pno is not first but last
              // do nothing
          }
          else if (p->prev==NONE && p->next!=NONE)
          {
              // page pno is not last but first
              PTE * pNext = &PageTable[p->next];
              pNext->prev = NONE;
              fifoList = p->next;
              PTE * pLast = &PageTable[fifoLast];
              pLast->next = pno;
              p->prev = fifoLast;
              fifoLast = pno;
              p->next = NONE;
          }
          else
          {
              // page pno is first and last at same time
              // do nothing
          }
      }
      break;
   default:
      fprintf(stderr,"Invalid page status\n");
      exit(EXIT_FAILURE);
   }
   if (mode == 'r')
      p->nPeeks++;
   else if (mode == 'w') {
      p->nPokes++;
      p->modified = 1;
   }
   p->accessTime = time;
   
   return p->frame;
}






// findVictim: find a page to be replaced
// uses the configured replacement policy

static int findVictim(int time)
{
   int victim = 0;
   victim = fifoList;
   PTE * vp = &PageTable[victim];
   // update new first PTE in PageTable
   PTE * pNewFirst = &PageTable[vp->next];
   fifoList = vp->next;
   pNewFirst->prev = NONE;
   switch (replacePolicy) {
   case REPL_LRU:
      // TODO: implement LRU strategy
      // LRU and FIFO no difference in this function
      // just pick up the first element as they are 
      // already sorted in accessTime
      break;
   case REPL_FIFO:
      // TODO: implement FIFO strategy
      // LRU and FIFO no difference in this function,
      // just pick up the first element as they are 
      // already sorted in loadTime
      break;
   case REPL_CLOCK:
      return 0;
   }

   return victim;
}

// showPageTableStatus: dump page table
// PTE(status,modified,frame,accessTime,nextPage,nPeeks,nWrites)

void showPageTableStatus(void)
{
   char *s;
   printf("%4s %6s %4s %6s %7s %7s %7s %7s\n",
          "Page","Status","Mod?","Frame","Acc(t)","Load(t)","#Peeks","#Pokes");
   for (int i = 0; i < nPages; i++) {
      PTE *p = &PageTable[i];
      printf("[%02d]", i);
      switch (p->status) {
      case NOT_USED:  s = "-"; break;
      case IN_MEMORY: s = "mem"; break;
      case ON_DISK:   s = "disk"; break;
      }
      printf(" %6s", s);
      printf(" %4s", p->modified ? "yes" : "no");
      if (p->frame == NONE)
         printf(" %6s", "-");
      else
         printf(" %6d", p->frame);
      if (p->accessTime == NONE)
         printf(" %7s", "-");
      else
         printf(" %7d", p->accessTime);
      if (p->loadTime == NONE)
         printf(" %7s", "-");
      else
         printf(" %7d", p->loadTime);
      printf(" %7d", p->nPeeks);
      printf(" %7d", p->nPokes);
      printf("\n");
   }
}
