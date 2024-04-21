// // Pushes a value to the other core. Will block if the FIFO is full.
// void rp2040.fifo.push(uint32_t);

// // Pushes a value to the other core. If the FIFO is full, returns false immediately and doesn’t block. If the push is successful, returns true.
// bool rp2040.fifo.push_nb(uint32_t);

// uint32_t rp2040.fifo.pop();

// // Reads a value from this core’s FIFO. Blocks until one is available.
// bool rp2040.fifo.pop_nb(uint32_t *dest);

// // Reads a value from this core’s FIFO and places it in dest. Will return true if successful, or false if the pop would block.
// int rp2040.fifo.available();

// volatile char bufferavaliable = 1;

// struct mtbuffer {
//   unsigned long timestampOxy[6] = { 0 };
//   float oxigen[6] = { 0.0f };
//   float tempTube[6] = { 0.0f };
// };

// // struct mtbuffer mtbuffer;


// char writetobuffer(struct packet data) {
//   data.id = 420;
//   return 0;
// }
