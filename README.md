# TCP-Forum-Server

### How to run:
1. Compile TCPServerMultithreaded.exe
2. Compile TCPServerTestHarness.exe
3. Navigate to TCPServerMultithreaded.exe and run "./TCPServerMultithreaded.exe" in the terminal
4. Navigate to TCPServerTestHarness.exe and run "./TCPServerTestHarness ip_address poster_count reader_count duration_secs throttle validate" in the terminal
  - ***ip_address:*** the server's IP
  - ***poster_count:*** the number of poster threads you want to create
  - ***reader_count:*** the number of reader threads you want to create
  - ***duration_secs:*** the duration of the test in seconds
  - ***throttle:*** 0 & 1 are valid, where the test will throttle on 1 and not on 0
  - ***validate:*** 0 & 1 are valid, where the test will validate responses on 1 and not on 0
5. The test will commence and report the results to the console as well as a CSV file located in the same directory as the test harness executable
