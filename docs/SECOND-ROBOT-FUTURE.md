# Second robot future plan

This robot has not been built. These notes preserve the useful parts of the earlier SealBot proposal without treating them as a finished design. The final CareBot already owns the 2, 6, 2 medical-kit route and both containment beams, so the second robot should focus on sample collection and laboratory delivery.

## Proposed job

Collect the three flat samples from the quarantine area and place one sample into each laboratory slot. The Senior-category sample positions may vary, so a fixed timed route alone is unlikely to be enough.

## Proposed mechanism

Use one shallow front tray for one sample at a time. The earlier proposal assumed samples approximately 56 mm in diameter and 5 mm thick, with a tray around 60 mm wide and a 65 to 70 mm entrance. A thin rounded PET lip would collect the disc. A low roof would reject stacked samples, and a light inward flap would retain one disc while reversing.

One positional servo would lift the tray through a short linkage while keeping it roughly level. A second servo would retract or open the tray floor to release the sample above a laboratory slot. Physical stops should define the tray positions so the servos do not hold the full mechanism load continuously.

## Navigation and sensing still to decide

The laboratory holes leave little clearance around a sample, so final placement needs closer sensing than a long timed drive. The older plan suggested a camera for finding samples and aligning to empty laboratory slots, plus a presence sensor to confirm that the tray contains a disc. None of this hardware or software has been selected or tested.

Before choosing a controller, camera, chassis, motors, sensors, battery, or driver, build one tray prototype and test pickup and release with full-size practice pieces. Confirm the current rulebook dimensions on the physical field. The second robot can then reuse compatible CareBot parts where that reduces spares and wiring work.

## Work remaining

1. Confirm the second robot's exact mission from the current rulebook.
2. Build and test one sample tray by hand.
3. Choose the controller and sensing method after measuring the required placement accuracy.
4. Write firmware only after the pickup and release mechanism works repeatedly.
5. Check that both robots fit the starting area and that their routes do not collide.

This file is a future concept, not approved CAD, wiring, firmware, budget, or evidence of a working second robot.
