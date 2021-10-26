#ifndef BACKGROUND_H
#define BACKGROUND_H

struct bgProcess *initializeBgProcess(void);

void bgProcessInsertFront(struct bgProcess *head, pid_t childPid);

void reapBgProccesses(struct bgProcess *head);

#endif
