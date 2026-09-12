# Two simple robots for a nine-day school build
SealBot and CareBot | Revised 6 September 2026 | Rs 8,790 planned | Rs 10,000 ceiling

## 1. Build the simpler version

Return to the previous design: CareBot delivers kits and carries one patient at a time; SealBot moves one floor-staged beam at a time. Add the missing sample task to SealBot using one shallow tray. Keep the ESP32-CAM approach and avoid the rotary magazine, patient elevator, indexed storage and servo expansion board.

The only new mission mechanism is SealBot's sample tray, with a short lift and an opening floor. It also gets a camera to find the randomly placed samples. CareBot's existing scoop, gate, ejector and three kit chutes stay the same. Both robots use the same basic drive electronics.

This covers every task in the rulebook, but covering every task is different from completing all of them in one two-minute round. Carrying patients individually is easy to build and debug, but costs travel time. The plan supports all twelve patients and the yellow 2/2 split; the number delivered in a match depends on measured trip times. We will not add a complicated sorter to conceal that tradeoff.

Budget Rs 8,790, including shipping and repair allowance. Leave the remaining Rs 1,210 unspent unless a tested problem needs it. Assume borrowed school tools, a laptop, multimeter and a full-size flat practice surface. Four to six students working together, with parts available by day 2, is the basis for the nine-day schedule.

## 2. The whole job, divided between two robots

| Robot | What it does | Mechanisms |
|---|---|---|
| SealBot | Collect three samples, place one in each lab slot, then place both beams and park. | One sample tray; one low beam clamp. |
| CareBot | Deliver six kits to H and two to each PCC; then carry patients individually to their colour destinations. | Three batch chutes; one patient pocket with gate and ejector. |

There are eight working servos across both robots: CareBot has five; SealBot has three. Buy two spare SG90s. There are two cameras, two Nanos, four drive motors and two motor drivers. No turntable, conveyor, sorting bins, patient lift, custom PCB or required 3D print.

The rulebook gives 50 positive points for all samples, 70 for complete containment, 50 for the exact kit distribution and 80 for all patients with their bonuses. The 250-point total is a scoring ceiling, not a promised result.

## 3. CareBot stays almost unchanged

### Deliver the kits in three batches

Make three covered, single-file chutes from smooth plastic or thin plywood lined with PET. Preload six kits for H, two for left PCC and two for right PCC. Each chute has one SG90-operated gate and releases its complete load at its destination. There is no individual-kit counting mechanism.

Kits measure 25 x 25 x 20 mm. Start with 28-30 mm internal channel width. The six-kit lane needs at least 150 mm of usable length, plus gate clearance. Adjust its slope until the actual pieces slide without sticking. Use a mechanical stop or latch to support the load rather than making the servo hold it continuously.

Deliver all ten kits first. Stop with the outlet well inside each destination, open its gate, let the batch clear, close it and reverse away. The whole robot does not need to enter the shallow healthcare zone. Test that released kits stay inside and do not block later patient drops.

### Keep one patient in one pocket

Use the previous shallow front scoop: a rounded 0.3-0.5 mm PET lip, a 45-50 mm gathering mouth and a 24-26 mm pocket for the 20 mm diameter patient. One SG90 closes the front gate. Another drives a short padded ejector so release does not depend on gravity alone. Add a light presence switch.

The patient stays in this pocket from pickup to delivery. After capture, stop and inspect it under a fixed dark hood with diffused white LEDs. Red goes to H, green to RZ inside start, and yellow to a PCC. At the destination, open the gate, extend the ejector, retract it and reverse clear.

Do not start another pickup while the pocket is occupied. If capture is partial, try one gentle reseating movement. If colour remains unclear after one repeat, retain the patient and stop the patient routine rather than guessing its destination.

### Add the missing full patient sequence in software

Visit the twelve sticker positions, checking each pickup with the presence switch. Page 6 explicitly says each patient starts on a sticker; page 16 shows six sites on each side. For Senior, read the colour each time instead of assuming a fixed colour order. Use a small local approach correction if the scoop misses the centre.

Maintain only a visited-site list and two yellow-delivery counters. Send yellow patients to the nearer PCC while that PCC has fewer than two confirmed yellow deliveries; once it has two, use the other PCC. This produces the required 2/2 split without storage or extra mechanics. Increment a counter only after a successful release inside the zone.

Continue with the next patient only when enough time remains for its worst measured delivery trip plus a safety margin. The software contains all twelve visits; the match deadline can stop the sequence early. That is the honest limit of the simple one-patient design.

