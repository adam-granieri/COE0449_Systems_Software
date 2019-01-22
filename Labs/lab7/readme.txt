Adam Granieri

The original freader.c had the issue of printing out the incorrect amount 
of values due to the simultaneous unlocked and unprotected threads.
While reading back the values from each file in each of the threads, they all
incremented the BufCount global vairable.  Since the execution time of each 
thread relative to one another is almost impossible, there are some times 
when 2 threads are incrementing BufCount simultaneously and since the computer 
can only truly execute one increment at an instant, both end up trying to update
the old value by one.  This causes one of the updates to get lost and not be reflected in
the acutal BufCount variable.  With this repeated multiple times, we end up losing 
a noticable amount, causing the reduced value we see after execution.