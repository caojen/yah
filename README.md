# YAH 3.0

## Description

Yah is a tool based on aircrack-ng. It captures the output aircrack-ng(Actually, just the standard output. The standard error has been ignored). In our perious implementations, because of fast refresh in aircrack-ng and needing our program dealing with them as fast as possiable, crashes might occur at running time. In this version, Yah has been updated with these components:

+ LRU cache
+ Formatter
+ Checker
+ Sender
+ Updater

Each of these run in a singel thread(or more threads, based on the configure file).

Formatter: fed with one single line captured from aircrack-ng. Formatter generates this line into AirodumpData format.

Checker: fed with AirodumpData (from Formatter). Firstly, check this data whether is in memory LRU cache. Then, check this data whether is in our localstorage (usually, a database, like sqlite3 or mysql). If the data is presented neither the cache nor the localstorage, checker treats it as 'passed'. Passed data should be stored into localstorage and LRU cache, and should be pushed into sender.

Sender: fed with AirodumpData(s), generates them into strings and encodes them. Then, send to remote server based on configure file. If this action succeeds, sender will push all the data to updater. Otherwise, push back into send queue.

Updater: fed with AirodumpData(s), updates them at localstorage(usually, labels them as 'is_uploaded')

## Install
Just call `make`. Sudo or root should be provided.

After installing, use `systemctl start yah` to start the program.
