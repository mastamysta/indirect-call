# Indirect Calls

The idea with this repo was to have a play with indirect calls and see if I could find a measurable disadvantage to using them. To do so, the example program calls a function repeatedly. The control program calls the function directly by statically linking with the function, while the test program contains the called function within a shared object. On Linux dynamically linked functions are implemented using the PLT (by default) which requires an indirect jump and an indirect call.

We expect to see significant performance degradation due to indirect calls (and jumps in this instance) through several mechanisms:
- The compiler cannot optimise across indirect calls.
- The CPU may mispredict branch targets causing pipelines to stall.

Confusingly, the BTB should have a decent chance at learning the correct branch targets since we repeatedly perform the same jumps from the same address here... so whats going on when we see:

Direct calls:
```
          2,675.02 msec task-clock                       #    1.000 CPUs utilized             
                14      context-switches                 #    5.234 /sec                      
                 0      cpu-migrations                   #    0.000 /sec                      
                50      page-faults                      #   18.691 /sec                      
    10,964,123,292      cycles                           #    4.099 GHz                         (71.42%)
         7,012,934      stalled-cycles-frontend          #    0.06% frontend cycles idle        (71.44%)
    33,024,393,380      instructions                     #    3.01  insn per cycle            
                                                  #    0.00  stalled cycles per insn     (71.44%)
     5,004,837,567      branches                         #    1.871 G/sec                       (71.44%)
           104,151      branch-misses                    #    0.00% of all branches             (71.44%)
    20,013,206,867      L1-dcache-loads                  #    7.482 G/sec                       (71.42%)
           158,012      L1-dcache-load-misses            #    0.00% of all L1-dcache accesses   (71.39%)
   <not supported>      LLC-loads                                                             
   <not supported>      LLC-load-misses                                                       

       2.676237211 seconds time elapsed

       2.674943000 seconds user
       0.000999000 seconds sys
```

Indirect calls:
```
          3,724.09 msec task-clock                       #    1.000 CPUs utilized             
                 6      context-switches                 #    1.611 /sec                      
                 1      cpu-migrations                   #    0.269 /sec                      
                58      page-faults                      #   15.574 /sec                      
    14,207,547,006      cycles                           #    3.815 GHz                         (71.43%)
     3,088,178,902      stalled-cycles-frontend          #   21.74% frontend cycles idle        (71.43%)
    37,046,585,921      instructions                     #    2.61  insn per cycle            
                                                  #    0.08  stalled cycles per insn     (71.43%)
     7,005,968,838      branches                         #    1.881 G/sec                       (71.43%)
           137,459      branch-misses                    #    0.00% of all branches             (71.43%)
    22,005,883,331      L1-dcache-loads                  #    5.909 G/sec                       (71.43%)
           120,039      L1-dcache-load-misses            #    0.00% of all L1-dcache accesses   (71.43%)
   <not supported>      LLC-loads                                                             
   <not supported>      LLC-load-misses                                                       

       3.724825020 seconds time elapsed

       3.722826000 seconds user
       0.001999000 seconds sys
```

We see significant frontend stalls with the indirect calling regime, but few branch mispredictions.

Digging deeper into this with perf we can see:

Direct calls:
```
3K branch-misses                                                                                                              ◆
1K L1-dcache-load-misses                                                                                                      ▒
11K cycles 
```

Branch misses:
```
Samples: 3K of event 'branch-misses', Event count (approx.): 263228
Overhead  Command         Shared Object         Symbol
  31.21%  good_branching  good_branching        [.] func(unsigned int, unsigned int)
  18.94%  good_branching  [unknown]             [k] 0xffffffffb02c0d53
  13.62%  good_branching  [unknown]             [k] 0xffffffffb02c4b06
   9.55%  good_branching  good_branching        [.] goodcall(unsigned int, unsigned int)
   7.66%  good_branching  good_branching        [.] main
```

DCache misses:
```
Samples: 1K of event 'L1-dcache-load-misses', Event count (approx.): 330039
Overhead  Command         Shared Object         Symbol
  24.38%  good_branching  [unknown]             [k] 0xffffffffb02c0d53
  13.19%  good_branching  good_branching        [.] func(unsigned int, unsigned int)
  12.44%  good_branching  [unknown]             [k] 0xffffffffb02c4b06
   6.15%  good_branching  [unknown]             [k] 0xffffffffb02c4919
   4.76%  good_branching  good_branching        [.] main
```

Indirect calls:
```
4K branch-misses                                                                                                              ◆
3K L1-dcache-load-misses                                                                                                      ▒
12K cycles  
```

The branch misses are predominantly upon the indirect call:
```
Samples: 4K of event 'branch-misses', Event count (approx.): 307754
Overhead  Command        Shared Object         Symbol
  34.65%  bad_branching  bad_branching         [.] func(unsigned int, unsigned int)
  14.29%  bad_branching  [unknown]             [k] 0xffffffffb02c0d53
  12.85%  bad_branching  [unknown]             [k] 0xffffffffb02c4b06
  11.89%  bad_branching  bad_branching         [.] badcall(unsigned int, unsigned int)
   9.87%  bad_branching  bad_branching         [.] main
```

But most interestingly of all, there are significant L1 data cache misses occurring upon the function pointer:
```
Samples: 3K of event 'L1-dcache-load-misses', Event count (approx.): 509476
Overhead  Command        Shared Object     Symbol
  27.06%  bad_branching  bad_branching     [.] func(unsigned int, unsigned int)
  15.71%  bad_branching  [unknown]         [k] 0xffffffffb02c0d53
  11.46%  bad_branching  bad_branching     [.] badcall(unsigned int, unsigned int)
   9.82%  bad_branching  [unknown]         [k] 0xffffffffb02c4b06
   8.20%  bad_branching  [unknown]         [k] 0xffffffffb02c4919
   6.31%  bad_branching  bad_branching     [.] main
```

So we're seeing significant delays due to data cache misses when using indirect calling. Roughly 2.5-3x as many dcache misses.
