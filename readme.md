# OS Scheduling Algorithms

Authors: Tomas Kmet, Meet Bhatt, Youssef Hamed
University of Calgary Project

## Contents
scheduler.c contains three different scheduling algorithms. Shortest Time to Completion First (STCF), RR (Round Robin), and Lottery Scheduling (LT) algorithms are tested using simpulated jobs. The first column of the job represents the arrival time of the job. The second column of the job represents the duration.  

To run the scheduler.c, a makefile is provided.
To test stcf: `./scheduler.out 1 STCF jobs_stcf.in 10`
To test rr: `./scheduler.out 1 RR jobs_rr.in 2`
To test lt: `./scheduler.out 1 LT jobs_rr.in 10`
