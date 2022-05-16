
/* LICENSE: this file is public domain */

#include <assert.h>
#include <cstdint>
#include <inttypes.h>
#include <ostream>
#include <stdio.h>

#include <fstream>

#include "saleae_v2_digital.h"

/* example: NAND with async parallel 8-Bit bus
 * channels 0..7: Data[0..7]
 *
 * NOTE: this is just an example to demonstrate use of
 * logic_init/_replay/_cleanup doing something useful is left as an exercise for
 * the reader ;)
 */

int main(int argc, char **argv) {
  logic_t logic;
  uint32_t prev_state;
  uint64_t ts;
  double tsd;
  int res;

  assert(argc > 1);
  res = logic_init(&logic, argv[1]);
  if (res >= 0) {
    fprintf(stderr, "%d channels found\n", res);
    fprintf(stderr, "initial state: %04x\n", logic.state);
  }

  std::ofstream out("converted.vcd");
  // Generate hardcoded vcd header!
  out << "$timescale\n1ns\n$end\n";
  // Generate for each channel a variable!
  for (int i = 0; i < logic.n_channels; ++i) {
    // use the index as variable identifier and D<index> as its full name
    out << "$var wire 1 " << i << " D" << i << " $end\n";
  }
  out << "$enddefinitions $end\n";
  out << "#1\n";
  out << "$dumpvars\n";
  // print the initial state
  for (int i = 0; i < logic.n_channels; ++i) {
    out << ((logic.state >> i) & 1) << i << "\n";
  }
  out << "$end\n";

  prev_state = logic.state;

  /* trigger on every transition! */
  while (logic_replay(&logic, NULL, &tsd, -1, -1)) {
    /* convert timestamp to integer, 1ns units */
    tsd *= 1000000000.0;
    ts = tsd;

    // print the updated states!
    out << "#" << ts << "\n";
    // print the initial state
    uint32_t stateUpdate = prev_state ^ logic.state;
    for (int i = 0; i < logic.n_channels; ++i) {
      // check if the channel changed and if so print the update!
      if (((stateUpdate >> i) & 1) == 1) {
        out << ((logic.state >> i) & 1) << i << "\n";
      }
    }

    prev_state = logic.state;
  }

  logic_cleanup(&logic);

  return 0;
}