## 4. SealBot gains one sample tray

### Pick up one disc at a time

Samples are 56 mm diameter and 5 mm thick. Make one shallow tray with approximately 60 mm internal width, a 65-70 mm entrance and a thin rounded PET scoop lip. A low roof, initially 6-7 mm above the floor, prevents stacked samples entering together. A light inward-opening PET flap retains the disc when reversing.

One MG90S positional servo moves the tray through a short four-bar linkage, keeping it roughly horizontal. It only lifts the disc enough to travel and clear the 3 mm laboratory panel. Use a spring or rubber band to counterbalance the empty tray and physical stops to establish repeatable positions.

One SG90 retracts the tray floor to drop the sample. Make the floor slide sideways with approximately 65 mm travel through a crank linkage, enough to clear the disc. Close the floor before lowering for another pickup. Build this as a removable front attachment, with the beam clamp at the rear so the mechanisms do not compete for the same space.

SealBot's second ESP32-CAM looks at the floor ahead. Detect a sample's dark circular outline within quarantine, centre the scoop and approach slowly. Ignore the internal printed symbol and long black tape shapes. Verify that the disc is fully in the tray before lifting. Test samples near both walls and in corners; do not rely on a blind wide sweep to collect all three.

### Drop it into an empty laboratory slot

Use the horizontal laboratory layout depicted in the assembled-field drawings on pages 13 and 15. The panel is 440 x 150 x 3 mm with three 60 mm holes. Hole spacing is not dimensioned, so locate the holes with the camera and calibrate their positions on the practice field instead of copying distances from the sketch.

The nominal clearance is only 2 mm around a centred 56 mm disc. Stop, observe an empty hole with the tray lifted clear, then move the tray to its calibrated release position and open its floor. Drop from just above the panel. The disc can hide the hole, so camera alignment happens before moving the tray over it. Test linkage play as part of the placement error.

Wheel encoders alone are not accurate enough for this final step. Use a close camera view and small corrections; target less than 1 mm centring error on the fixture. The robot corrects sideways error by a short turn-drive-turn movement, not by sliding sideways. Verify the tray is empty and the disc is seated before marking that slot used.

Repeat for the other two slots. This is the hardest addition to the simple build, so make and test it on day 1. A disc sitting somewhere in the laboratory zone is not the same as a disc inside its slot.

### Then use the previous beam clamp

Keep both beams upright on the starting-area floor. After all samples are delivered, return for one beam at a time. The rear U-shaped clamp has padded contacts and one SG90 controlling linked retaining fingers. The board carries the beam's weight; the robot slides it slowly without lifting it.

Apply force near the base, initially 5-10 mm above the floor, and avoid sharp turns. At the target, align with the required wall and boundary, release the fingers fully and withdraw normal to the beam face. Nothing remains under or against the beam. Place the second beam without entering the enclosure or disturbing the first, then park clear of both beams and RZ.

The source contains a dimensional problem: page 15 labels 280 mm to the inside of the tape, while a 250 mm beam plus the other beam's 20 mm thickness reaches 270 mm in the ordinary right-angle arrangement. The hardware can position both pieces, but the document cannot promise that contradictory dimensions satisfy every contact rule. Reproduce the supplied dimensions on the practice corner and record the actual mismatch; do not modify game pieces to hide it.

## 5. Fit, routes and the two-minute limit

Keep the previous starting arrangement. Reserve a 300 x 60 mm strip at the back of the 480 x 280 mm start for two upright beams with about 20 mm between them. Place both robots in front of that strip, each at most 220 x 200 mm including stowed attachments, with a 10 mm gap. The nominal occupied envelope is 450 x 270 mm.

Both robots face out of start. Fold SealBot's tray within its starting envelope. Check the real loaded arrangement with a cardboard inspection frame, including all ten kits, wheels, wires and decoration. A stock chassis may need a shorter offcut deck. Once carrying a beam, SealBot's width includes the full beam, not just its wheels.

CareBot works mainly in the upper and middle field; SealBot handles quarantine and the nearby laboratory in the lower field. Keep the beam staging strip toward the back-left of start and reserve a separate front-right landing patch for green patients. CareBot should eject into that patch from the start boundary where its outlet reach permits, avoiding a drive across the beam storage area.

Check those routes with the real loaded robots. If their approaches overlap, one waits or stops on an obstruction; a timed delay alone does not prove the corridor is clear. Do not impose a long mandatory wait before every green delivery. Keep delivered green patients out of SealBot's second-beam pickup path and both robots' final parking positions.

