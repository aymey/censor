# censor
benchmarker/profiler

# how it works
not entirely sure how this is usually done so it just injects timer code for each function

make sure that the injectee as read and write permissions i.e
```sh
$ chmod +rw injectee
```

# roadmap
aiming to support:
- [ ] time taken and memory usage of specific or all functions
- [ ] static modification
- [ ] dynamic/runtime injection
- [ ] chrome & firefox profiler formatted output
