# master-control-program
MCP whose job it is to run and schedule a workload of programs on a system. The MCP will read a list of programs (with arguments) to run from a file, startup and run the programs as processes, and then schedule the processes to run concurrently in a time-slicedmanner. In addition, the MCP will monitor the processes, keeping track of how the processes are using system resources.