| Robot | Normal sequence | Timing consequence |
|---|---|---|
| SealBot | Three sample-to-lab trips; two beam trips; park. | Measure both task groups together. Adding samples changes the earlier beam-only schedule. |
| CareBot | Three kit stops; repeat pickup, inspect and direct delivery. | At 35-45 seconds for kits and 15-20 seconds per patient, only a few patients fit. These are planning examples, not test results. |

Twelve patient trips in the remaining 75 seconds would average 6.25 seconds each, including pickup, colour reading, travel and release. Do not assume that performance from a beginner build. Shorten routes after measuring them, but keep the simple mechanism. Every long action has a timeout and at most one retry. Both robots stop all actuation at 118 seconds, independently of camera responses.

## 6. Keep the electronics familiar

Each robot uses one Nano, one ESP32-CAM, two BO drive motors, one DRV8833, two wheel pulse sensors, three selected channels of a line array, bumpers and an accessible power-stop switch. The Nano controls the robot; the camera returns a small result over UART. No wireless connection or computer is needed during a round.

For CareBot, capture a fixed crop of the stopped patient under white LEDs and classify calibrated red, yellow or green pixels. Start with QVGA JPEG and local decoding on the camera's PSRAM, one frame buffer, and Wi-Fi disabled. Require agreement across fresh frames and return UNKNOWN on an empty pocket, conflicting result or two-second timeout. Tune on actual painted pieces and validate with separate captures. See the [official Espressif camera driver](https://github.com/espressif/esp32-camera).

SealBot uses its camera for sample outlines and laboratory alignment. It does not need a colour sorter or a second processor beyond the same Nano/camera pair. Use separate camera modes for finding a sample, verifying the tray and finding an empty hole. Prototype the camera view with the actual tray before fixing the mount.

| Nano pins | CareBot | SealBot |
|---|---|---|
| D0 / D1 | Camera UART | Camera UART |
| D2 / D3 | Wheel pulse inputs | Wheel pulse inputs |
| D4/D5 and D7/D6 | Motor input pairs; PWM on D5/D6 | Same |
| D8 / D9 / D10 | Three kit gates | Sample lift / sample floor / beam clamp |
| D11 / A3 | Patient gate / ejector | Spare |
| D12 / D13 | Start button / driver sleep | Same |
| A0 / A1 / A2 | Three line channels | Same |
| A4 / A5 | Left/right bumper | Same |
| A6 / A7 | Pocket presence / battery divider | Tray presence / battery divider |

