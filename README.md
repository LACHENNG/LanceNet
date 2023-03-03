# Tiny Web Sever From Scratch

1. concurrent echo server with Select IO multiplexing
 
   a. the main thread  select on read_set to monitor any possible readable events of sockets fds, if listen socket is readalbe, accepts new connection and update read_set. and inform one of the thread in thread pool to do the `echo` job
   the difficulty is how to monitor the events of close when peer close the connect, Which thread updates readsets and closes fd? Because here are serveral race conditions that may occur:
   1) if the peer close the connection,  or a file descriptor being monitored by select()  is  closed  in  another
       thread,  the  result  is  unspecified.   On  some  UNIX systems, select()
       unblocks and returns, with an indication  that  the  file  descriptor  is
       ready  (a subsequent I/O operation will likely fail with an error  
      
