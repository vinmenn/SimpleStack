#SimpleStack Serialize/deserialize data into byte arrays or EEProm arranged as FIFO stack

##Description
This library push and pop values from a byte array (or storage with EEProm libray) as a FIFO stack. 
This is useful for transmission / reception routines that have a fixed length fields or configuration routines that needs to read/save
information from permanent storage

##Usage
Writing into stack
Is possible to fill stack with a buffer array and then pop out values one after one,
or as opposite is possible to push values and at the end get entire buffer filled.
See included example.
Is also possible to use EEProm as storage for read/write operations, in this way is possible to serialize/deserialize information
using simple calls with various data types.

