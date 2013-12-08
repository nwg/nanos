#include "intel_8259_nanos.h"

void intel_8259_nanos_init() {

    // Disable all IRQs by default
    intel_8259_set_irq_mask(I8259_MASK_ALL);
}
