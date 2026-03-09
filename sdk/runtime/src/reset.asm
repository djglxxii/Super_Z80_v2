; Reset and IRQ vector table for the minimal SDK runtime.

 ORG SZ_ROM_BASE

sz_runtime_reset_vector:
 jp sz_runtime_reset

 defs SZ_IRQ_VECTOR - $, $00

 ORG SZ_IRQ_VECTOR

sz_runtime_irq_vector:
 jp sz_runtime_irq_handler