A6/A7 are analog-only on the classic Nano and need appropriate external resistors. Keep motor PWM off D9/D10 because the AVR Servo library uses Timer1. Keep the normal Timer0 timing on D5/D6. Driver sleep must default low during boot. Check the module against the [TI DRV8833 datasheet](https://www.ti.com/lit/ds/symlink/drv8833.pdf), whose current ratings depend on package.

Send camera requests with an ID and accept only the matching response, so an old colour cannot classify the next patient. Use disconnectable UART links for programming. Nano TX is 5 V and needs a divider or level shifter into the ESP32's 3.3 V GPIO. GPIO13/14 are candidate UART pins on an AI-Thinker camera board with microSD disabled; verify the actual board. Power the camera at its specified 5 V input.

Use one protected, preassembled 2S pack per robot and a compatible 8.4 V CC/CV charger. Confirm at least 3 A continuous pack discharge capability. Use three regulated 5 V branches per robot for logic, drive motors and servos, with common grounds, suitable capacitors and motor suppression. This modest power improvement keeps motor starts away from the camera supply and avoids putting 8.4 V directly on low-voltage BO motors.

Set regulators before connecting boards and test voltage under real loads. Their advertised maximum current is not a guarantee. Fit a fuse, insulate connectors and secure batteries. The stop switch physically disconnects power; gates retain pieces mechanically when unpowered. After reset, wait for a new start action rather than resuming with lost state. Do not use a single-cell TP4056 charger for a 2S pack.

## 7. Budget for both robots

These are tax-inclusive planning allowances, not a completed supplier cart. The camera allowance uses the checked Rs 610 plus 18 percent GST listing, or about Rs 720 each. Other references were inspected on 5-6 September 2026. Verify exact contents, stock and delivered cost before buying.

| Item | Quantity / allowance | Total Rs |
|---|---|---|
| Complete 2WD chassis kits | 2 x 350 | 700 |
| Nano CH340 boards | 2 x 200 | 400 |
| ESP32-CAM with OV2640 and PSRAM | 2 x 720 | 1440 |
| Shared USB-to-TTL programmer | 1 | 150 |
| DRV8833 motor drivers | 2 x 80 | 160 |
| SG90 positional servos | 9 x 100; seven working, two spare | 900 |
| MG90S positional servo | 1 sample lift | 200 |
| Five-channel line arrays | 2 x 190 | 380 |
| Wheel pulse sensors and discs | Four sensing positions | 200 |
| Protected 2S packs | 2 x 600; verify current rating | 1200 |
| Compatible 8.4 V charger | 1 shared | 650 |
| Buck regulators | 6 x 60 | 360 |
| Electrical consumables | Stops, fuse, presence switches, wire, LEDs, connectors and capacitors | 450 |
| Mechanism materials | Offcuts, PET, linkages, fasteners and local cutting | 500 |
| Practice pieces and tape | Full-size base and tools borrowed | 300 |
| Shipping / local collection | Consolidated orders | 400 |
| Repair reserve | In addition to spare servos | 400 |
| Planned total | Both complete robots and shared supplies | 8790 |

The remaining Rs 1,210 under the Rs 10,000 ceiling is headroom, not a reason to add features. Battery and charger costs remain allowances for a verified compatible pair. If school tools and a practice surface cannot be borrowed, include their real cost before calling this a complete budget.

Price references: [Directronics camera](https://directronics.in/product/esp32-cam-wifi-module-camera-module), [Probots chassis](https://probots.co.in/2wd-clear-acrylic-smart-robot-chassis-car-kit.html), [Robocraze servos](https://robocraze.com/collections/servo-motors-controllers), [Tomson line array](https://www.tomsonelectronics.com/products/5-channel-line-tracking-sensor-module-bfd-1000), [Hubtronics wheel sensor](https://hubtronics.in/lm393-speed-measuring-sensor-module), and [KTRON regulator](https://www.ktron.in/product/lm2596-step-down-module/). Rounded allowances include small substitutions; no orders have been placed.

## 8. Nine days, with the difficult part first

1. Day 1: make the legal start-size mock-up and the single sample tray. Reproduce a 56 mm disc and 60 mm lab hole. Check the lift and drop by hand before mounting anything on a chassis. Collect available parts.
2. Day 2: bring up both Nanos and cameras. Demonstrate reliable colour readings in CareBot's pocket and repeated sample drops in SealBot's fixture. Fix the tray now if it jams or misses.
3. Day 3: finish the three kit chutes, patient gate/ejector and low beam clamp. Repeat each release on the bench with real-size pieces. Wire and load-test both power systems.
4. Day 4: calibrate driving, turns, line references and slow docking. Test corner sample pickups and the three distinct lab slots. Check the complete starting arrangement again.
5. Day 5: run SealBot's sample-and-beam sequence and CareBot's kit route plus one patient trip. Record actual times. Add the remaining sticker visits and yellow counters using the same repeated routine.
6. Day 6: run both robots together. Fix overlapping approaches and add missing-line, camera-timeout, stalled-movement and deadline stops. Freeze mechanisms at the end of the day.
7. Day 7: run full two-minute rounds with random patient colours and different sample positions. Fix the largest repeated failure; do not add new hardware.
8. Day 8: repeat full rounds at different battery levels and lighting. Record final pieces and score, not merely completed waypoints. Select a match sequence supported by those results.
9. Day 9: rehearse loading and start, charge packs, secure wiring, add health/SDG3 decoration, save working firmware and pack spares. Make calibration changes only.

Before trusting a mechanism, aim for twenty consecutive correct bench cycles. Test sample placement at each hole, not just the easiest one. For the camera, include empty pockets, partial pickups and strong side light; a wrong accepted colour is worse than UNKNOWN. Deliberately delay one robot to test the shared start approach. These checks expose faults; they do not prove a failure-free competition run.

The supplied Senior penalties conflict: page 21 lists -3 for patients outside their correct destination, while page 23 lists -5 for incorrect placement and -5 for samples left in quarantine. Calculate both interpretations when reviewing practice scores. Both robots must have accessible stops, rounded edges, secured wiring and SDG3 decoration, and no setup jig remains on the field. At most three students enter the competition area.

This is a simplified build specification. It restores the previous mechanisms and adds sample handling plus the complete patient sequence. It is not uploaded firmware, tested CAD or evidence that every mission finishes within 120 seconds.
