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

With huge frontend stalls for indirect calls.
